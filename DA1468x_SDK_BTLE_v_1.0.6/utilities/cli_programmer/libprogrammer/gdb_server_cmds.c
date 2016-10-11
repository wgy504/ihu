/**
 ****************************************************************************************
 *
 * @file gdb_server_cmds.c
 *
 * @brief GDB Server interface
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#ifdef WIN32
#include <winsock.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#define inline __inline
#define sleep(s) Sleep((s) * 1000)
#define strdup _strdup
#define socket_error() WSAGetLastError()
#ifdef EADDRINUSE /* Occasional conflict with POSIX error code in errno.h */
#undef EADDRINUSE
#endif
#define EADDRINUSE WSAEADDRINUSE
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#define closesocket close
#define socket_error() errno
#endif
#include "programmer.h"
#include "gdb_server_cmds.h"

#define ACK '\x06'
#define NAK '\x15'

#define GDB_RECONNECT_MAX_TIME_MS       2000
#define GDB_SERVER_CLOSE_MAX_TIME_MS    4000
#define GDB_LOCAL_CMD_EXE               "qRcmd"
#define GDB_CONTINUE                    "$c#63"
#define GDB_STEP                        "$s#73"
#define SOH                             '\x01'
/*
 * Buffer should contain 64kB of data (size of OTP memory) encoded 2 bytes per each data byte and
 * additional frame characters
 */
#define MAX_BUF_LEN                     132000
#define MAX_READ_SIZE                   0x10000
#define ADDRESS_TMP                     (0xFFFFFFFF)
#define GDB_SERVER_REPEATS_LIMIT        3
#define DBG_GDB_SERVER                  0
#define ACK_CHAR                        '+'
#define NACK_CHAR                       '-'
#define CHUNK_SIZE                      0x1000

typedef struct {
        uint32_t run_swd_addr;
        uint32_t cmd_num_addr;
        uint32_t cmd_buf_addr;
        uint32_t buf_addr;
        uint32_t ack_nak_addr;
} swd_if_addr_t;

typedef struct {
        uint32_t len;
        uint8_t buf[MAX_BUF_LEN];
} recv_buf_t;

const char marker[] = "DBGP";

static uint8_t *boot_loader_code;
static size_t boot_loader_size;

/* struct with swd important addresses in uartboot */
static swd_if_addr_t swd_addr;
/* local copy of command number from uartboot */
static uint32_t cmd_num_cp;
/* GDB Server socket */
static int gdb_server_sock;
/* buffer for GDB Server responses */
static recv_buf_t gdb_server_recv_buf = {/* .len = */ 0};
/* flag indicates that the uartboot code was loaded on platform */
static bool uartboot_loaded = false;
/* GDB Server configuration */
static prog_gdb_server_config_t configuration;
#ifdef WIN32
/* GDB Server handle */
static DWORD gdb_server_pid;
static HANDLE gdb_server_handle = INVALID_HANDLE_VALUE;
#else
/* GDB Server PID */
static pid_t gdb_server_pid = -1;
#endif
/* Target reset command */
extern char *target_reset_cmd;

#ifdef WIN32
typedef ULONGLONG time_ms_t;
#define gdb_server_get_current_time_ms GetProcAddress(GetModuleHandle("kernel32.dll"), "GetTickCount64")
#else
typedef long long time_ms_t;
static time_ms_t gdb_server_get_current_time_ms(void)
{
        struct timespec spec;

        clock_gettime(CLOCK_REALTIME, &spec);
        return spec.tv_sec * 1000 + spec.tv_nsec / 1000000;

}
#endif

/* set fields in swd_addr structure */
static void set_swd_addresses(void)
{
        uint32_t i;

        /* find marker "DBGP" in uartboot code */
        for (i = 0; i < boot_loader_size - 4; i++) {
                if (!memcmp(&boot_loader_code[i], marker, 4)) {
                        swd_addr.run_swd_addr = i + 4;
                        break;
                }
        }

        swd_addr.cmd_num_addr = swd_addr.run_swd_addr + sizeof(uint32_t);
        swd_addr.cmd_buf_addr = *((uint32_t *) &boot_loader_code[swd_addr.cmd_num_addr +
                                                                          sizeof(uint32_t)]);
        /* on 680 platform is 32-bits addressing */
        swd_addr.buf_addr = *((uint32_t *) &boot_loader_code[swd_addr.cmd_num_addr +
                                                                             sizeof(uint32_t) * 2]);

        swd_addr.ack_nak_addr = swd_addr.cmd_num_addr + 3 * sizeof(uint32_t);
}

/* save memory to file specified by file_path */
static int memory_to_file(const char *file_path, uint8_t *memory, uint32_t len)
{
        FILE *file = fopen(file_path, "wb");

        if (!file) {
                return ERR_FILE_OPEN;
        }

        if (fwrite(memory, 1, len, file) == -1) {
                fclose(file);
                return ERR_FILE_WRITE;
        }

        return (!fclose(file) ? 0 : ERR_FILE_CLOSE);
}

/* calculation of checksum from data */
static uint8_t checksum(const char *data, uint32_t len)
{
        uint8_t cs = 0;

        for (; len > 0; len--) {
                cs += data[len - 1];
        }

        return cs;
}

/*
 * create uint8_t from 2 characters, if cannot convert characters to number set status
 * to false and return 0xFF, otherwise return converted value and set status to true
 */
static inline uint8_t chars_to_uint8(const char up_char, const char low_char, bool *status)
{
        char digit_char[] = {up_char, low_char, '\0'};

        if (!(isxdigit(up_char) && isxdigit(low_char))) {
                *status = false;
                return 0xFF;
        }

        *status = true;
        return (uint8_t) strtoul(digit_char, NULL, 16);
}

/* check that a host_name is the localhost */
static bool is_localhost(const char *host_name)
{
        uint32_t addr = 0;

        /* try resolve host_name as address */
        if ((addr = inet_addr(host_name)) == INADDR_NONE) {
                /* get address from host_name*/
                struct hostent *host = gethostbyname(host_name);

                addr = *(uint32_t *) host->h_addr_list[0];
        }

        return htonl(addr) == INADDR_LOOPBACK;
}

