#include "ATcmd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "w25qxx.h"
#include "test.h"

#define MAX_CMD_LEN 64
#define MAX_ARG_LEN  32

uint8_t AT_FlashWrite_Flag = 0;

typedef struct {
    char cmd[MAX_CMD_LEN];                // AT�����ַ���
    void (*handler)(char *args);          // ���������ĺ���
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
AT_Command_t cmd_table[] = {
		{"AT+HELP", AT_HELP},
    {"AT+ADC_Value", AT_ADC_Value},
		{"AT+EXP_test", AT_EXP_test},
		{"AT+EPD_test", AT_EPD_test},
    {"AT+SetWrite", AT_SetWrite},
    {"AT+WriteDate", AT_WriteDate},
		{"AT+SetRead", AT_SetRead},
//		{"AT+ReadDate", AT_ReadDate},
		
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
    printf("Unknown command: %s\n", input);
}

// ��� AT+HELP �������
void AT_HELP(char *args)
{
    // ������п��õ�AT����
    printf("Available AT commands:\n");
    for (int i = 0; i < CMD_TABLE_SIZE; i++) {
        printf("%s\n", cmd_table[i].cmd);
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

// AT+EXP_test=1 ��������AT+EXP_test=0 ֹͣ����
void AT_EXP_test(char *args)
{
    int enable = atoi(args);  // ������תΪ����

    if (enable == 1) {
			printf("EXP test Task already running.\r\n");
        // ����ADC_Value����
        if (ADC_Value_Handle == NULL) 
					{
							xTaskCreate((TaskFunction_t )EXP_test,             
													(const char*    )"EXP_test",           
													(uint16_t       )128,        
													(void*          )NULL,                  
													(UBaseType_t    )2,        
													(TaskHandle_t*  )&EXP_test_Handle);  
					} 
				else 
				{
            printf("EXP test Task already running.\n");
        }
    } else if (enable == 0) 
		{
			printf("EXP test Task stopped.\n");
        // ɾ��ADC_Value����
        if (EXP_test_Handle != NULL) 
				{
            vTaskDelete(EXP_test_Handle);
            EXP_test_Handle = NULL;
            printf("EXP test Task stopped.\n");
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
void AT_EPD_test(char *args)
{
    int enable = atoi(args);  // ������תΪ����

    if (enable == 1) {
			printf("EPD_test Task already running.\r\n");
        // ����ADC_Value����
        if (ADC_Value_Handle == NULL) 
					{
						xTaskCreate((TaskFunction_t )EPD_test,             
												(const char*    )"EPD_test",           
												(uint16_t       )128,        
												(void*          )&EPD_test_Handle,                  
												(UBaseType_t    )2,
													NULL
												); 
					} 
				else 
				{
            printf("EPD_test Task already running.\n");
        }
    } else if (enable == 0) 
		{
			printf("EPD_test Task stopped.\n");
        if (EPD_test_Handle != NULL) 
				{
            vTaskDelete(EPD_test_Handle);
            EPD_test_Handle = NULL;
            printf("EPD_test Task stopped.\n");
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
