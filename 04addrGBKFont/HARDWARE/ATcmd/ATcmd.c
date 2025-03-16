#include "ATcmd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "w25qxx.h"
#include "test.h"

#define MAX_CMD_LEN 64
#define MAX_ARG_LEN  32

uint8_t AT_FlashWrite_Flag = 0;
extern RTC_HandleTypeDef hrtc;

typedef struct {
    char *cmd;           // AT ָ������
    void (*handler)(char *args); // ������
    char *help;          // �������˵��
} AT_Command_t;


// ATָ�����ԭ��
void AT_HELP(char *args);
void AT_ADC_Value(char *args);
void AT_SetWrite(char *args);
void AT_WriteDate(char *args);
void AT_EXP_test(char *args);
void AT_EPD_test(char *args);
void AT_SetRead(char *args);
void AT_ReadDate(void);
void AT_SetRTC(char *args);

AT_Command_t cmd_table[] = {
    {"AT+HELP", AT_HELP, "Shows this help message with a list of available commands."},
    {"AT+ADC_Value", AT_ADC_Value, "Usage: AT+ADC_Value=<1/0>\n  1 - Start ADC task\n  0 - Stop ADC task"},
    {"AT+EXP_test", AT_EXP_test, "Usage: AT+EXP_test=<1/0>\n  1 - Start EXP test task\n  0 - Stop EXP test task"},
    {"AT+EPD_test", AT_EPD_test, "Usage: AT+EPD_test=<1/0>\n  1 - Start EPD test task\n  0 - Stop EPD test task"},
    {"AT+SetWrite", AT_SetWrite, "Usage: AT+SetWrite=0x00,size\n  Sets a value for write operation"},
    {"AT+WriteDate", AT_WriteDate, "Usage: AT+WriteDate=<date>\n  Write a specific date to the system"},
    {"AT+SetRead", AT_SetRead, "Usage: AT+SetRead=0x00,size\n  Sets a value for read operation"},
    {"AT+SetRTC", AT_SetRTC, "Usage: AT+SetRTC=YYYY-MM-DD HH:MM:SS WeekDay\n  Set the RTC time in the format YYYY-MM-DD HH:MM:SS WeekDay (1=Monday, ..., 7=Sunday)"},
};

#define CMD_TABLE_SIZE (sizeof(cmd_table) / sizeof(cmd_table[0]))



// ������ִ��ATָ��
void AT_Command_Parser(char *input)
{
    char *cmd_end = strchr(input, '=');
    if (cmd_end != NULL) {
        *cmd_end = '\0';  // ������Ͳ����ֿ�
        char *args = cmd_end + 1;  // ��ȡ��������

        // ������������ƥ�������
        for (int i = 0; i < CMD_TABLE_SIZE; i++) {
            if (strncmp(input, cmd_table[i].cmd, MAX_CMD_LEN) == 0) {
                // ���ö�Ӧ����Ĵ�����
                cmd_table[i].handler(args);
                return;
            }
        }
    }
    // ���û��ƥ���������������ﷵ�ش�����Ӧ
    printf("Unknown command: %s\nAT+HELP=1 \n", input);
}

void AT_HELP(char *args)
{
    // ������п��õ� AT ����������Ϣ
    printf("Available AT commands:\n");
    for (int i = 0; i < CMD_TABLE_SIZE; i++) {
        printf("%s\n", cmd_table[i].cmd);
        printf("Description: %s\n", cmd_table[i].help);
        printf("Example usage: %s\n\n", cmd_table[i].help);
    }
}

