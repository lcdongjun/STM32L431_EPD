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
    char cmd[MAX_CMD_LEN];                // AT命令字符串
    void (*handler)(char *args);          // 处理该命令的函数
} AT_Command_t;

// AT指令处理函数原型
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



// 解析并执行AT指令
void AT_Command_Parser(char *input)
{
    char *cmd_end = strchr(input, '=');
    if (cmd_end != NULL) {
        *cmd_end = '\0';  // 把命令和参数分开
        char *args = cmd_end + 1;  // 获取参数部分

        // 遍历命令表查找匹配的命令
        for (int i = 0; i < CMD_TABLE_SIZE; i++) {
            if (strncmp(input, cmd_table[i].cmd, MAX_CMD_LEN) == 0) {
                // 调用对应命令的处理函数
                cmd_table[i].handler(args);
                return;
            }
        }
    }
    // 如果没有匹配的命令，可以在这里返回错误响应
    printf("Unknown command: %s\n", input);
}

// 添加 AT+HELP 命令处理函数
void AT_HELP(char *args)
{
    // 输出所有可用的AT命令
    printf("Available AT commands:\n");
    for (int i = 0; i < CMD_TABLE_SIZE; i++) {
        printf("%s\n", cmd_table[i].cmd);
    }
}

// AT+ADC_Value=1 启动任务，AT+ADC_Value=0 停止任务
void AT_ADC_Value(char *args)
{
    int enable = atoi(args);  // 将参数转为整数

    if (enable == 1) {
			printf("ADC Value Task already running.\r\n");
        // 创建ADC_Value任务
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
        // 删除ADC_Value任务
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

// AT+EXP_test=1 启动任务，AT+EXP_test=0 停止任务
void AT_EXP_test(char *args)
{
    int enable = atoi(args);  // 将参数转为整数

    if (enable == 1) {
			printf("EXP test Task already running.\r\n");
        // 创建ADC_Value任务
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
        // 删除ADC_Value任务
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

// AT+EPD_test=1 启动任务，AT+EPD_test=0 停止任务
void AT_EPD_test(char *args)
{
    int enable = atoi(args);  // 将参数转为整数

    if (enable == 1) {
			printf("EPD_test Task already running.\r\n");
        // 创建ADC_Value任务
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


// 设置写入地址和数据大小
uint32_t at_write_addr = 0;
uint32_t at_write_size = 0;

void AT_SetWrite(char *args)
{
    // 参数格式：Addr,DateSize
    char *comma_pos = strchr(args, ',');
    if (comma_pos != NULL) {
        *comma_pos = '\0';  // 把地址和数据大小分开
        at_write_addr = strtol(args, NULL, 16);  // 地址为十六进制
        at_write_size = atoi(comma_pos + 1);    // 数据大小为十进制
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
    // 参数格式：Addr,DateSize
    char *comma_pos = strchr(args, ',');
    if (comma_pos != NULL) {
        *comma_pos = '\0';  // 把地址和数据大小分开
        at_read_addr = strtol(args, NULL, 16);  // 地址为十六进制
        at_read_size = atoi(comma_pos + 1);    // 数据大小为十进制

        printf("ReadAddr: 0x%X, DateSize: %d\n", at_read_addr, at_read_size);
				AT_ReadDate();
    } else {
        printf("Invalid argument for AT+SetWrite\n");
    }
}

#define BUFFER_SIZE 256  // 定义缓冲区大小

void AT_ReadDate() 
{
    uint8_t buffer[BUFFER_SIZE]; 

    while (at_read_size > BUFFER_SIZE)
    {
        // 从闪存中读取BUFFER_SIZE大小的数据
        W25QXX_Read(buffer, at_read_addr, BUFFER_SIZE);
        
        // 发送数据到串口
        USART1_DMA_Send(buffer, BUFFER_SIZE);
        
        // 等待数据发送完成（此处使用一个非阻塞方式）
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET) {
            // 可以加一些超时控制或其他逻辑来防止死循环
        }

        // 更新读取地址和大小
        at_read_addr += BUFFER_SIZE;
        at_read_size -= BUFFER_SIZE;
    }

    // 处理最后剩余不足BUFFER_SIZE的数据
    if (at_read_size > 0)
    {
        // 读取剩余的部分
        W25QXX_Read(buffer, at_read_addr, at_read_size);
        
        // 发送剩余部分
        USART1_DMA_Send(buffer, at_read_size);
        
        // 等待发送完成
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET) {
            // 可以加一些超时控制或其他逻辑来防止死循环
        }
    }

    // 重置读取参数，准备下一次读取
    at_read_size = 0;
    at_read_addr = 0;
}