/* get program name from specified path */
static void get_name_from_path(const char *path, char *name_buf)
{
#ifdef WIN32
        char path_buf[MAX_PATH];
        const char *end;
        const char *ext;
        if (*path == '"') {
                path++; /* Skip " */
                end = strchr(path, '"');
        } else {
                end = strchr(path, ' ');
        }
        if (end != NULL) {
                /* Copy till space or " */
                strncpy(path_buf, path, end - path);
                path_buf[end - path] = '\0';
        } else {
                strcpy(path_buf, path);
        }
        /* Copy just file name, discard folder if any */
        strcpy(name_buf, PathFindFileName(path_buf));

        /* Make sure file name ends with .exe since this always should be an executable */
        ext = PathFindExtension(name_buf);
        if (ext == NULL || strcasecmp(ext, ".exe")) {
                strcat(name_buf, ".exe");
        }
#else
        uint16_t i;
        uint16_t last_slash_pos = 0;
        uint16_t name_length = 0;

        /* find start of program name in path */
        for (i = 0; i < strlen(path) - 1; i++) {
                if (path[i] == '/') {
                        last_slash_pos = i;
                }

                /* space in file path */
                if (path[i] == ' ' && memcmp(&path[i-1], "\\ ", 2)) {
                        break;
                }
        }

        /* copy name from path */
        name_length = i - (last_slash_pos + 1);
        memcpy(name_buf, &path[last_slash_pos + 1], name_length);
        name_buf[name_length] = '\0';
#endif
}

/* create temporary file name */
static int get_tmp_file_name(char *name)
{
#ifdef WIN32
        char path[MAX_PATH];

        if (!GetTempPath(MAX_PATH, path) || !GetTempFileName(path, "", 0, name)) {
                return ERR_FILE_OPEN;
        }

        return 0;
#else
        /* The last six characters must be XXXXXX and will be replaced with a string
         * that makes the filename unique.
         */
        char template[] = "/tmp/cli_programmer_XXXXXX";
        memcpy(name, template, sizeof(template));
        return (mkstemp(name) ? 0 : ERR_FILE_OPEN);
#endif
}

#ifndef WIN32
static int get_gdb_server_pid(const char *gdb_cmdline, int port)
{
        char netstat_cmd[100];  /* netstat command buffer */
        char gdb_name[300];     /* expected gdbserver name taken from gdb_cmdline */
        char result_name[300];  /* file name where netstat, awk result are redirected */
        int pid = -1;
        FILE *f;
        char *owning_proc = NULL; /* process name that owns listening port */

        get_tmp_file_name(result_name);
        get_name_from_path(gdb_cmdline, gdb_name);

        /*
         * netstat for listening tcp v4 ports numeric display with process name
         * awk filters by port name prints pid/processname that is later sscanf %d/%s
         */
        snprintf(netstat_cmd, sizeof(netstat_cmd),
                "netstat -lpnt4 2>/dev/null | awk '/:%d / { print $7 }' >%s", port, result_name);
        if (system(netstat_cmd) < 0) {
                return -1;
        }
        f = fopen(result_name, "r");
        if (f) {
                /*
                 * Read first line that should have result like this 2331/JLinkGDBServer
                 */
                if (2 > fscanf(f, "%d/%ms", &pid, &owning_proc) ||
                        strcmp(owning_proc, gdb_name) != 0) {
                        pid = -1; /* format did not match of process name was different */
                }
                free(owning_proc);
                fclose (f);
                unlink(result_name);
        }
        return pid;
}
#endif

/* stop all GDB Server instances - specified by name in path to new GDB Server */
static void stop_gdb_server_instances(const char *new_path, int port)
{
        char pkill_cmd[300];
        int ret = 0;
#ifdef WIN32
        strcpy(pkill_cmd, "%WINDIR%\\system32\\TASKKILL /F /IM >nul 2>nul ");
        get_name_from_path(new_path, &pkill_cmd[strlen(pkill_cmd)]);
        /* kill all processes based on GDB Server name */
        ret = system(pkill_cmd);
#else
        int pid = get_gdb_server_pid(new_path, port);
        if (pid > 0) {
                sprintf(pkill_cmd, "kill %d", pid);
                ret = system(pkill_cmd);
        }
#endif
        (void) ret;
}

/* check that there is at least one GDB Server instance running */
static bool is_running_gdb_server(const char *new_path, int port)
{
#ifdef WIN32
        HANDLE snap_shot;
        PROCESSENTRY32 proc_entry;
        bool found = false;
        char gdb_server_cmd[MAX_PATH];

        get_name_from_path(new_path, gdb_server_cmd);

        snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if ((snap_shot !=  INVALID_HANDLE_VALUE) && Process32First(snap_shot, &proc_entry)) {
                do {
                        found = _stricmp(gdb_server_cmd, proc_entry.szExeFile) == 0;
                } while (!found && Process32Next(snap_shot, &proc_entry));
        }

        if (snap_shot !=  INVALID_HANDLE_VALUE) {
                CloseHandle(snap_shot);
        }
        return found;
#else
        return get_gdb_server_pid(new_path, port) > 0;
#endif
}
/*
 * Before starting gdb server that is expected to open specific port, make sure
 * that this port can be used without problem.
 * When gdb server process is killed, socket that was used for communication with clients
 * can be in temporary state that prevents next instance of gdb server to use this
 * socket. This function tries to bind to gdb server socket for some time, if
 * it can be accomplished socket is closed and gdb server instance will not have
 * problems opening it.
 */
static int verify_local_socket_can_bind(uint16_t port, int timeout_ms)
{
        struct sockaddr_in addr;
        int err = 0;
        time_ms_t time_start_ms;
        time_ms_t time_check_ms;

        int sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                return ERR_GDB_SERVER_SOCKET;
        }
        addr.sin_family = PF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        time_start_ms = gdb_server_get_current_time_ms();

        /* Try to bind socket to specified port for some time */
        do {
                err = bind(sock, (struct sockaddr *) &addr, sizeof(addr));
                if (err) {
                        err = socket_error();
                }
                time_check_ms = gdb_server_get_current_time_ms();
        } while (err == EADDRINUSE && (time_check_ms - time_start_ms) <= timeout_ms);

        closesocket(sock);
        if (err) {
                err = ERR_GDB_SERVER_SOCKET;
        }

        return err;
}

#ifdef WIN32
/* Callback passed to EnumWindows which will close any window which is owned by given process */
BOOL CALLBACK terminate_app_cb(HWND hwnd, LPARAM lparam)
{
        DWORD pid;

        GetWindowThreadProcessId(hwnd, &pid);

        if (pid == (DWORD) lparam) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
        }

    return TRUE;
 }
#endif

