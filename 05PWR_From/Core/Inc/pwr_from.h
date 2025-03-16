#ifndef __PWR_FROM_H
#define __PWR_FROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

extern SemaphoreHandle_t rtcAlarmASemaphore;
extern TaskHandle_t RTC_AlarmATask_Handle;
extern TaskHandle_t TimeDisplayTask_Handle;

void AllInit(void);
void RTC_AlarmATask(void *argument);
void USAR_RTC_Alarm_IRQHandler(void);
void USAR_EXTI15_10_IRQHandler(void);


#ifdef __cplusplus
}
#endif



#endif /* __PWR_FROM_H */
