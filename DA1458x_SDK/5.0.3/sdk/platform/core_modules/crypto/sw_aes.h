 
#ifndef HEADER_AES_H
#define HEADER_AES_H

#include "os_int.h"

#define AES_ENCRYPT 1
#define AES_DECRYPT 0

/**************************************************************************
 * AES declarations 
 **************************************************************************/

#define AES_MAXROUNDS			14
#define AES_BLOCKSIZE		   16
#define AES_IV_SIZE			 16

#ifndef htonl
	#define htonl(a)					\
		((((a) >> 24) & 0x000000ff) |   \
		 (((a) >>  8) & 0x0000ff00) |   \
		 (((a) <<  8) & 0x00ff0000) |   \
		 (((a) << 24) & 0xff000000))
#endif

#ifndef ntohl
	#define ntohl(a)	htonl((a))
#endif


typedef struct aes_key_st 
{
	uint16_t rounds;
	uint16_t key_size;
	uint32_t ks[(AES_MAXROUNDS+1)*8];
	uint8_t iv[AES_IV_SIZE];
} AES_CTX;

typedef enum
{
	AES_MODE_128,
	AES_MODE_256
} AES_MODE;

/**
 * Set up AES with the key/iv and cipher size.
 */
void AES_set_key(AES_CTX *ctx, const uint8_t *key, 
		const uint8_t *iv, AES_MODE mode);

/**
 * Encrypt a byte sequence (with a block size 16) using the AES cipher.
 */
void AES_cbc_encrypt(AES_CTX *ctx, const uint8_t *msg, 
		uint8_t *out, int length);

/**
 * Decrypt a byte sequence (with a block size 16) using the AES cipher.
 */
void AES_cbc_decrypt(AES_CTX *ks, const uint8_t *in, uint8_t *out, int length);

/**
 * Change a key for decryption.
 */
void AES_convert_key(AES_CTX *ctx);

/**
 * Decrypt a single block (16 bytes) of data
 */
void AES_decrypt(const AES_CTX *ctx, uint32_t *data);

/**
 * Encrypt a single block (16 bytes) of data
 */
void AES_encrypt(const AES_CTX *ctx, uint32_t *data);


#endif /* !HEADER_AES_H */
