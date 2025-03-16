#ifndef __DS3231_H
#define __DS3231_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* DS3231 ����ͷ�ļ� */
#define DS3231_ADDR_WRITE 0xD0
#define DS3231_ADDR_READ  0xD1

// �Ĵ�����ַ����
#define DS3231_TIME_REG    0x00
#define DS3231_CONTROL_REG 0x0E
#define DS3231_STATUS_REG  0x0F
#define DS3231_AGING_REG   0x10
#define DS3231_ALARM1_REG  0x07
#define DS3231_ALARM2_REG  0x0B

// ���ƼĴ���λ����
#define DS3231_INTCN   (1 << 2)
#define DS3231_RS1     (1 << 3)
#define DS3231_RS2     (1 << 4)
#define DS3231_CONV    (1 << 5)
#define DS3231_BBSQW   (1 << 6)
#define DS3231_EOSC    (1 << 7)

// ״̬�Ĵ���λ����
#define DS3231_EN32KHZ (1 << 3)

typedef enum {
    SQW_DISABLE,
    SQW_1HZ,
    SQW_1024HZ,
    SQW_4096HZ,
    SQW_8192HZ
} SQW_Freq;


// ����ƥ��ģʽö��
typedef enum {
    ALARM_MATCH_SECOND,      // ÿ�봥��
    ALARM_MATCH_MIN_SEC,     // ÿ��xx�봥��
    ALARM_MATCH_HOUR_MIN_SEC,// ÿ��xxʱxx��xx�봥��
    ALARM_MATCH_DATE_HOUR_MIN_SEC, // �ض����ڴ���
    ALARM_MATCH_DAY_HOUR_MIN_SEC   // �ض����ڴ���
} AlarmMatchMode;

// �������ýṹ��
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_or_date;
    AlarmMatchMode mode;
} AlarmConfig;

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
bool DS3231_Init(void);
bool DS3231_GetTime(DateTime *dt);
bool DS3231_SetTime(DateTime *dt);
bool DS3231_SetSQW(SQW_Freq freq);
bool DS3231_Enable32kHz(bool enable);
void PrintDateTime(const DateTime *dt);
bool DS3231_SetAlarm1(const AlarmConfig *config);
bool DS3231_ClearAlarmFlag(void);
bool DS3231_CheckAlarmTriggered(void);

#ifdef __cplusplus
}
#endif

#endif /* __DS3231_H */