/* run specified system command in different process */
static void run_command(const char *cmd)
{
#ifdef WIN32
        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi;
        SECURITY_ATTRIBUTES sa;
        char cmd_line[32768];
        char log_file[MAX_PATH];
        strcpy(cmd_line, cmd);

        /*
         * Standard output will be redirected to file, for this security attributes must have
         * inheritable flag.
         */
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        /*
         * Try to open gdb server log file in current folder, if not possible try temp folder
         */
        si.hStdOutput = CreateFile("cli_gdb_server.log", GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_ALWAYS, 0, 0);
        if (si.hStdOutput == INVALID_HANDLE_VALUE)
        {
                GetTempPath(sizeof(log_file) / sizeof(log_file[0]), log_file);
                PathAppend(log_file, "cli_gdb_server.log");
                si.hStdOutput = CreateFile(log_file, GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_ALWAYS, 0, 0);
        }
        si.hStdError = si.hStdOutput;
        si.hStdInput = (HANDLE) STD_INPUT_HANDLE; /* Both handles to one file */

        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        if (CreateProcessA(NULL, cmd_line, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
                CloseHandle(pi.hThread);
                gdb_server_pid = pi.dwProcessId;
                gdb_server_handle = pi.hProcess;
        }
        if (si.hStdOutput != INVALID_HANDLE_VALUE) {
                CloseHandle(si.hStdOutput);
        }
#else
        pid_t pid = fork();

        /* child process */
        if (pid == 0) {
                int fd;
                char *exec_cmd;

                /* Open log file and redirect (append) stdout and stderr there */
                fd = open("cli_gdb_server.log", O_APPEND | O_CREAT | O_WRONLY, 0666);
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
                close(STDIN_FILENO);

                /*
                 * Launching GDB server via execlp() in a safe manner would require to parse 'cmd'
                 * into separate arguments which is tricky. To make things easier we skip this and
                 * launch 'exec <cmd>' command in a shell. The result is more or less the same as
                 * using system() call but since shell process replaces current one and 'exec <cmd>'
                 * replaces shell process, 'pid' will be actual pid of process running GDB server
                 * and not pid of shell or its parent process.
                 */
                exec_cmd = alloca(5 + strlen(cmd) + 1);
                strcpy(exec_cmd, "exec ");
                strcat(exec_cmd, cmd);
                execlp("sh", "sh", "-c", exec_cmd, NULL);

                /* No more code will be executed here */
        } else if (pid == -1) {
                printf("Cannot fork GDB Server \n");
                return;
        }
        /* Save GDB Server process PID */
        gdb_server_pid = pid;
#endif
}

/* create socket and connect to GDB Server specified by host_name and port */
static int gdb_server_init_socket(uint16_t port, const char *host_name)
{
        struct sockaddr_in addr;
        time_ms_t time_start_ms;
        time_ms_t time_check_ms;
        const int nodelay = 1;

        if ((gdb_server_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
                return ERR_GDB_SERVER_SOCKET;
        }

        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = PF_INET;

        /* try resolve host_name as address */
        if ((addr.sin_addr.s_addr = inet_addr(host_name)) == INADDR_NONE) {
                /* get address from host_name*/
                struct hostent *host = gethostbyname(host_name);

                /* cannot find host_name */
                if (!host) {
                        closesocket(gdb_server_sock);
                        return ERR_GDB_SERVER_SOCKET;
                }

                addr.sin_addr.s_addr = *(uint32_t *) host->h_addr_list[0];
        }

        addr.sin_port = htons(port);

        time_start_ms = gdb_server_get_current_time_ms();

        /* Setting TCP_NODELAY option increases flashing speed on Linux */
        setsockopt(gdb_server_sock, IPPROTO_TCP, TCP_NODELAY, (void *) &nodelay, sizeof(nodelay));

        /* try connect to created socket */
        do {
                if (connect(gdb_server_sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in))
                                                                                        == 0) {
                        goto done;
                }

                time_check_ms = gdb_server_get_current_time_ms();
        } while ((time_check_ms - time_start_ms) <= GDB_RECONNECT_MAX_TIME_MS);

        closesocket(gdb_server_sock);
        return ERR_GDB_SERVER_SOCKET;

done:
        return 0;
}

/* check that checksum of data in gdb_server_recv_buf.buf is correct */
static bool gdb_sever_check_checksum(void)
{
        bool status = false;
        uint8_t from_frame = checksum((char *) gdb_server_recv_buf.buf + 1, gdb_server_recv_buf.len
                                                                                        - 4);
        uint8_t correct = chars_to_uint8(gdb_server_recv_buf.buf[gdb_server_recv_buf.len - 2],
                                gdb_server_recv_buf.buf[gdb_server_recv_buf.len - 1], &status);

        return (from_frame == correct);
}

/* check that received frame contains '$', '#' characters and checksum */
static bool gdb_server_check_frame(void)
{
        uint32_t i;
        /* $ character is start of the frame */
        bool dollar_found = false;
        /* # character is end of the data in frame */
        bool hash_found = false;
        uint32_t hash_pos = 0;

        for (i = 0; i < gdb_server_recv_buf.len; i++) {
                if (gdb_server_recv_buf.buf[i] == '$' ) {
                        dollar_found = true;
                } else if (gdb_server_recv_buf.buf[i] == '#') {
                        hash_pos = i;
                        hash_found = true;
                }

                /* response contains start frame and end data characters */
                if (dollar_found && hash_found) {
                        /* check that response contains checksum */
                        return ((hash_pos + 2 <= gdb_server_recv_buf.len));
                }
        }

        return false;
}

/* send data to GDB Server socket */
static int gdb_server_send(const char *buf, uint32_t buf_len)
{
        int i;
#if DBG_GDB_SERVER
        printf("<-- ");

        for (i = 0; i < (int) buf_len; i++) {
                printf("%c", buf[i]);
        }

        printf("\n");
#endif
        while (buf_len > 0) {
                i = send(gdb_server_sock, buf, buf_len, 0);

                if (i < 0) {
                        return ERR_GDB_SERVER_SOCKET;
                }

                buf_len -= i;
                buf += i;
        }

        return 0;
}

/* send acknowledgement to GDB Server socket */
static inline int gdb_server_ack(void)
{
        return gdb_server_send("+", 1);
}

/* send not acknowledgement to GDB Server socket */
static inline int gdb_server_nack(void)
{
        return gdb_server_send("-", 1);
}

/* receive data from GDB Server, check frame and send acknowledgement */
static int gdb_server_recv_with_ack(void)
{
        uint8_t repeats_cnt = 0;
        int status;
#if DBG_GDB_SERVER
        int i;
#endif
start:
        gdb_server_recv_buf.len = 0;

        /* receive until data is incomplete */
        do {
                gdb_server_recv_buf.len += recv(gdb_server_sock, (char *) gdb_server_recv_buf.buf +
                                gdb_server_recv_buf.len, MAX_BUF_LEN - gdb_server_recv_buf.len, 0);

#if DBG_GDB_SERVER
                printf("--> ");

                for (i = 0; i < (int) gdb_server_recv_buf.len; i++) {
                        printf("%c", gdb_server_recv_buf.buf[i]);
                }

                printf("\n");
#endif
        } while (!gdb_server_check_frame());

        if (!gdb_sever_check_checksum()) {
                /* checksum of received data is incorrect */
                if (repeats_cnt >= GDB_SERVER_REPEATS_LIMIT) {
                        /* exceeded repeats limit */
                        return ERR_GDB_SERVER_CRC_MISMATCH;
                }

                if ((status = gdb_server_nack()) != 0) {
                        return status;
                }

                ++repeats_cnt;
                goto start;
        }

        return gdb_server_ack();
}

/*
 * wait for acknowledgement (ack_nack set to true) or not acknowledgement (ack_nack set to false)
 * from GDB Server
 */
static inline int gdb_server_wait_for_ack(bool *ack_nack)
{
        uint8_t tmp_buf[1];
        long int tmp_buf_len;

        while (true) {
                if ((tmp_buf_len = recv(gdb_server_sock, (char *) tmp_buf, 1, 0)) < 0) {
                        return ERR_GDB_SERVER_SOCKET;
                }

                if (tmp_buf_len >= 1 && tmp_buf[0] == ACK_CHAR) {
                        *ack_nack = true;
                        return 0;
                } else if (tmp_buf_len >= 1 && tmp_buf[0] == NACK_CHAR) {
                        *ack_nack = false;
                        return 0;
                }
        }
}

/*
 * send data to GDB Server and wait for acknowledgement, if NACK received more than
 * GDB_SERVER_REPEATS_LIMIT return error else receive data to gdb_server_recv_buf.buf and send
 * acknowledgement
 */
static int gdb_server_send_recv_ack(const char *buf, uint32_t buf_len)
{
        uint8_t repeats_cnt = 0;
        int status;
        bool ack_nack;

        while (repeats_cnt < GDB_SERVER_REPEATS_LIMIT) {
                if ((status = gdb_server_send(buf, buf_len)) != 0) {
                       return status;
                }

                ++repeats_cnt;

                if ((status = gdb_server_wait_for_ack(&ack_nack)) != 0) {
                       return status;
                }

                if (ack_nack) {
                        return gdb_server_recv_with_ack();
                }
        }

        return ERR_GDB_SERVER_CMD_REJECTED;
}

/* send read command to GDB Server, command syntax typical for GDB */
static int gdb_server_send_read_cmd(uint32_t addr, uint32_t len)
{
        /*
         * length calculation: address (8 bytes), length (8 bytes), checksum (2 bytes), frame
         * characters (4 bytes) and NULL terminating (1 byte)
         */
        char cmd[8 + 8 + 2 + 4 + 1];
        uint32_t cmd_len;

        /* frame example: $m00000000,00000002#FF */
        sprintf(cmd, "$m%08X,%08X", addr, len);
        cmd_len = strlen(cmd);
        sprintf(&cmd[cmd_len], "#%02X", checksum(cmd + 1, cmd_len - 1));

        return gdb_server_send_recv_ack(cmd, strlen(cmd));
}

/* send write command to GDB Server, command syntax typical for GDB */
static int gdb_server_send_write_cmd(uint32_t addr, uint32_t data_len, const uint8_t *data)
{
        uint32_t tmp_len;
        uint32_t i;
        char *cmd;
        int ret;

        /*
         * length calculation: address (up to 8 bytes), data length (up to 8 bytes), checksum
         * (2 bytes), frame characters (5 bytes), data (up to data_len * 2 bytes) and NULL
         * terminating (1 byte)
         */
        cmd = malloc(data_len * 2 + 8 + 8 + 2 + 5 + 1);

        if (!cmd) {
                return ERR_ALLOC_FAILED;
        }

        /*
         * frame example: $m10,2:01234#FF, data are not a characters - binary data with some
         * characters escaping
         */
        sprintf(cmd, "$X%x,%x:", addr, data_len);
        tmp_len = strlen(cmd);

        for (i = 0; i < data_len; i++) {
                /* escape special characters */
                if (data[i] == '#' || data[i] == '$' || data[i] == '}') {
                        cmd[tmp_len++] = '}';
                        cmd[tmp_len++] = data[i] - 0x20;
                } else {
                        cmd[tmp_len++] = data[i];
                }
        }

        sprintf(&cmd[tmp_len], "#%02X", checksum(cmd + 1, tmp_len - 1));

        ret = gdb_server_send_recv_ack(cmd, tmp_len + 3);
        free(cmd);

        return ret;
}

/* create loadbin request with specified file path, after using returned pointer must be freed */
static char *gdb_server_create_loadbin_req(const char *file_path, uint32_t addr)
{
        char *req;

        /*
         * length calculation: file path (file path length bytes), loadbin command (7 bytes),
         * address (8 bytes), checksum (2 bytes), request frame characters (5 bytes) and NULL
         * terminating (1 byte)
         */
        req = malloc(strlen(file_path) + 7 + 8 + 5 + 2 + 1);

        if (!req) {
                return NULL;
        }

        /* request example: loadbin /tmp/AbCdfile.bin, 0x00000000 */
        sprintf(req, "%s %s, 0x%08X", "loadbin", file_path, addr);

        return req;
}

/*
 * send local command to GDB Server, command syntax typical for GDB, commands may be different
 * depending of the server application
 */
static int gdb_server_send_local_cmd(const char *req)
{
        uint32_t len;
        uint32_t i;
        char *cmd;
        int ret;

        /*
         * length calculation: execute local command (5 bytes), request (2 * request length bytes),
         * checksum (2 bytes), frame characters (3 bytes), and NULL terminating (1 byte)
         */
        cmd = malloc(strlen(req) * 2 + 5 + 2 + 3 + 1);

        if (!cmd) {
                return ERR_ALLOC_FAILED;
        }

        /*
         * frame example: $qRcmd,012345678#FF where 0123.. is encoded request - 2 characters for
         * each byte of request
         */
        sprintf(cmd,"$%s,", GDB_LOCAL_CMD_EXE);
        len = strlen(cmd);

        for (i = 0; i < strlen(req); i++) {
                sprintf(&cmd[len], "%02X", req[i]);
                len += 2;
        }

        strcat(cmd + len, "#");
        ++len;
        sprintf(cmd + len, "%02X", checksum(cmd + 1, len - 2));

        ret = gdb_server_send_recv_ack(cmd, strlen(cmd));
        free(cmd);

        return ret;
}

/* send loadbin command to GDB Server */
static int gdb_server_send_loadbin_cmd(const char *path, uint32_t addr)
{
        char *req;
        int ret;

        req = gdb_server_create_loadbin_req(path, addr);

        if (!req) {
                return ERR_ALLOC_FAILED;
        }

        ret = gdb_server_send_local_cmd(req);
        free(req);

        return ret;
}

/* set swd_run field on true - this allows swd_loop execution in uartboot */
static int gdb_server_set_run_swd(void)
{
        uint8_t arr[] = {true};

        return gdb_server_send_write_cmd(swd_addr.run_swd_addr, 1, arr);
}

/* increase command number field - this allows new command execution by uartboot */
static int gdb_server_inc_cmd_num(void)
{
        ++cmd_num_cp;

        return gdb_server_send_write_cmd(swd_addr.cmd_num_addr, 4, (uint8_t *) &cmd_num_cp);
}

/*
 * send step and continue commands to GDB Server - after continue command only program cannot skip
 * breakpoint
 */
static int gdb_server_send_continue(void)
{
        int status;

        if ((status = gdb_server_send_recv_ack(GDB_STEP, strlen(GDB_STEP))) != 0) {
                return status;
        }

        return gdb_server_send_recv_ack(GDB_CONTINUE, strlen(GDB_CONTINUE));
}

/*
 * create array of uint8_t from characters buffer, e.g: ABCDEF -> {0xAB, 0xCD, 0xEF}, if conversion
 * cannot be done return false otherwise return true
 */
static bool gdb_server_frame_to_uint8_buf(uint8_t *buf, uint32_t len)
{
        uint32_t i;
        uint32_t start_data = 0;
        bool conversion_status = false;

        /* find start of the data - '$' character */
        for (i = 0; i < gdb_server_recv_buf.len; i++) {
                if (gdb_server_recv_buf.buf[i] == '$') {
                        start_data = i + 1;
                        break;
                }
        }

        /* convert characters to uint8_t values, 2 characters for each value */
        for (i = 0; i < len; i++) {
                buf[i] = chars_to_uint8(gdb_server_recv_buf.buf[start_data + i * 2],
                        gdb_server_recv_buf.buf[start_data + i * 2 + 1], &conversion_status);

                if (!conversion_status) {
                      /* cannot convert characters to uint8_t */
                     return false;
                }
        }

        /* conversion done without problems */
        return true;
}

/*
 * perform all necessary action to properly connect to GDB Server and start the platform
 * loading uartboot code must be in other function - when initialization function is called
 * the local copy of uartboot code is not created
 */
static int gdb_server_init(const prog_gdb_server_config_t *gdb_server_conf)
{
        uint16_t tmp_16;
        uint32_t tmp_32;
        int status;
        bool start_gdb = false;
        bool local_rst = false;

        /* try to stop previous instances of GDB Server */
        if (gdb_server_conf->stop_prv_gdb_server && gdb_server_conf->gdb_server_path &&
                                                        is_localhost(gdb_server_conf->host_name)) {
                stop_gdb_server_instances(gdb_server_conf->gdb_server_path, gdb_server_conf->port);

                start_gdb = true;
        } else if (is_localhost(gdb_server_conf->host_name) && gdb_server_conf->gdb_server_path &&
                        !is_running_gdb_server(gdb_server_conf->gdb_server_path, gdb_server_conf->port)) {
                start_gdb = true;
        }

        if (start_gdb) {
                /*
                 * Before starting gdb server make sure that it can open specific port.
                 * If port can't be opened there is not point in starting gdb server.
                 */
                status = verify_local_socket_can_bind(gdb_server_conf->port, 5000);
                if (status) {
                        return status;
                }
                if (target_reset_cmd && is_localhost(gdb_server_conf->host_name)) {
                        status = system(target_reset_cmd);
                        if (status < 0) {
                                return status;
                        }
                        local_rst = true;
                }
                run_command(gdb_server_conf->gdb_server_path);
        }

        /* open socket to GDB Server */
        if ((status = gdb_server_init_socket(gdb_server_conf->port, gdb_server_conf->host_name))
                                                                                        != 0) {
                printf("Failed to bind to socket \n");
                return status;
        }

        if (!local_rst) {
                /* set speed to 4000 kHz */
                if ((status = gdb_server_send_local_cmd("speed 4000")) != 0) {
                        return status;
                }

                /* remove all breakpoints */
                if ((status = gdb_server_send_local_cmd("clrbp")) != 0) {
                        return status;
                }

                /* hardware platform reset */
                if ((status = gdb_server_send_local_cmd("reset 0")) != 0) {
                        return status;
                }

                /* Wait for reset */
                if ((status = gdb_server_send_local_cmd("sleep 10")) != 0) {
                        return status;
                }

                /* stop program execution on platform */
                if ((status = gdb_server_send_local_cmd("halt")) != 0) {
                        return status;
                }

                /* write MAGIC VALUE */
                status  = gdb_server_send_local_cmd("memU32 0x7fd0000 = 0xdeadbeef");
                status |= gdb_server_send_local_cmd("memU32 0x7fd0004 = 0xdeadbeef");
                status |= gdb_server_send_local_cmd("memU32 0x7fd0008 = 0xdeadbeef");
                status |= gdb_server_send_local_cmd("memU32 0x7fd000c = 0xdead10cc");
                if (status  != 0) {
                        return status;
                }

                /* set SWD_HW_RESET_REQ bit in SWD_RESET_REG */
                tmp_32 = 0x00000001;
                /* verification of this writing could fail - not readable register */
                if ((status = gdb_server_send_write_cmd(0x400C3050, 4, (uint8_t *)&tmp_32)) != 0) {
                        return status;
                }

                /* wait after platform reset */
                if ((status = gdb_server_send_local_cmd("sleep 100")) != 0) {
                        return status;
                }
                sleep(1);

                if ((status = gdb_server_send_local_cmd("reset 0")) != 0) {
                        return status;
                }

                /* Wait for reset */
                if ((status = gdb_server_send_local_cmd("sleep 1")) != 0) {
                        return status;
                }

                /* stop program execution on platform */
                if ((status = gdb_server_send_local_cmd("halt")) != 0) {
                        return status;
                }
        }
        /* set SYS_CTRL_REG register */
        tmp_16 = 0x00AB;

        return gdb_server_send_write_cmd(0x50000012, 2, (uint8_t *) &tmp_16);
}

static int gdb_server_load_image(void)
{
        char tmp_name[500];
        int status;

        /*
         * these operations can not be performed in initialization function - without uartboot
         * code
         */
        if ((status = get_tmp_file_name(tmp_name)) != 0) {
                return status;
        }

        if ((status = memory_to_file(tmp_name, boot_loader_code, boot_loader_size)) != 0) {
                return status;
        }

        /* write boot loader code from file to platform */
        if ((status = gdb_server_send_loadbin_cmd(tmp_name, 0)) != 0) {
                return status;
        }

        /* hardware platform reset */
        if ((status = gdb_server_send_local_cmd("reset 0")) != 0) {
                return status;
        }

        /* hardware platform reset */
        if ((status = gdb_server_send_local_cmd("go")) != 0) {
                return status;
        }

        /* remove created temporary file */
        remove(tmp_name);

        uartboot_loaded = true;

        return 0;
}

static int gdb_server_load_uartboot(void)
{
        char tmp_name[500];
        int status;

        /*
         * these operations can not be performed in initialization function - without uartboot
         * code
         */
        if ((status = get_tmp_file_name(tmp_name)) != 0) {
                return status;
        }

        if ((status = memory_to_file(tmp_name, boot_loader_code, boot_loader_size)) != 0) {
                return status;
        }

        /* write boot loader code from file to platform */
        if ((status = gdb_server_send_loadbin_cmd(tmp_name, 0)) != 0) {
                return status;
        }

        /* hardware platform reset */
        if ((status = gdb_server_send_local_cmd("reset 0")) != 0) {
                return status;
        }

        /* stop program execution on platform */
        if ((status = gdb_server_send_local_cmd("halt")) != 0) {
                return status;
        }

        /* set run_swd field in boot loader on the platform */
        if ((status = gdb_server_set_run_swd()) != 0) {
                return status;
        }

        /* continue program execution  */
        if ((status = gdb_server_send_continue()) != 0) {
                return status;
        }

        /* remove created temporary file */
        remove(tmp_name);

        uartboot_loaded = true;

        return 0;
}

/* check is the gdb server socket was opened before, if not open it and initialize GDB Server */
static int gdb_server_verify_connection(void)
{
        int status;

        /* initial procedure was done earlier */
        if (gdb_server_sock > -1) {
                if (!uartboot_loaded) {
                        return gdb_server_load_uartboot();
                }

              return 0;
        }

        /*
         * standard initialization debugger procedure:
         * - initialize GDB Server socket
         * - reset and halt device few time
         * - write registers values
         * - start swd loop in uartboot
         */
        if ((status = gdb_server_init(&configuration)) != 0) {
                return status;
        }

        return gdb_server_load_uartboot();
}

/*
 *  write command header to command buffer in uartboot, increase command number and continue
 *  execution swd loop in uartboot
 */
static int gdb_server_send_swd_cmd_header(uint32_t data_len, const uint8_t *data)
{
        int status;

        if ((status = gdb_server_send_write_cmd(swd_addr.cmd_buf_addr, data_len, data)) != 0) {
                return status;
        }

        if ((status = gdb_server_inc_cmd_num()) != 0) {
                return status;
        }

        return gdb_server_send_continue();
}

/*
 * read gdb frames, first the buffer len is detected and then the translated data
 * are copied into buf
 */
static int gdb_server_read_dynamic_length(uint8_t **buf, uint32_t *len)
{
        int ret = 0;
        uint16_t size;

        /* receive payload len */
        if ((ret = gdb_server_send_read_cmd(swd_addr.buf_addr, sizeof(uint16_t))) != 0) {
                goto done;
        }

        if (!gdb_server_frame_to_uint8_buf((uint8_t *) &size, sizeof(uint16_t))) {
                ret = ERR_FAILED;
                goto done;
        }

        *len = size;

        if ((*buf = malloc(*len)) == NULL) {
                ret = ERR_ALLOC_FAILED;
                goto done;
        }

        /* receive payload */
        if ((ret = gdb_server_send_read_cmd(swd_addr.buf_addr, size)) != 0) {
                goto done;
        }

        if (!gdb_server_frame_to_uint8_buf(*buf, size)) {
                ret = ERR_FAILED;
                goto done;
        }

done:
        return ret;
}

int gdb_server_initialization(const prog_gdb_server_config_t *gdb_server_conf)
{
#ifdef WIN32
        WSADATA wsaData;

        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        /* close socket if opened */
        if (gdb_server_sock != -1) {
                closesocket(gdb_server_sock);
                gdb_server_sock = -1;
        }

        /* make local copy of configuration */
        memset(&configuration, 0 , sizeof(configuration));
        configuration.port = gdb_server_conf->port;
        configuration.stop_prv_gdb_server = gdb_server_conf->stop_prv_gdb_server;

        /* GDB Server path could be empty */
        if (gdb_server_conf->gdb_server_path) {
                configuration.gdb_server_path = strdup(gdb_server_conf->gdb_server_path);
        } else {
                configuration.gdb_server_path = NULL;
        }

        configuration.host_name = strdup(gdb_server_conf->host_name);

        return gdb_server_init(&configuration);
}

void gdb_server_set_boot_loader_code(uint8_t *code, size_t size)
{
        boot_loader_code = code;
        boot_loader_size = size;

        /* set initial values */
        set_swd_addresses();
        cmd_num_cp = 0;
}

void gdb_server_get_boot_loader_code(uint8_t **code, size_t *size)
{
        *code = boot_loader_code;
        *size = boot_loader_size;
}

/* write to RAM could be direct - without using uartboot command */
int gdb_server_cmd_write(const uint8_t *buf, size_t size, uint32_t addr)
{
        int status = 0;
        size_t part_size;

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /*
         * if address is equal ADDRESS_TMP then buffer should be write to temporary buffer on the
         * platform - to address specified by swd_addr.buf_addr
         */
        addr = ((addr == ADDRESS_TMP) ? swd_addr.buf_addr : addr);

        for (part_size = 0; part_size < size;) {
                size_t write_size = ((CHUNK_SIZE > (size - part_size)) ? (size - part_size) :
                                                                                        CHUNK_SIZE);

                status = gdb_server_send_write_cmd(addr + part_size, write_size, buf + part_size);

                if (status) {
                        return status;
                }

                part_size += write_size;
        }

        return status;
}

/* read from RAM could be direct - without using uartboot command */
int gdb_server_cmd_read(uint8_t *buf, size_t size, uint32_t addr)
{
        int status;
        size_t chunk;
        size_t offset = 0;

        if ((status = gdb_server_verify_connection()) != 0) {
                return status;
        }

        /*
         * buffer for reading response from gdb server is limited to 64K.
         * If user requested more data split it to several gdb server requests.
         */
        while (size > 0 && status == 0) {
                chunk = MAX_READ_SIZE < size ? MAX_READ_SIZE : size;
                if ((status = gdb_server_send_read_cmd(addr + offset, chunk)) != 0) {
                        return status;
                }
                if (!(gdb_server_frame_to_uint8_buf(buf + offset, chunk))) {
                        status = ERR_GDB_SERVER_INVALID_RESPONSE;
                } else {
                        size -= chunk;
                        offset += chunk;
                }
        }
        return status;
}

/*
 * Some commands will return NAK, this can happen if arguments were wrong or execution is not
 * possible. For example writing to already written OTP address will result in NAK.
 */
static int gdb_server_read_ack(void)
{
        int status;
        uint8_t ack_nak;

        /* read ACK or NAK left by command execution in swd area */
        if ((status = gdb_server_send_read_cmd(swd_addr.ack_nak_addr, 1)) != 0) {
                return status;
        }

        /* Decode ACK or NAK from GDB one byte read */
        if (!gdb_server_frame_to_uint8_buf(&ack_nak, 1)) {
                return ERR_GDB_SERVER_INVALID_RESPONSE;
        }

        return (ack_nak == NAK) ? ERR_PROT_COMMAND_ERROR : 0;
}

int gdb_server_cmd_copy_to_qspi(uint32_t src_address, size_t size, uint32_t dst_address)
{
        uint8_t header_buf[14];

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_COPY_QSPI;
        header_buf[2] = (uint8_t) 10;
        header_buf[3] = (uint8_t) (10 >> 8);

        /*
         * if src_address is equal ADDRESS_TMP then buffer should be write to temporary buffer on
         * the platform - to address specified by swd_addr.buf_addr
         */
        src_address = ((src_address == ADDRESS_TMP) ? swd_addr.buf_addr : src_address);

        header_buf[4] = (uint8_t) (src_address);
        header_buf[5] = (uint8_t) (src_address >> 8);
        header_buf[6] = (uint8_t) (src_address >> 16);
        header_buf[7] = (uint8_t) (src_address >> 24);

        header_buf[8] = (uint8_t) size;
        header_buf[9] = (uint8_t) (size >> 8);

        header_buf[10] = (uint8_t) (dst_address);
        header_buf[11] = (uint8_t) (dst_address >> 8);
        header_buf[12] = (uint8_t) (dst_address >> 16);
        header_buf[13] = (uint8_t) (dst_address >> 24);

        /* send header */
        return gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf);
}

