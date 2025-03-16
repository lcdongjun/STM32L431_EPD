#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


void DWT_Init(void);
void Delay_us(unsigned long  us);

#ifdef __cplusplus
}
#endif



#endif /* __DELAY_H */