// AT+ADC_Value=1 ��������AT+ADC_Value=0 ֹͣ����
void AT_ADC_Value(char *args)
{
    int enable = atoi(args);  // ������תΪ����

    if (enable == 1) {
			printf("ADC Value Task already running.\r\n");
        // ����ADC_Value����
        if (ADC_Value_Handle == NULL) 
					{
							xTaskCreate((TaskFunction_t )ADC_Value,             
													(const char*    )"ADC_Value",           
													(uint16_t       )128,        
													(void*          )NULL,                  
													(UBaseType_t    )2,        
													(TaskHandle_t*  )&ADC_Value_Handle); 
					} 
				else 
				{
            printf("ADC Value Task already running.\n");
        }
    } else if (enable == 0) 
		{
			printf("ADC Value Task stopped.\n");
        // ɾ��ADC_Value����
        if (ADC_Value_Handle != NULL) 
				{
            vTaskDelete(ADC_Value_Handle);
            ADC_Value_Handle = NULL;
            printf("ADC Value Task stopped.\n");
        } 
				else 
				{
            printf("ADC Value Task is not running.\n");
        }
    } 
		else 
		{
        printf("Invalid argument for AT+ADC_Value\n");
    }
}

uint8_t EXP_testFlage = 0;
// AT+EXP_test=1 ��������AT+EXP_test=0 ֹͣ����
void AT_EXP_test(char *args)
{
    int enable = atoi(args);  // ������תΪ����

    if (enable == 1) {
			printf("EXP test Task already running.\r\n");
        // ����ADC_Value����
        if (EXP_testFlage==0) 
					{
							xTaskCreate((TaskFunction_t )EXP_test,             
													(const char*    )"EXP_test",           
													(uint16_t       )128,        
													(void*          )NULL,                  
													(UBaseType_t    )2,        
													(TaskHandle_t*  )&EXP_test_Handle);
							EXP_testFlage=1;
					} 
				else 
				{
            printf("EXP test Task already running.\n");
        }
    } else if (enable == 0) 
		{
			printf("EXP test Task stopped.\n");
        // ɾ��ADC_Value����
        if (EXP_testFlage==1) 
				{
            vTaskDelete(EXP_test_Handle);
            printf("EXP test Task stopped.\n");
						EXP_testFlage=0;
        } 
				else 
				{
            printf("EXP test Task is not running.\n");
        }
    } 
		else 
		{
        printf("Invalid argument for AT+EXP_test\n");
    }
}

// AT+EPD_test=1 ��������AT+EPD_test=0 ֹͣ����
uint8_t EPD_testFlage = 0;
void AT_EPD_test(char *args)
{
    int enable = atoi(args);  // ������תΪ����

    if (enable == 1) {
			printf("EPD_test Task already running.\r\n");
        // ����ADC_Value����
        if (EPD_testFlage==0) 
					{
						xTaskCreate((TaskFunction_t )EPD_test,             
													(const char*    )"EPD_test",           
													(uint16_t       )256,        
													(void*          )NULL,                  
													(UBaseType_t    )2,        
													(TaskHandle_t*  )&EPD_test_Handle);
						EPD_testFlage=1;
					} 
				else 
				{
            printf("EPD_test Task already running.\n");
        }
    } else if (enable == 0) 
		{
        if (EPD_testFlage==1) 
				{
            vTaskDelete(EPD_test_Handle);
            printf("EPD_test Task stopped.\n");
						EPD_testFlage=0;
        } 
				else 
				{
            printf("EPD_test Task is not running.\n");
        }
    } 
		else 
		{
        printf("Invalid argument for AT+EPD_test\n");
    }
}


// ����д���ַ�����ݴ�С
uint32_t at_write_addr = 0;
uint32_t at_write_size = 0;

void AT_SetWrite(char *args)
{
    // ������ʽ��Addr,DateSize
    char *comma_pos = strchr(args, ',');
    if (comma_pos != NULL) {
        *comma_pos = '\0';  // �ѵ�ַ�����ݴ�С�ֿ�
        at_write_addr = strtol(args, NULL, 16);  // ��ַΪʮ������
        at_write_size = atoi(comma_pos + 1);    // ���ݴ�СΪʮ����
        printf("WriteAddr: 0x%X, DateSize: %d\n", at_write_addr, at_write_size);
				W25QXX_Erase_By_Size(at_write_addr,at_write_size);
				printf("Erase OK\r\n");
    } else {
        printf("Invalid argument for AT+SetWrite\n");
    }
}

void AT_WriteDate(char *args)
{
			AT_FlashWrite_Flag = 1;
			printf("FlashWriteDateTask Run\r\n");
}


