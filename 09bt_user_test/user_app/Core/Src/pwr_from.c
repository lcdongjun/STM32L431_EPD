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

static uint8_t s_cont = 0;
static uint16_t HiTpBat_flage = 0;
static uint8_t KEY_OK_Flage = 0;

// 全局互斥量和信号量
SemaphoreHandle_t rtcAlarmASemaphore;
SemaphoreHandle_t rtcAlarmASemaphore = NULL;
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
		DWT_Init();
		W25QXX_WAKEUP();
		AHT20_Init();
}

void AllSleep()
{
	EPD_4IN2_V2_Sleep();
	W25QXX_PowerDown();
}

void InitOSResources(void)
{
    rtcAlarmASemaphore = xSemaphoreCreateBinary();
}

// 处理闹钟事件
static void HandleAlarmEvent(void)
{
    AllInit();
    printf("RTC_AlarmATask Run\r\n");
    
    HiTpBat_flage++;
    Sync_time();
    s_cont = SystemInfo.DT.second;
    initCalendar(10, 70, 60, 10);

    if (HiTpBat_flage >= 3) HiTp(278, 30);
    if (HiTpBat_flage >= 5)
		{
        Bat(278, 100);
        HiTpBat_flage = 0;
    }

    AllSleep();
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

// 处理按键事件
static void HandleKeyEvent(void)
{
    AllInit();
    printf("KEY_OK start\r\n");
    vTaskResume(TimeDisplayTask_Handle);
    vTaskDelay(pdMS_TO_TICKS(120000));
    KEY_OK_Flage = 0;
    printf("KEY_OK stop\r\n");
    vTaskSuspend(TimeDisplayTask_Handle);
		AllSleep();
}

// RTC闹钟任务
void RTC_AlarmATask(void *argument)
{
    Sync_time();
    initCalendar(10, 70, 60, 10);
    HiTp(278, 30);
    Bat(280, 100);
    xTaskCreate(TimeDisplayTask, "TimeDisplay", 256, NULL, 2, &TimeDisplayTask_Handle);
    vTaskSuspend(TimeDisplayTask_Handle);
		Sync_time();
    s_cont = SystemInfo.DT.second;
		DS3231_SetSQW(SQW_1HZ);
    while (1)
		{
        if (xSemaphoreTake(rtcAlarmASemaphore, portMAX_DELAY) == pdTRUE )
				{
            if (KEY_OK_Flage == 0 && s_cont == 60)
						{
                HandleAlarmEvent();
            }
						else if(KEY_OK_Flage == 0 && s_cont < 60)
						{
								HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
						}
						else if(KEY_OK_Flage == 1)
						{
                HandleKeyEvent();
            }
				}
    }
}

// 时间显示任务
void TimeDisplayTask(void *argument)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    while (1)
		{
        Sync_time();
				initCalendar(10, 70, 60, 10);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// RTC闹钟中断
void USAR_RTC_Alarm_IRQHandler()
{
    s_cont++;
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(rtcAlarmASemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    if (s_cont >= 61)
		{
        s_cont = 1;
    }
}

// 按键中断
void USAR_EXTI11_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    taskENTER_CRITICAL_FROM_ISR();
    KEY_OK_Flage = 1;
    taskEXIT_CRITICAL_FROM_ISR(xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(rtcAlarmASemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

