#ifndef __TEST_H
#define __TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

extern TaskHandle_t ADC_Value_Handle;
extern TaskHandle_t EPD_test_Handle;
extern TaskHandle_t EXP_test_Handle;

void ADC_Value(void *pvParameters);
void EPD_test(void *pvParameters);
void EXP_test(void *pvParameters);
void Get_RTC_Time(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */

