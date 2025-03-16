#include "test.h"
#include "adc.h"
#include "rtc.h"
#include "gpio.h"
#include "aht20.h"
#include "w25qxx.h" 
#include "DEV_Config.h"
#include "EPD_Command.h"
#include "GUI_Paint.h"
#include "Button.h"


TaskHandle_t ADC_Value_Handle = NULL;
TaskHandle_t EPD_test_Handle = NULL;
TaskHandle_t EXP_test_Handle = NULL;


void Get_RTC_Time(void);
void Piant_Time(uint8_t Xstart,uint8_t Ystart);

void ADC_Value(void *pvParameters)
{
		uint16_t cont = 0;
		uint16_t ADC_Value;
	while(1)
	{
		HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1,50);
	  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
	  {
			ADC_Value = HAL_ADC_GetValue(&hadc1);
	  }
		printf(" AD1 value = %1.3fV \r\n", ADC_Value*2.45f/4096*2);
		vTaskDelay(pdMS_TO_TICKS(1000));
		cont++;
		if(cont==5)
		{
			HAL_GPIO_WritePin(EN_CHK_VU_GPIO_Port,EN_CHK_VU_Pin,GPIO_PIN_RESET);
		}
		else if(cont ==10)
		{
			cont = 0;
			HAL_GPIO_WritePin(EN_CHK_VU_GPIO_Port,EN_CHK_VU_Pin,GPIO_PIN_SET);
		}
		HAL_ADC_Stop(&hadc1);
	}
}

void EPD_test(void *pvParameters)
{
	DEV_Module_Init();
	EPD_4IN2_V2_Init();
	EPD_4IN2_V2_Clear();
	vTaskDelay(pdMS_TO_TICKS(1000));
	uint8_t *BlackImage;
	BlackImage = mymalloc(SRAMIN, ((EPD_4IN2_V2_WIDTH % 8 == 0) ? (EPD_4IN2_V2_WIDTH / 8) : (EPD_4IN2_V2_WIDTH / 8 + 1)) * EPD_4IN2_V2_HEIGHT);
	if (BlackImage == NULL)
	{
			printf("Failed to apply for black memory...\r\n");
	}	
	
	Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, EPD_WHITE);
	Paint_SelectImage(BlackImage);
	Paint_Clear(EPD_WHITE);
	Paint_Show_Str(120,130,"Hello STM32 !!!",24,1,0);
	EPD_4IN2_V2_Display(BlackImage);
	myfree(SRAMIN,BlackImage);
	vTaskDelete(EPD_test_Handle);
	
//	Piant_Time(100,100);
//	vTaskDelete(EPD_test_Handle);	
}

void EXP_test(void *pvParameters)
{
		AHT20_Data aht20_data;
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
//	W25QXX_Write(char1,0x0000,sizeof(char1));
		uint8_t *char2;
		char2 = mymalloc(SRAMIN, 128);
		W25QXX_Read(char2,0x0000,128);
		printf("%s",char2);
		AHT20_ReadData(&aht20_data);
		printf("humidity: %f\r\n",aht20_data.humidity);
		printf("temp: %f\r\n",aht20_data.temperature);
		Get_RTC_Time();
		myfree(SRAMIN,char2);
	}
}

void Get_RTC_Time(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // 获取时间和日期
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // 打印时间和日期
    printf("Time: %02d:%02d:%02d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
    printf("Date: %02d-%02d-%02d\n", sDate.Year, sDate.Month, sDate.Date);
}

uint8_t lastHour = 0xFF;   // 初始化为无效值
uint8_t lastMinute = 0xFF; // 初始化为无效值
void Piant_Time(uint8_t Xstart,uint8_t Ystart)
{

	RTC_TimeTypeDef sTime = {0};
	char timeStr[6]; 
	uint8_t *BlackImage2;
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		if (sTime.Minutes != lastMinute) 
		{
			BlackImage2 = mymalloc(SRAMIN, (130/8)*50);
			if (BlackImage2 == NULL)
			{
					printf("Failed to apply for black memory...\r\n");
			}
			memset(BlackImage2, 0, (130/8)*50);
			
			Paint_NewImage(BlackImage2,130, 50, 0, EPD_WHITE);
			Paint_SelectImage(BlackImage2);
			Paint_Clear(EPD_WHITE);
			lastMinute = sTime.Minutes;
			lastHour = sTime.Hours;
			snprintf(timeStr, sizeof(timeStr), "%02d:%02d", sTime.Hours, sTime.Minutes);
			Paint_Show_Str(0, 0,(u8*)timeStr, 48, 1, 1);
			EPD_4IN2_V2_PartialDisplay(BlackImage2,Xstart,Ystart,Xstart+130,Ystart+50);
			myfree(SRAMIN,BlackImage2);
		}

}