int gdb_server_cmd_erase_qspi(uint32_t address, size_t size)
{
        uint8_t header_buf[12];

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_ERASE_QSPI;
        header_buf[2] = (uint8_t) 8;
        header_buf[3] = (uint8_t) (8 >> 8);

        header_buf[4] = (uint8_t) (address);
        header_buf[5] = (uint8_t) (address >> 8);
        header_buf[6] = (uint8_t) (address >> 16);
        header_buf[7] = (uint8_t) (address >> 24);
        header_buf[8] = (uint8_t) (size);
        header_buf[9] = (uint8_t) (size >> 8);
        header_buf[10] = (uint8_t) (size >> 16);
        header_buf[11] = (uint8_t) (size >> 24);

        /* send header */
        return gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf);
}

int gdb_server_cmd_chip_erase_qspi(void)
{
        uint8_t header_buf[2];

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_CHIP_ERASE_QSPI;

        /* send header */
        return gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf);
}

int gdb_server_cmd_run(uint32_t address)
{
        uint8_t header_buf[8];

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_RUN;
        header_buf[2] = (uint8_t) 4;
        header_buf[3] = (uint8_t) (4 >> 8);

        header_buf[4] = (uint8_t) (address);
        header_buf[5] = (uint8_t) (address >> 8);
        header_buf[6] = (uint8_t) (address >> 16);
        header_buf[7] = (uint8_t) (address >> 24);

        /* send header */
        return gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf);
}

