/**
****************************************************************************************
*
* @file opt_handlers.c
*
* @brief Handling of CLI options provided on command line
*
* Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
* program includes Confidential, Proprietary Information and is a Trade Secret of
* Dialog Semiconductor Ltd. All use, disclosure, and/or reproduction is prohibited
* unless authorized in writing. All Rights Reserved.
*
* <black.orca.support@diasemi.com> and contributors.
*
****************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cli_common.h"
#include "cli_config_parser.h"
#include <programmer.h>
#ifdef WIN32
#define strdup _strdup
#endif

struct cli_options main_opts = {
        /* .initial_baudrate = */ 0,
        /* .uartboot_config = */ {
                /* .baudrate = */ 57600,
                /* .baudrate_patch = */ 0,
                /* .tx_port = */ 1,
                /* .tx_port_patch = */ 0,
                /* .tx_pin = */ 3,
                /* .tx_pin_patch = */ 0,
                /* .rx_port = */ 2,
                /* .rx_port_patch = */ 0,
                /* .rx_pin = */ 3,
                /* .rx_pin_patch = */ 0,
        },
        /* .timeout = */ 5000,
        /* This field should point on dynamic allocated memory or NULL */
        /* .bootloader_fname = */ NULL,
        /* .gdb_server_config = */ {
                /* .port = */ 2331,
                /* This field should point on dynamic allocated memory or NULL */
                /* .host_name = */ NULL,
                /* This field should point on dynamic allocated memory or NULL */
                /* .gdb_server_path = */ NULL,
                /* .stop_prv_gdb_server = */ true
        },
        /*.config_file_path  = */ NULL,
        /* .chip_rev = */ NULL,
        /* .enable_uart = */ true,
        /* .ram_shuffling = */ 0,
        /* .target_reset_cmd  = */ NULL,
};

void set_str_opt(char **opt, const char *val)
{
        if (*opt) {
                free(*opt);
                *opt = NULL;
        }
        if (val) {
                *opt = strdup(val);
        }
}

