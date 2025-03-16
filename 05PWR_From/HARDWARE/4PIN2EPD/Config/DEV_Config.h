
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "cmsis_os.h"


/**
 * data
 **/
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

//#define F_EPD_RST_PIN PBout(0)
//#define F_EPD_DC_PIN PBout(1)
//#define F_EPD_PWR_PIN PBout(12)
//#define F_EPD_CS_PIN PBout(2)
////#define F_EPD_BUSY_PIN PAout(6)
//#define F_EPD_MOSI_PIN PAout(7)
//#define F_EPD_SCLK_PIN PAout(5)
/**
 * e-Paper GPIO
 **/
//#define EPD_RST_PIN GPIOB, GPIO_Pin_0
//#define EPD_DC_PIN GPIOB, GPIO_Pin_1
//#define EPD_PWR_PIN GPIOB, GPIO_Pin_12
//#define EPD_CS_PIN GPIOB, GPIO_Pin_2
//#define EPD_BUSY_PIN GPIOA, GPIO_Pin_6
//#define EPD_MOSI_PIN GPIOA, GPIO_Pin_7
//#define EPD_SCLK_PIN GPIOA, GPIO_Pin_5

/**
 * GPIO read and write
 **/
#define DEV_Digital_Write(_pin, _value) ((_value) == 0 ? HAL_GPIO_WritePin(_pin##_GPIO_Port, _pin##_Pin, GPIO_PIN_RESET) : HAL_GPIO_WritePin(_pin##_GPIO_Port, _pin##_Pin, GPIO_PIN_SET))
#define DEV_Digital_Read(_pin) HAL_GPIO_ReadPin(_pin##_GPIO_Port, _pin##_Pin)


/**
 * delay x ms
 **/
#define DEV_Delay_ms(__xms)  vTaskDelay(pdMS_TO_TICKS(__xms));

void EPD_GPIO_Init(void);

uint8_t DEV_SPI_WriteByte(UBYTE value);
void DEV_SPI_Write_nByte(UBYTE *value, UDOUBLE len);

int DEV_Module_Init(void);
void DEV_Module_Exit(void);
void DEV_GPIO_Init(void);
void DEV_SPI_Init(void);
void DEV_SPI_SendData(UBYTE Reg);
UBYTE DEV_SPI_ReadData(void);
#endif
