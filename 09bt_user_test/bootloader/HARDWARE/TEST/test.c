#include "test.h"
#include "adc.h"
#include "gpio.h"
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
void Piant_BMP(uint32_t flashReadAddress,uint16_t xstart,uint16_t ystart,uint16_t xend,uint16_t yend);
void ADC_Value(void *pvParameters)
{
		uint16_t cont = 0;
		uint16_t ADC_Value;
		HAL_GPIO_WritePin(EN_CHK_VU_GPIO_Port,EN_CHK_VU_Pin,GPIO_PIN_SET);
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
	while(1)
	{
		Piant_BMP(0x500000,1,1,101,101);
		vTaskDelay(30000);
//			vTaskDelay(pdMS_TO_TICKS(1000));
//			uint8_t *BlackImage;
//			BlackImage = mymalloc(SRAMIN, ((EPD_4IN2_V2_WIDTH % 8 == 0) ? (EPD_4IN2_V2_WIDTH / 8) : (EPD_4IN2_V2_WIDTH / 8 + 1)) * EPD_4IN2_V2_HEIGHT);
//			if (BlackImage == NULL)
//			{
//					printf("Failed to apply for black memory...\r\n");
//			}	
//			
//			Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, EPD_WHITE);
//			Paint_SelectImage(BlackImage);
//			Paint_Clear(EPD_WHITE);
//			Paint_Show_Str(120,130,"hello RTC !!!",24,1,0);
//			EPD_4IN2_V2_Display(BlackImage);
//			myfree(SRAMIN,BlackImage);
//			printf("EPD_test OK!\r\n");
//			vTaskDelay(pdMS_TO_TICKS(1000000));
	}
//	vTaskDelete(EPD_test_Handle);
	
//	Piant_Time(100,100);
//	vTaskDelete(EPD_test_Handle);	
}


void Piant_BMP(uint32_t flashReadAddress,uint16_t xstart,uint16_t ystart,uint16_t xend,uint16_t yend)
{
		uint8_t *bmp;
		bmp = mymalloc(SRAMIN, ((xend-xstart)/8)*(yend-ystart));
		if (bmp == NULL)
		{
				printf("Failed to apply for bmp memory...\r\n");
		}
		Paint_NewImage(bmp,(xend-xstart), (yend-ystart), 0, EPD_WHITE);
		Paint_SelectImage(bmp);
		Paint_Clear(EPD_WHITE);
		W25QXX_Read(bmp, flashReadAddress, ((xend-xstart)/8)*(yend-ystart));
//		HAL_UART_Transmit(&huart1,bmp,(200/8)*156,0xfff);
		Paint_DrawBitMap(bmp);
		EPD_4IN2_V2_PartialDisplay(bmp,xstart,ystart,xend,yend);
		myfree(SRAMIN,bmp);
}