static void opth_help(void)
{
        printf("usage: cli_programmer [-h] [--cfg <config_file>]\n"
                "                      [-s <baudrate>] [-i <baudrate>] \n"
                "                      [-p <port_num>] [-r <host>]\n"
                "                      [--tx-port <port_num>] [--tx-pin <pin_num>] \n"
                "                      [--rx-port <port_num>] [--rx-pin <pin_num] [-w timeout] \n"
                "                      [--no-kill] [--gdb-cmd <cmd>] \n"
                "                      [--trc <cmd>] \n"
                "                      [--save-ini] \n"
                "                      [--save <config_file>]\n"
                "                      [--prod-id <id>] [--enable-uart y|n] [--ram-shuffling <n>]\n"
                "                      [-b file] \n");
        printf("                       <interface> <command> [<args>]\n");
        printf("\n");
        printf("interface can be gdbserver or serial port name\n");
        printf("\n");
        printf("options:\n");
        printf("    -h                     Print this message\n");
        printf("    --cfg <config-file>    Use options from user specified file.\n"
                "                           Default file can be created with --save option.\n"
                "                           Options from this file override any options that\n"
                "                           were specified on command line before --cfg option\n"
                "                           it is best to put this option as the first option.\n");
        printf("    -i <baudrate>          Initial baud rate used for uploading uartboot or a \n"
                "                           user supplied binary. This depends on the rate used \n"
                "                           by the boot loader of the device. The default \n"
                "                           behavior is to use the value passed by '-s' \n"
                "                           (see below) or its default, if the parameter is not \n"
                "                           given. This option is ignored by the \"boot\" \n"
                "                           command. '-s' option should be used in this case. \n");
        printf("    -s <baudrate>          Baud rate used for UART by uartboot. This parameter \n"
                "                           is patched to the uploaded uartboot binary (in that \n"
                "                           way passed as a parameter). The default value is %d.\n",
                main_opts.uartboot_config.baudrate);
        printf("    --tx-port <port_num>   GPIO port used for UART Tx by uartboot. This \n"
                "                           parameter is patched to the uploaded uartboot binary \n"
                "                           (in that way passed as a parameter). Default value \n"
                "                           is %d. This argument is ignored when the 'boot' \n"
                "                           command is given (see below).\n",
                main_opts.uartboot_config.tx_port);
        printf("    --tx-pin <pin_num>     GPIO pin used for UART Tx by uartboot. This \n"
                "                           parameter is patched to the uploaded uartboot binary \n"
                "                           (in that way passed as a parameter). Default value \n"
                "                           is %d. This argument is ignored when the 'boot' \n"
                "                           command is given (see below).\n",
                main_opts.uartboot_config.tx_pin);
        printf("    --rx-port <port_num>   GPIO port used for UART Rx by uartboot. This \n"
                "                           parameter is patched to the uploaded uartboot binary \n"
                "                           (in that way passed as a parameter). Default value \n"
                "                           is %d. This argument is ignored when the 'boot' \n"
                "                           command is given (see below).\n",
                main_opts.uartboot_config.rx_port);
        printf("    --rx-pin <pin_num>     GPIO pin used for UART Rx by uartboot. This \n"
                "                           parameter is patched to the uploaded uartboot binary \n"
                "                           (in that way passed as a parameter). Default value \n"
                "                           is %d. This argument is ignored when the 'boot' \n"
                "                           command is given (see below).\n",
                main_opts.uartboot_config.rx_pin);
        printf("    --prod-id <id>         Chip product id:\n"
                "                           DA14680-00, DA14681-00, DA14680-01, DA14681-01\n");
        printf("    --enable-uart y|n      For chip AD enable booting from UART.\n");
        printf("    --ram-shuffling <n>    configure the DataRAM cell shuffling:\n"
                "                           0:  8 - 24 - 32 (default)\n"
                "                           1: 24 -  8 - 32\n"
                "                           2: 32 -  8 - 24\n"
                "                           3: 32 - 24 -  8\n");
        printf("    -w <timeout>           Serial port communication timeout.\n");
        printf("    -r <host>              Gdb server host (default: localhost).\n");
        printf("    -p <port>              Gdb server port (default: 2331).\n");
        printf("    --gdb-cmd <cmd>        Gdb server start command. Must be used if there is \n"
                "                           a need to start the gdb server instance on host. \n"
                "                           Without this parameter any gdb server instance won't \n"
                "                           be started or stopped.\n");
        printf("    --no-kill              Don't stop previous gdb server instances.\n");
        printf("    --trc <cmd>            Target reset command. Must be used if there is \n"
                "                           a need to replace the default localhost reset command. \n");
        printf("    -b <file>              Filename of 2nd stage bootloader or application \n"
                "                           binary.\n");
        printf("                           If this parameter is not specified built-in version of \n"
                "                           uartboot.bin is used.\n");
        printf("    --save-ini             Save CLI programmer configuration to cli_programmer.ini \n"
                "                           file.\n");
        printf("    --save <config_file>   Save CLI programmer configuration to user specified file.\n");
        printf("\n");
        printf("commands:\n");
        printf("    write <address> <file> [<size>]\n");
        printf("        writes up to <size> bytes of <file> into RAM memory at <address>\n");
        printf("        if <size> is omitted, complete file is written\n");
        printf("    read <address> <file> <size>\n");
        printf("        reads <size> bytes from RAM memory starting at <address> into <file>\n");
        printf("        if <file> is specified as either '-' or '--', data is output to stdout \n"
                "        as hexdump\n");
        printf("        hexdump is either 16-bytes (-) or 32-bytes (--) wide\n");
        printf("    write_qspi <address> <file> [<size>]\n");
        printf("        writes up to <size> bytes of <file> into QSPI flash at <address>\n");
        printf("        if <size> is omitted, complete file is written\n");
        printf("    write_qspi_bytes <address> <data1> [<data2> [...]]\n");
        printf("        writes bytes specified on command line into QSPI flash at <address>\n");
        printf("    write_qspi_exec <image_file>\n");
        printf("        writes image to QSPI flash.\n");
        printf("    write_suota_image <image_file> <version>\n");
        printf("        writes SUOTA enabled binary file to executable partition.\n");
        printf("        <version> is user supplied version string that goes to image header.\n");
        printf("    read_qspi <address> <file> <size>\n");
        printf("        reads <size> bytes from QSPI memory starting at <address> into <file>\n");
        printf("        if <file> is specified as either '-' or '--', data is output to stdout \n"
                "        as hexdump\n");
        printf("        hexdump is either 16-bytes (-) or 32-bytes (--) wide\n");
        printf("    erase_qspi <address> <size>\n");
        printf("        erases <size> bytes of QSPI flash starting at <address>\n");
        printf("        note: actual area erased may be different due to size of erase block\n");
        printf("    chip_erase_qspi\n");
        printf("        erases whole QSPI flash\n");
        printf("    copy_qspi <address_ram> <address_qspi> <size>\n");
        printf("        copies <size> bytes from RAM memory starting at <address_ram> to QSPI \n"
                "       flash\n");
        printf("        at <address_flash>\n");
        printf("    is_empty_qspi [start_address size]\n");
        printf("        checks that <size> bytes of QSPI flash contain only 0xFF values, starting\n"
                "       from <start_address>. Default value of <size> is 1 MB and default value of\n"
                "       <start_address> is 0.\n");
        printf("    read_partition_table\n");
        printf("        reads the partition table (if any) and prints its contents\n");
        printf("    write_otp <address> <length> [<data> [<data> [...]]]\n");
        printf("        writes <length> words to OTP at <address>\n");
        printf("        <data> are 32-bit words to be written, if less than <length> words are \n"
                "        specified,\n");
        printf("        remaining words are assumed to be 0x00\n");
        printf("    write_otp_raw_file <address> <file> [<size>]\n");
        printf("        writes up to <size> bytes of <file> into OTP at <address>\n");
        printf("        if <size> is omitted, the complete file is written\n");
        printf("        remaining bytes in the last word are set to 0x00\n");
        printf("    read_otp <address> <length>\n");
        printf("        reads <length> 32-bit words from OTP address <address>\n");
        printf("    write_otp_file <file>\n");
        printf("        writes data to OTP as defined in <file> (default values specified are \n"
                "        written)\n");
        printf("    read_otp_file <file>\n");
        printf("        reads data from OTP as defined in <file> (cells with default value \n"
                "        provided are read)\n");
        printf("        contents of each cell is printed to stdout\n");
        printf("    write_tcs <length> [<reg_addr> <reg_data> [<reg_addr> <reg_data>  [...]]]\n");
        printf("        writes <length> 64-bit words to OTP TCS section at first available (filled with 0) "
                        "section of size <length>\n");
        printf("        <reg_address>: the register address. It will be written as a 64bit word "
                        "[<reg_address>, ~<reg_address>] \n");
        printf("        <reg_data>: the register data. It will be written as a 64bit word "
                        "[<reg_data>, ~<reg_data>] \n");
        printf("    boot\n");
        printf("        boot 2nd stage bootloader or application binary (defined with -b) and \n"
                "        exit\n");
        printf("    read_chip_info\n");
        printf("        reads chip information \n");
        printf("\n");
        printf("examples:\n");
        printf("Upload \"test_api.bin\" program to RAM and run it, using UART Tx/Rx \n "
                "P1_0/P1_5 (uses boot rom booter baud rate at 57600):\n");
        printf("> cli_programmer -b test_api.bin COM40 boot\n\n");

        printf("Upload \"test_api.bin\" program to RAM and run it, using UART Tx/Rx \n "
                "P1_3/P2_3 (uses boot rom booter baud rate at 9600):\n");
        printf("> cli_programmer -s 9600 -b test_api.bin COM40 boot\n\n");

        printf("Run a few commands with uartboot, using UART Tx/Rx P1_0/P1_5 at baud rate \n"
                "115200 (initial rate for uartboot uploading must be 57600):\n");
        printf("> cli_programmer -i 57600 -s 115200 COM40 write_qspi 0x0 data_i\n");
        printf("> cli_programmer -i 57600 -s 115200 COM40 read_qspi 0x0 data_o 0x100\n\n");

        printf("Run a few commands with uartboot, using UART Tx/Rx P1_3/P2_3 at baud rate \n"
                "115200 (initial rate for uartboot uploading must be 9600):\n");
        printf("> cli_programmer -i 9600 -s 115200 --tx-port 1 --tx-pin 3 --rx-port 2 --rx-pin 3 \n"
                "                 COM40 write_qspi 0x0 data_i\n");
        printf("> cli_programmer -i 9600 -s 115200 --tx-port 1 --tx-pin 3 --rx-port 2 --rx-pin 3 \n"
                "                 COM40 read_qspi 0x0 data_o 0x100\n\n");

        printf("Read qspi flash contents (10 bytes at address 0x0) \n ");
        printf("Start gdbserver manually !\n");
        printf("> cli_programmer gdbserver read_qspi 0 -- 10 \n\n");
        printf("Write register 0x50003000 with value 0x00FF and register 0x50003002 with value 0x00AA \n ");
        printf("> cli_programmer gdbserver write_tcs 4 0x50003000 0x00FF 0x50003002 0x00AA \n\n");

        printf("Write settings to the cli_programmer.ini file.\n");
        printf("> cli_programmer -b c:\\users\\jon\\sdk\\bsp\\system\\loaders\\uartboot\\Release\\uartboot.bin "
                "--save-ini --gdb-cmd \"\\\"C:\\Program Files\\SEGGER\\JLink_V510d\\JLinkGDBServerCL.exe\\\" "
                "-if SWD -device Cortex-M0 -singlerun -silent -speed auto\"\n\n");
        printf("Write 6 bytes specified in command line to flash at address 0x80000\n");
        printf("> cli_programmer gdbserver write_qspi_bytes 0x80000 0x11 0x22 0x33 0x44 0x55 0x66\n\n");

        printf("Write SUOTA enable application to proper location in flash\n");
        printf("> cli_programmer gdbserver write_suota_image pxp_reporte.bin \"1.1.0.1 a\" \n\n");
}

