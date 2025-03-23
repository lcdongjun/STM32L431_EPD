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
    char *cmd;           // AT 指令名称
    void (*handler)(char *args); // 处理函数
    char *help;          // 命令帮助说明
} AT_Command_t;


// AT指令处理函数原型
void AT_HELP(char *args);
void AT_SetWrite(char *args);
void AT_WriteDate(char *args);
void AT_SetRead(char *args);
void AT_ReadDate(void);

AT_Command_t cmd_table[] = {
    {"AT+HELP", AT_HELP, "Shows this help message with a list of available commands."},
    {"AT+SetWrite", AT_SetWrite, "Usage: AT+SetWrite=0x00,size\n  Sets a value for write operation"},
    {"AT+WriteDate", AT_WriteDate, "Usage: AT+WriteDate=<date>\n  Write a specific date to the system"},
    {"AT+SetRead", AT_SetRead, "Usage: AT+SetRead=0x00,size\n  Sets a value for read operation"},
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
    printf("Unknown command: %s\nAT+HELP=1 \n", input);
}

void AT_HELP(char *args)
{
    // 输出所有可用的 AT 命令及其帮助信息
    printf("Available AT commands:\n");
    for (int i = 0; i < CMD_TABLE_SIZE; i++) {
        printf("%s\n", cmd_table[i].cmd);
        printf("Description: %s\n", cmd_table[i].help);
        printf("Example usage: %s\n\n", cmd_table[i].help);
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