uint32_t at_read_addr = 0;
uint32_t at_read_size = 0;

void AT_SetRead(char *args)
{
    // ������ʽ��Addr,DateSize
    char *comma_pos = strchr(args, ',');
    if (comma_pos != NULL) {
        *comma_pos = '\0';  // �ѵ�ַ�����ݴ�С�ֿ�
        at_read_addr = strtol(args, NULL, 16);  // ��ַΪʮ������
        at_read_size = atoi(comma_pos + 1);    // ���ݴ�СΪʮ����

        printf("ReadAddr: 0x%X, DateSize: %d\n", at_read_addr, at_read_size);
				AT_ReadDate();
    } else {
        printf("Invalid argument for AT+SetWrite\n");
    }
}

#define BUFFER_SIZE 256  // ���建������С

void AT_ReadDate() 
{
    uint8_t buffer[BUFFER_SIZE]; 

    while (at_read_size > BUFFER_SIZE)
    {
        // �������ж�ȡBUFFER_SIZE��С������
        W25QXX_Read(buffer, at_read_addr, BUFFER_SIZE);
        
        // �������ݵ�����
        USART1_DMA_Send(buffer, BUFFER_SIZE);
        
        // �ȴ����ݷ�����ɣ��˴�ʹ��һ����������ʽ��
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET) {
            // ���Լ�һЩ��ʱ���ƻ������߼�����ֹ��ѭ��
        }

        // ���¶�ȡ��ַ�ʹ�С
        at_read_addr += BUFFER_SIZE;
        at_read_size -= BUFFER_SIZE;
    }

    // �������ʣ�಻��BUFFER_SIZE������
    if (at_read_size > 0)
    {
        // ��ȡʣ��Ĳ���
        W25QXX_Read(buffer, at_read_addr, at_read_size);
        
        // ����ʣ�ಿ��
        USART1_DMA_Send(buffer, at_read_size);
        
        // �ȴ��������
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET) {
            // ���Լ�һЩ��ʱ���ƻ������߼�����ֹ��ѭ��
        }
    }

    // ���ö�ȡ������׼����һ�ζ�ȡ
    at_read_size = 0;
    at_read_addr = 0;
}

// ���� RTC ʱ�䲢��������
void RTC_SetTime(int year, int month, int day, int hour, int minute, int second, int weekday)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    // ��������
    sDate.Year = year - 2000;  // RTC ����ݴ� 2000 ��ʼ
    sDate.Month = month;
    sDate.Date = day;
    sDate.WeekDay = weekday;   // �������ڣ�1 = Monday, ..., 7 = Sunday

    // ����ʱ��
    sTime.Hours = hour;
    sTime.Minutes = minute;
    sTime.Seconds = second;
    sTime.TimeFormat = RTC_HOURFORMAT_24;  // 24 Сʱ��

    // ����ʱ�������
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        printf("Error setting RTC time\n");
    }

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        printf("Error setting RTC date\n");
    }

    printf("RTC time set to: %04d-%02d-%02d %02d:%02d:%02d, Weekday: %d\n", year, month, day, hour, minute, second, weekday);
}

// ���������� RTC ʱ��� AT �������
void AT_SetRTC(char *args)
{
    int year, month, day, hour, minute, second, weekday;

    // ����ʱ���ַ����������ʽ�� YYYY-MM-DD HH:MM:SS WeekDay��
    if (sscanf(args, "%4d-%2d-%2d %2d:%2d:%2d %d", &year, &month, &day, &hour, &minute, &second, &weekday) == 7) {
        // �ж�����ֵ�Ƿ�Ϸ�
        if (weekday < 1 || weekday > 7) {
            printf("Invalid weekday. Please provide a value between 1 (Monday) and 7 (Sunday).\n");
            return;
        }

        // ���� RTC ʱ��
        RTC_SetTime(year, month, day, hour, minute, second, weekday);
    } else {
        printf("Invalid argument for AT+SetRTC. Please use the format YYYY-MM-DD HH:MM:SS WeekDay\n");
    }
}
