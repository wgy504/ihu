/**
 ****************************************************************************************
 *
 * @file bin2image.c
 *
 * @brief Utility for creating a bootable image from an executable raw binary.
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
#define _XOPEN_SOURCE 700
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif
#ifdef __linux__
#include <endian.h>
#endif
#include <string.h>
#include <errno.h>
#include <time.h>

#include "programmer.h"

#ifdef _MSC_VER
#  define RW_RET_TYPE	int
#  define snprintf	_snprintf
#  define S_IRUSR	S_IREAD
#  define S_IWUSR	S_IWRITE
#else
#  define RW_RET_TYPE	ssize_t
#endif


#define BIN2IMAGE_VERSION "1.10"


static void usage(const char* my_name)
{
	fprintf(stderr,
		"Version: " BIN2IMAGE_VERSION "\n"
		"\n"
		"  %s <type> <binary_file> <image_file> [ DA14680-00|DA14681-00|AD [enable_uart] [<ram_shuffling>] ]\n"
		"\n"
		"  Convert the executable binary file 'binary_file' to a\n"
		"  bootable image file 'image_file'.\n"
		"  The 'type' argument defines what kind of image to be \n"
		"  generated:\n"
		"    qspi_cached    		QSPI flash image for cached mode (production & development paths)\n"
		"    qspi_mirrored  		QSPI flash image for mirrored mode (production path only)\n"
		"    qspi_single_mirrored   	QSPI flash image for mirrored mode (development path only, qspi operating in single mode)\n"
		"    otp_cached     		OTP image for cached mode\n"
		"    otp_mirrored   		OTP image for mirrored mode\n"
                "\n"
                "  If argument 'DA14680-00' or 'DA14681-00' or 'AD' is passed, it commands the generation of\n"
                "  an image for version DA14680-00 of the chip, whose header is 16\n"
                "  bytes long, instead of 8.\n"
                "  If 'DA14680-00' or 'DA14681-00' or 'AD' is passed:\n"
                "   - 'enable_uart' can be used to enable UART during boot\n"
                "     (the default is not to enable it)\n"
                "   - 'ram_shuffling' can take value 0, 1, 2, or 3 to\n"
                "     configure the DataRAM cell shuffling:\n"
                "       0:  8 - 24 - 32 (default)\n"
                "       1: 24 -  8 - 32\n"
                "       2: 32 -  8 - 24\n"
                "       3: 32 - 24 -  8\n"
		"\n"
		"  Note: For cached mode images, the MSBit of the length is set to 1\n"
		"\n",
		my_name);
}


static int safe_write(int fd, const void* buf, size_t len)
{
	RW_RET_TYPE n;
	const uint8_t* _buf = buf;

	do {
		n = write(fd, _buf, len);
		if (n > 0) {
			len -= n;
			_buf += n;
		} else if (n < 0  &&  errno != EINTR)
			return -1;
	} while (len);

	return 0;
}


static int safe_read(int fd, void* buf, size_t len)
{
	RW_RET_TYPE n;
	uint8_t* _buf = buf;

	do {
		n = read(fd, _buf, len);
		if (n > 0) {
			len -= n;
			_buf += n;
		} else if (n == 0) {
			return len;  /* EOF: return number of bytes missing */
		} else if (n < 0  &&  errno != EINTR) {
			return -1;
		}
	} while (len);

	return 0;
}

