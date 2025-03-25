#ifndef __TEST_H
#define __TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

void ADC_Value(void *pvParameters);
void EPD_test(void *pvParameters);
void EXP_test(void *pvParameters);
void Get_RTC_Time(void);
void Piant_Time(uint8_t Xstart,uint8_t Ystart);
void Piant_BMP(uint32_t flashReadAddress,uint16_t xstart,uint16_t ystart,uint16_t xend,uint16_t yend);
void HiTp(uint32_t xstart ,uint32_t ystart);
void Bat(uint32_t xstart,uint32_t ystart);
#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */

