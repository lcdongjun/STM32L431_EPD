#include "DEV_Config.h"
#include "spi.h"
#include "sys.h"

void EPD_GPIO_Init()
{
	DEV_Delay_ms(500);
}

uint8_t DEV_SPI_WriteByte(UBYTE value)
{

    UBYTE RxData = 0;

    if (HAL_SPI_TransmitReceive(&hspi1, &value, &RxData, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        // 错误处理
        return 0xFF;
    }
    return RxData;
	
}

void DEV_SPI_Write_nByte(UBYTE *value, UDOUBLE len)
{
	uint8_t data;
	for (; len > 0; len--)
	{
		data = *value;			 // 读取当前字节
		DEV_SPI_WriteByte(data); // 发送字节
		value++;				 // 将指针移动到下一个字节
	}
}

void DEV_GPIO_Init()
{

}

void DEV_SPI_Init()
{

}

//void DEV_SPI_SendData(UBYTE Reg)
//{
//	UBYTE i, j = Reg;

//	F_EPD_MOSI_PIN = 1;
//	F_EPD_SCLK_PIN = 1;
//	F_EPD_CS_PIN = 0;
//	for (i = 0; i < 8; i++)
//	{
//		F_EPD_SCLK_PIN = 0;
//		if (j & 0x80)
//		{
//			F_EPD_MOSI_PIN = 1;
//		}
//		else
//		{
//			F_EPD_MOSI_PIN = 0;
//		}

//		F_EPD_SCLK_PIN = 1;
//		j = j << 1;
//	}
//	F_EPD_SCLK_PIN = 0;
//	F_EPD_CS_PIN = 1;
//}

//UBYTE DEV_SPI_ReadData()
//{
//	UBYTE i, j = 0xff;

//	F_EPD_MOSI_PIN = 0;
//	F_EPD_SCLK_PIN = 1;
//	F_EPD_CS_PIN = 0);
//	for (i = 0; i < 8; i++)
//	{
//		F_EPD_SCLK_PIN = 0;
//		j = j << 1;
//		if (DEV_Digital_Read(EPD_MOSI_PIN))
//		{
//			j = j | 0x01;
//		}
//		else
//		{
//			j = j & 0xfe;
//		}
//		F_EPD_SCLK_PIN = 1;
//	}
//	F_EPD_SCLK_PIN = 0;
//	F_EPD_CS_PIN = 1);
//	return j;
//}

int DEV_Module_Init(void)
{
//	F_EPD_DC_PIN = 0;
//	F_EPD_CS_PIN = 0;
//	F_EPD_PWR_PIN = 1;
//	F_EPD_RST_PIN = 1;
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port,EPD_DC_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_CS_GPIO_Port,EPD_CS_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_RES_GPIO_Port,EPD_RES_Pin,GPIO_PIN_SET);
	return 0;
}

void DEV_Module_Exit(void)
{
		HAL_GPIO_WritePin(EPD_DC_GPIO_Port,EPD_DC_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EPD_CS_GPIO_Port,EPD_CS_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EPD_RES_GPIO_Port,EPD_RES_Pin,GPIO_PIN_RESET);
}
