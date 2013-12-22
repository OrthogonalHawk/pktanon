#include <sys/types.h>
#include <memory.h>
#include "sha1.h"
#include "sha1_hmac.h"

#define IPAD_BYTE 0x36
#define OPAD_BYTE 0x5C
#define B 64
#define MIN(a,b)	(a < b ? a : b)

void HmacSha1::sha1_hmac(uint8_t *dest, uint8_t *key, size_t klen, uint8_t *text, size_t tlen)
{
	Sha1::SHA1_CTX sha;
	char kipad[B];
	char kopad[B];
	char digest[SHA1_DIGLEN];
	char k[SHA1_DIGLEN];
	int i;

	/* Hash the key if longer than the block size */
	if (klen > B) {
		Sha1::SHA1_Init(&sha);
		Sha1::SHA1_Update(&sha, (uint8_t*) key, (int32_t)klen);
		Sha1::SHA1_Final((uint8_t*)k, &sha);
		key = (uint8_t*)k;
		klen = SHA1_DIGLEN;
	}

	/* Create K xor ipad and k xor opad */
	memset(kipad, 0, B);
	memcpy(kipad, key, MIN(klen, B));
	memcpy(kopad, kipad, B);
	for (i = 0; i < B; i++) {
		kipad[i] ^= IPAD_BYTE;
		kopad[i] ^= OPAD_BYTE;
	}

	/* Compute H(K xor ipad, text) */
	Sha1::SHA1_Init(&sha);
	Sha1::SHA1_Update(&sha, (uint8_t*)kipad, B);
	Sha1::SHA1_Update(&sha, (uint8_t*)text, (int)tlen);
	Sha1::SHA1_Final((uint8_t*)digest, &sha);

	/* Compute H(K xor opad, H(K xor ipad, text)) */
	Sha1::SHA1_Init(&sha);
	Sha1::SHA1_Update(&sha, (uint8_t*)kopad, B);
	Sha1::SHA1_Update(&sha, (uint8_t*)digest, SHA1_DIGLEN);
	Sha1::SHA1_Final((uint8_t*)dest, &sha);

	memset(kipad, 0, B);
	memset(kopad, 0, B);
	memset(digest, 0, SHA1_DIGLEN);
	/* It's just not good programming pratice to clear 'key' here.
	* It should be done by the caller */
}

/* version with 96-bit truncation */
void HmacSha1::sha1_hmac_96(uint8_t *dest, uint8_t *key, size_t klen, uint8_t *text, size_t tlen)
{
	int8_t digest[SHA1_DIGLEN];

	sha1_hmac((uint8_t*)&digest, key, klen, text, tlen);
	memcpy(dest, digest, 96/8);
}

