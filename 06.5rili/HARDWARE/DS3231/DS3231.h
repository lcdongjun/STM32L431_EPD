#ifndef __DS3231_H
#define __DS3231_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

/* DS3231 驱动头文件 */
#define DS3231_ADDR_WRITE 0xD0
#define DS3231_ADDR_READ  0xD1

// 寄存器地址定义
#define DS3231_TIME_REG    0x00
#define DS3231_CONTROL_REG 0x0E
#define DS3231_STATUS_REG  0x0F
#define DS3231_AGING_REG   0x10

// 控制寄存器位定义
#define DS3231_INTCN   (1 << 2)
#define DS3231_RS1     (1 << 3)
#define DS3231_RS2     (1 << 4)
#define DS3231_CONV    (1 << 5)
#define DS3231_BBSQW   (1 << 6)
#define DS3231_EOSC    (1 << 7)

// 状态寄存器位定义
#define DS3231_EN32KHZ (1 << 3)

typedef enum {
    SQW_1HZ,
    SQW_1024HZ,
    SQW_4096HZ,
    SQW_8192HZ
} SQW_Freq;

typedef struct DateTImeStruct{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t dayofmonth;
	uint8_t month;
	uint16_t year;
	uint8_t dayOfWeek;
}DateTime;


/*----------------------------------------------------------------------------*/
bool DS3231_GetTime(DateTime *dt);
bool DS3231_SetTime(DateTime *dt);
bool DS3231_SetSQW(SQW_Freq freq);
bool DS3231_Enable32kHz(bool enable);

#ifdef __cplusplus
}
#endif

#endif /* __DS3231_H */