int gdb_server_cmd_write_otp(uint32_t address, const uint32_t *buf, uint32_t len)
{
        uint8_t header_buf[8];
        uint32_t size = len * sizeof(*buf);
        uint32_t part_size;
        int status = 0;

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create constant part of header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_WRITE_OTP;

        for (part_size = 0; part_size < size;) {
                size_t write_size = ((CHUNK_SIZE > (size - part_size)) ? (size - part_size) :
                                                                                        CHUNK_SIZE);
                header_buf[2] = (uint8_t) (4 + write_size);
                header_buf[3] = (uint8_t) ((4 + write_size) >> 8);

                header_buf[4] = (uint8_t) (address);
                header_buf[5] = (uint8_t) (address >> 8);
                header_buf[6] = (uint8_t) (address >> 16);
                header_buf[7] = (uint8_t) (address >> 24);

                /* send data */
                gdb_server_send_write_cmd(swd_addr.buf_addr, write_size, (const uint8_t *) buf +
                                                                                        part_size);

                /* send header */
                if ((status = gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf))
                                                                                        != 0) {
                        return status;
                }

                /* wait for ACK or NACK */
                if ((status = gdb_server_read_ack()) != 0) {
                        return status;
                }

                part_size += write_size;
                /* It is a cell address - should be divided by 8 bytes */
                address += (write_size >> 3);
        }

        return status;
}
int gdb_server_cmd_read_otp(uint32_t address, uint32_t *buf, uint32_t len)
{
        uint8_t header_buf[10];
        uint32_t size = len * sizeof(*buf);
        int status;

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_READ_OTP;
        header_buf[2] = (uint8_t) 6;
        header_buf[3] = (uint8_t) (6 >> 8);

        header_buf[4] = (uint8_t) (address);
        header_buf[5] = (uint8_t) (address >> 8);
        header_buf[6] = (uint8_t) (address >> 16);
        header_buf[7] = (uint8_t) (address >> 24);

        header_buf[8] = (uint8_t) len;
        header_buf[9] = (uint8_t) (len >> 8);

        /* send header */
        if ((status = gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf)) != 0) {
                return status;
        }

        /* Check is command returned NAK */
        if ((status = gdb_server_read_ack()) != 0) {
                return status;
        }

        /* receive data */
        if ((status = gdb_server_send_read_cmd(swd_addr.buf_addr, size)) != 0) {
                return status;
        }

        return (gdb_server_frame_to_uint8_buf((uint8_t *) buf, size) ? 0 :
                                                                ERR_GDB_SERVER_INVALID_RESPONSE);
}