static int opth_baudrate(const char * param)
{
        return get_number(param, &main_opts.uartboot_config.baudrate);
}

static int opth_initial_baudrate(const char *param)
{
        return get_number(param, &main_opts.initial_baudrate);
}

static int opth_timeout(const char *param)
{
        return get_number(param, &main_opts.timeout);
}

static int opth_gdb_port(const char *param)
{
        return get_number(param, &main_opts.gdb_server_config.port);
}

static int opth_tx_port(const char * param)
{
        return get_number(param, &main_opts.uartboot_config.tx_port);
}

static int opth_tx_pin(const char * param)
{
        return get_number(param, &main_opts.uartboot_config.tx_pin);
}

static int opth_rx_port(const char * param)
{
        return get_number(param, &main_opts.uartboot_config.rx_port);
}

static int opth_rx_pin(const char * param)
{
        return get_number(param, &main_opts.uartboot_config.rx_pin);
}

static int opth_bootloader(const char *param)
{
        set_str_opt(&main_opts.bootloader_fname, param);

        return 1;
}

int handle_option(char opt, const char *param)
{
        int ret = 0;

        switch (opt) {
        case '?':
        case 'h':
                opth_help();
                return -1;
        case 's':
                if (!param || !opth_baudrate(param)) {
                        fprintf(stderr, "invalid baudrate\n");
                        return -1;
                }
                main_opts.uartboot_config.baudrate_patch = 1;
                ret = 1;
                break;
        case 'i':
                if (!param || !opth_initial_baudrate(param)) {
                        fprintf(stderr, "invalid initial baudrate\n");
                        return -1;
                }
                ret = 1;
                break;
        case 'w':
                if (!param || !opth_timeout(param)) {
                        fprintf(stderr, "invalid timeout\n");
                        return -1;
                }
                ret = 1;
                break;
        case 'b':
                if (!param || !opth_bootloader(param)) {
                        fprintf(stderr, "invalid bootloader filename\n");
                        return -1;
                }
                ret = 1;
                break;
        case 'p':
                if (!param || !opth_gdb_port(param)) {
                        fprintf(stderr, "invalid gdbserver port\n");
                        return -1;
                }
                ret = 1;
                break;
        case 'r':
                if (!param) {
                        fprintf(stderr, "invalid gdbserver host name\n");
                        return -1;
                }

                set_str_opt(&main_opts.gdb_server_config.host_name, param);
                ret = 1;
                break;
        default:
                fprintf(stderr, "invalid parameter -%c\n", opt);
                return -1;
        }

        return ret;
}

