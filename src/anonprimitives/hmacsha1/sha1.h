#ifndef __SHA1_H
#define __SHA1_H

#include <stdio.h>
#include <boost/cstdint.hpp>

namespace Sha1 {

#define SHA1_DIGLEN 20

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t  buffer[64];
} SHA1_CTX;

void SHA1Transform	(uint32_t state[5], uint8_t buffer[64]);
void SHA1_Init		(SHA1_CTX* context);
void SHA1_Update	(SHA1_CTX* context, uint8_t* data, uint32_t len);
void SHA1_Final		(uint8_t digest[20], SHA1_CTX* context);

} // namespace Sha1 

#endif /* __SHA1_H */