int main(int argc, const char* argv[])
{
	int inf = -1, outf = -1;
	int oflags, res = EXIT_FAILURE;
        int binary_file_size;                   /* Input binary file size */
        int image_size;                         /* Output image size */
        uint8_t *buf = NULL;
	struct stat sbuf;
	char chip_ver[6] = "680AC";
	image_type_t type;
	image_mode_t mode;
        bool is_for_AD = false;
        bool AD_enable_uart = false;
        unsigned AD_ram_shuffling = 0;

	if (argc < 4 ) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	/* parse image type */
	if (strcmp(argv[1], "qspi_cached") == 0) {
		type = IMG_QSPI;
		mode = IMG_CACHED;
	} else if (strcmp(argv[1], "qspi_mirrored") == 0) {
		type = IMG_QSPI;
		mode = IMG_MIRRORED;
	} else if (strcmp(argv[1], "qspi_single_mirrored") == 0) {
		type = IMG_QSPI_S;
		mode = IMG_MIRRORED;
	} else if (strcmp(argv[1], "otp_cached") == 0) {
		type = IMG_OTP;
		mode = IMG_CACHED;
	} else if (strcmp(argv[1], "otp_mirrored") == 0) {
		type = IMG_OTP;
		mode = IMG_MIRRORED;
	} else {
		fprintf(stderr, "Invalid image type '%s'\n", argv[1]);
		usage(argv[0]);
		goto cleanup_and_exit;
	}

        if (argc > 4) {
                int i;

                if ((strcmp(argv[4], "DA14680-00") != 0) && (strcmp(argv[4], "DA14681-00") != 0)
                        && (strcmp(argv[4], "AD") != 0)) {
                        usage(argv[0]);
                        goto cleanup_and_exit;
                }

                chip_ver[4] = 'D';
                is_for_AD = true;

                /* parse DA1468(0/1)-00-specific options */
                for (i = 5; i < argc; i++) {
                        if (strcmp(argv[i], "enable_uart") == 0)
                                AD_enable_uart = true;
                        else {
                                /* it should be the 'ram_shuffling' option */
                                char *end;
                                long val;

				errno = 0;
                                val = strtol(argv[i], &end, 0);
                                if ((errno == ERANGE) || (errno != 0 && val == 0)) {
                                        perror("strtol");
                                        usage(argv[0]);
                                        goto cleanup_and_exit;
                                }
                                if (end == argv[i]) {
                                        usage(argv[0]);
                                        goto cleanup_and_exit;
                                }

                                AD_ram_shuffling = (unsigned)val;
                                if (AD_ram_shuffling > 3) {
                                        usage(argv[0]);
                                        goto cleanup_and_exit;
                                }
                        }
                }
        }

	/* open input file and get its size */
	oflags = O_RDONLY;
#ifdef O_BINARY
	oflags |= O_BINARY;
#endif
	inf = open(argv[2], oflags);
	if (-1 == inf) {
		perror(argv[2]);
		goto cleanup_and_exit;
	}
	if (fstat(inf, &sbuf)) {
		perror(argv[2]);
		goto cleanup_and_exit;
	}
	binary_file_size = (int) sbuf.st_size;

	/* open the output file */
	oflags = O_RDWR | O_CREAT | O_TRUNC;
#ifdef O_BINARY
	oflags |= O_BINARY;
#endif
	outf = open(argv[3], oflags, S_IRUSR | S_IWUSR);
	if (-1 == outf) {
		perror(argv[3]);
		goto cleanup_and_exit;
	}

	if (IMG_QSPI == type) {
                if (IMG_CACHED != mode && is_for_AD) {
                        fprintf(stderr, "Production mirror mode is not supported in chip version DA14680/1-00!\n");
                        goto cleanup_and_exit;
                }
        } else if (IMG_OTP == type) {
                if (is_for_AD) {
                        fprintf(stderr, "OTP images are not supported in chip version DA14680/1-00!\n");
                        goto cleanup_and_exit;
                }
        } else if (IMG_QSPI_S != type) {
                fprintf(stderr, "Unexpected image type %d\n", (int) type);
                goto cleanup_and_exit;
        }

        buf = malloc(binary_file_size + 16);
        if (NULL == buf) {
                fprintf(stderr, "Memory allocation error\n");
                goto cleanup_and_exit;
        }

        safe_read(inf, buf, binary_file_size);

        /* Request libprogrammer to prepare image */
        image_size = prog_make_image(buf, binary_file_size, chip_ver, AD_enable_uart,
                                AD_ram_shuffling, type, mode, buf, binary_file_size + 16, NULL);

        if (image_size < 0) {
                fprintf(stderr, "Insufficient memory buffer\n");
                goto cleanup_and_exit;
        }

        safe_write(outf, buf, image_size);

	res = EXIT_SUCCESS;

cleanup_and_exit:
        if (buf) {
                free(buf);
        }

        if (outf != -1) {
		if (close(outf))
			perror(argv[3]);
	}

	if (inf != -1) {
		if (close(inf))
			perror(argv[2]);
	}

	exit(res);
}

