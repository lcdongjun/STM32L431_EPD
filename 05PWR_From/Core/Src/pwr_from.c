#include "pwr_from.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "ATcmd.h"
#include "aht20.h"
#include "ds1307.h"
#include "w25qxx.h" 
#include "DEV_Config.h"
#include "EPD_Command.h"
#include "GUI_Paint.h"
#include "Button.h"
#include "test.h"


void TimeDisplayTask(void *argument);

uint8_t KEY_OK_Flage = 0;

SemaphoreHandle_t rtcAlarmASemaphore;
TaskHandle_t RTC_AlarmATask_Handle;
TaskHandle_t TimeDisplayTask_Handle;


void AllInit()
{
		SystemClock_Config();
		MX_GPIO_Init();
		MX_DMA_Init();
		HAL_ADC_MspInit(&hadc1);
		MX_ADC1_Init();
		HAL_SPI_MspInit(&hspi1);
		MX_SPI1_Init();
		HAL_SPI_MspInit(&hspi2);
		MX_SPI2_Init();
		HAL_UART_MspInit(&huart1);
		HAL_UART_MspInit(&huart2);
		HAL_UART_MspInit(&huart3);
		MX_USART1_UART_Init();
		MX_USART2_UART_Init();
		MX_USART3_UART_Init();
//		MX_RTC_Init();
//		DWT_Init();
		AHT20_Init();
		W25QXX_Init();
}



void RTC_AlarmATask(void *argument)
{
    Piant_Time(130, 100);
		taskENTER_CRITICAL();
		xTaskCreate(TimeDisplayTask, "TimeDisplay", 256, NULL, 2, &TimeDisplayTask_Handle);
		vTaskSuspend(TimeDisplayTask_Handle);
		taskEXIT_CRITICAL();
    while (1)
    {
        if (xSemaphoreTake(rtcAlarmASemaphore, portMAX_DELAY) == pdTRUE && KEY_OK_Flage == 0)
        {
            AllInit();
            printf("RTC_AlarmATask Run\r\n");
            Get_RTC_Time();
            Piant_Time(130, 100);

            HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
        }
        else if (KEY_OK_Flage == 1)
        {
            AllInit();
            printf("KEY_OK start\r\n");

            vTaskResume(TimeDisplayTask_Handle);
            for (uint8_t i = 0; i <= 10; i++)
            {
                vTaskDelay(pdMS_TO_TICKS(12000));
            }
            KEY_OK_Flage = 0;

            printf("KEY_OK stop\r\n");
            vTaskSuspend(TimeDisplayTask_Handle);
        }
    }
}



void TimeDisplayTask(void *argument)
{
		vTaskDelay(pdMS_TO_TICKS(100));
    while (1)
    {
        Get_RTC_Time();
        Piant_Time(130, 100);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void USAR_RTC_Alarm_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(rtcAlarmASemaphore, &xHigherPriorityTaskWoken);
}

void USAR_EXTI15_10_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    KEY_OK_Flage = 1; 
    xSemaphoreGiveFromISR(rtcAlarmASemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