int gdb_server_cmd_read_qspi(uint32_t address, uint8_t *buf, uint32_t len)
{
        uint8_t header_buf[10];
        uint32_t size = len * sizeof(*buf);
        int status = 0;

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_READ_QSPI;
        header_buf[2] = (uint8_t) 6;
        header_buf[3] = (uint8_t) (6 >> 8);

        header_buf[4] = (uint8_t) (address);
        header_buf[5] = (uint8_t) (address >> 8);
        header_buf[6] = (uint8_t) (address >> 16);
        header_buf[7] = (uint8_t) (address >> 24);

        header_buf[8] = (uint8_t) len;
        header_buf[9] = (uint8_t) (len >> 8);

        /* send header */
        if ((status = gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf)) != 0) {
                return status;
        }

        /* receive data */
        if ((status = gdb_server_send_read_cmd(swd_addr.buf_addr, size)) != 0) {
                return status;
        }

        return ((!status && gdb_server_frame_to_uint8_buf((uint8_t *) buf, len)) ? 0 :
                                                                ERR_GDB_SERVER_INVALID_RESPONSE);
}

int gdb_server_cmd_is_empty_qspi(unsigned int size, unsigned int start_address, int *ret_number)
{
        uint8_t header_buf[12];
        int status = 0;

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_IS_EMPTY_QSPI;
        header_buf[2] = (uint8_t) 8;
        header_buf[3] = (uint8_t) (8 >> 8);
        header_buf[4] = (uint8_t) (size);
        header_buf[5] = (uint8_t) (size >> 8);
        header_buf[6] = (uint8_t) (size >> 16);
        header_buf[7] = (uint8_t) (size >> 24);

        header_buf[8] = (uint8_t) (start_address);
        header_buf[9] = (uint8_t) (start_address >> 8);
        header_buf[10] = (uint8_t) (start_address >> 16);
        header_buf[11] = (uint8_t) (start_address >> 24);

        /* send header */
        if ((status = gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf)) != 0) {
                return status;
        }

        /* receive data */
        if ((status = gdb_server_send_read_cmd(swd_addr.buf_addr, sizeof(int))) != 0) {
                return status;
        }

        return (gdb_server_frame_to_uint8_buf((uint8_t *) ret_number, sizeof(uint32_t)) ? 0 :
                                                                ERR_GDB_SERVER_INVALID_RESPONSE);
}

