#include "pwr_from.h"
#include "SystemInfo.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "ATcmd.h"
#include "aht20.h"
#include "ds3231.h"
#include "w25qxx.h" 
#include "DEV_Config.h"
#include "EPD_Command.h"
#include "GUI_Paint.h"
#include "Button.h"
#include "test.h"
#include "calendar.h"

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
		W25QXX_WAKEUP();
		AHT20_Init();
//		W25QXX_Init();
}

void AllSleep()
{
	EPD_4IN2_V2_Sleep();
	W25QXX_PowerDown();
}

//目前时间一个小时快0.12s
void RTC_AlarmATask(void *argument)
{
		uint8_t HiTpBat_flage = 0;
    initCalendar(10,70,60,10);
		HiTp(278,30);
		Bat(280,100);
		taskENTER_CRITICAL();
		xTaskCreate(TimeDisplayTask, "TimeDisplay", 256, NULL, 2, &TimeDisplayTask_Handle);
		vTaskSuspend(TimeDisplayTask_Handle);
		taskEXIT_CRITICAL();
    while (1)
    {
        if (xSemaphoreTake(rtcAlarmASemaphore, portMAX_DELAY) == pdTRUE && KEY_OK_Flage == 0)
        {
//            AllInit();
            printf("RTC_AlarmATask Run\r\n");
						HiTpBat_flage++;
            Get_RTC_Time();
						initCalendar(10,70,60,10);
						if(HiTpBat_flage>=3)
						{
							HiTp(278,30);
						}
						if(HiTpBat_flage>=10)
						{
							Bat(278,100);
							HiTpBat_flage = 0;
						}
//						AllSleep();
//            HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
        }
        else if (KEY_OK_Flage == 1)
        {
//            AllInit();
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
//        Get_RTC_Time();
				initCalendar(10,70,60,10);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void USAR_RTC_Alarm_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(rtcAlarmASemaphore, &xHigherPriorityTaskWoken);
}

void USAR_EXTI11_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    KEY_OK_Flage = 1; 
    xSemaphoreGiveFromISR(rtcAlarmASemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

