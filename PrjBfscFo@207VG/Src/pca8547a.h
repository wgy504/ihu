#ifndef _PCA8547A_H_
#define _PCA8547A_H_
#include <inttypes.h>

int32_t PCA8574A_write(uint8_t val);
void PCA8574A_init(void);
int32_t PCA8574A_read(uint8_t *val);

#endif //_PCA8547A_H_