int gdb_server_cmd_read_partition_table(uint8_t **buf, uint32_t *len)
{
        uint8_t header_buf[2];
        int status = 0;

        if (gdb_server_verify_connection()) {
                return ERR_GDB_SERVER_SOCKET;
        }

        /* create header */
        header_buf[0] = SOH;
        header_buf[1] = CMD_READ_PARTITION_TABLE;

        /* send header */
        if ((status = gdb_server_send_swd_cmd_header(sizeof(header_buf), header_buf)) != 0) {
                return status;
        }

        return ((!status && gdb_server_read_dynamic_length(buf, len)) ?
                                                                ERR_GDB_SERVER_INVALID_RESPONSE :
                                                                0);
}

int gdb_server_cmd_boot(void)
{
        return gdb_server_load_image();
}

void gdb_server_close(void)
{
#ifndef WIN32
        pid_t tmp_pid;
        int status = 0;
        time_ms_t time_start_ms;
        time_ms_t time_check_ms;

        if (gdb_server_sock != -1) {
                closesocket(gdb_server_sock);
        }

        if (gdb_server_pid < 0) {
                return;
        }

        /* Terminate GDB server gracefully */
        kill(gdb_server_pid, SIGTERM);

        time_start_ms = gdb_server_get_current_time_ms();

        /* wait until GDB Server is running or timeout is not reached */
        do {
                /* GDB Server exited */
                if ((tmp_pid = waitpid(gdb_server_pid, &status, WNOHANG)) != 0) {
                        goto done;
                }

                time_check_ms = gdb_server_get_current_time_ms();
        } while ((time_check_ms - time_start_ms) <= GDB_SERVER_CLOSE_MAX_TIME_MS);

        /* If GDB server process is still running, just force kill it with SIGKILL */
        kill(gdb_server_pid, SIGKILL);
        waitpid(gdb_server_pid, &status, 0);

done:

#else
        if (gdb_server_sock != -1) {
                closesocket(gdb_server_sock);
        }

        /* Try to stop JLinkGDBServer gracefully */
        if (gdb_server_pid) {
                EnumWindows(terminate_app_cb, (LPARAM) gdb_server_pid);
        }

        /* GDB server was started by CLI wait for graceful end first, then kill */
        if (gdb_server_handle != INVALID_HANDLE_VALUE) {
                if (WaitForSingleObject(gdb_server_handle, GDB_SERVER_CLOSE_MAX_TIME_MS)
                                                                                != WAIT_OBJECT_0) {
                        char kill_cmd[MAX_PATH];

                        sprintf(kill_cmd, "%%WINDIR%%\\system32\\TASKKILL /F /PID %d >nul 2>nul",
                                                                        (int) gdb_server_pid);
                        system(kill_cmd);
                }
                CloseHandle(gdb_server_handle);
        }

        WSACleanup();
#endif

        /* Cleanup local configuration structure */
        if (configuration.host_name) {
                free(configuration.host_name);
        }

        if (configuration.gdb_server_path) {
                free(configuration.gdb_server_path);
        }
        if (target_reset_cmd)
                free(target_reset_cmd);
}

void gdb_invalidate_stub(void)
{
	uartboot_loaded = false;
}