int handle_long_option(const char * opt, const char * param) {
        char chip_rev[CHIP_REV_STRLEN] = {0};
        if (!strcmp(opt, "tx-port")) {
                if (!param || !opth_tx_port(param)) {
                        fprintf(stderr, "invalid tx port\n");
                        return -1;
                }
                main_opts.uartboot_config.tx_port_patch = 1;
                return 1;
        }
        if (!strcmp(opt, "tx-pin")) {
                if (!param || !opth_tx_pin(param)) {
                        fprintf(stderr, "invalid tx pin\n");
                        return -1;
                }
                main_opts.uartboot_config.tx_pin_patch = 1;
                return 1;
        }
        if (!strcmp(opt, "rx-port")) {
                if (!param || !opth_rx_port(param)) {
                        fprintf(stderr, "invalid rx port\n");
                        return -1;
                }
                main_opts.uartboot_config.rx_port_patch = 1;
                return 1;
        }
        if (!strcmp(opt, "rx-pin")) {
                if (!param || !opth_rx_pin(param)) {
                        fprintf(stderr, "invalid rx pin\n");
                        return -1;
                }
                main_opts.uartboot_config.rx_pin_patch = 1;
                return 1;
        }
        if (!strcmp(opt, "gdb-cmd")) {
                if (!param) {
                        fprintf(stderr, "invalid gdbserver command\n");
                        return -1;
                }

                set_str_opt(&main_opts.gdb_server_config.gdb_server_path, param);
                return 1;
        }
        if (!strcmp(opt, "no-kill")) {
                main_opts.gdb_server_config.stop_prv_gdb_server = false;
                return 0;
        }
        if (!strcmp(opt, "trc")) {
                if (!param) {
                        fprintf(stderr, "invalid target reset command\n");
                        return -1;
                }

                set_str_opt(&main_opts.target_reset_cmd, param);
                return 1;
        }
        if (!strcmp(opt, "save-ini")) {
                set_str_opt(&main_opts.config_file_path, "cli_programmer.ini");

                return 0;
        }
        if (!strcmp(opt, "save")) {
                if (!param) {
                        fprintf(stderr, "invalid configuration file path\n");
                        return -1;
                }

                set_str_opt(&main_opts.config_file_path, param);

                return 1;
        }
        if (!strcmp(opt, "cfg")) {
                if (!param) {
                        fprintf(stderr, "invalid configuration file path\n");
                        return -1;
                }
                if (!cli_config_load_from_ini_file(param, &main_opts)) {
                        fprintf(stderr, "failed to read configuration from %s\n", param);
                        return -1;
                }

                return 1;
        }
        if (!strcmp(opt, "prod-id")) {
                if ( (!param) || prog_map_product_id_to_chip_rev((char*)param, chip_rev) ){
                        fprintf(stderr, "invalid chip product id\n");
                        return -1;
                }
                set_str_opt(&main_opts.chip_rev, chip_rev);

                return 1;
        }
        if (!strcmp(opt, "enable-uart")) {
                if (!param || (tolower(*param) == 'y')) {
                        main_opts.enable_uart = true;
                        return param ? 1 : 0;
                } else if (tolower(*param) == 'n') {
                        main_opts.enable_uart = false;
                        return 1;
                } else {
                        fprintf(stderr, "invalid parameter for --enable-uart, only y or n are allowed\n");
                        return -1;
                }
        }
        if (!strcmp(opt, "ram-shuffling")) {
                if (param) {
                        long int lval;
                        char *endptr;

                        lval = strtol(param, &endptr, 10);
                        if (endptr > param) {
                                if (lval >=0 && lval < 4) {
                                        main_opts.ram_shuffling = (uint8_t) lval;
                                        return 1;
                                }
                        }
                        fprintf(stderr, "invalid parameter for ram-shuffling (%s),"
                                                                " only 0-3 are allowed\n", param);
                } else {
                        fprintf(stderr, "missing ram-shuffling, please provide number 0-3\n");
                }
                return -1;
        }
        fprintf(stderr, "invalid parameter --%s\n", opt);
        return -1;
}