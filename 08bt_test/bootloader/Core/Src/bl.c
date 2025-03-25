#include "bl.h"

#define BOOT_SIZE 0x10000 //boot程序大小
#define APP_SIZE 0x3FFFF //用户程序大小
#define APP_START_ADDRESS 0x8010000 // 用户程序起始地址
#define APP_START_END 0x803FFFF 		// 用户程序结束

// 标志位定义
#define BOOT_FLAG_NONE 0x00000000		// 无标志，直接进入用户程序
#define BOOT_FLAG_ADDRESS 0x2000BFF0  // 与用户程序中一致
#define BOOT_FLAG_VALUE   0xDEADBEEF  // 与用户程序中一致

#define SRAM1_START   0x20000000
#define SRAM1_END     0x20010000
#define SRAM2_START   0x10000000
#define SRAM2_END     0x10004000
		
// 设置进入 Bootloader 标志
void SetBootFlag(void)
{
    *(volatile uint32_t *)BOOT_FLAG_ADDRESS = BOOT_FLAG_VALUE;
}

// 清除进入 Bootloader 标志
void ClearBootFlag(void)
{
    *(volatile uint32_t *)BOOT_FLAG_ADDRESS = 0;
}
//检测是否存在进入 Bootloader 标志
uint8_t CheckBootFlag(void)
{
    if (*(volatile uint32_t *)BOOT_FLAG_ADDRESS == BOOT_FLAG_VALUE) {
        ClearBootFlag();
        return 1;
    }
    return 0;
}
// 解锁 Flash
void STM32_Flash_Init(void)
{
	HAL_FLASH_Unlock();
}

// 锁定 Flash
void STM32_Flash_DeInit(void)
{
	HAL_FLASH_Lock();
}

HAL_StatusTypeDef Flash_Erase_By_Size(uint32_t at_write_addr, uint32_t at_write_size)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError = 0;
    
    // 确保地址合法性
    if ((at_write_addr + at_write_size) > 0x3ffff) {
			printf("write_addr err 0x%x\r\n",at_write_addr);
			return HAL_ERROR;
    }

    // 计算起始页和结束页
    uint32_t firstPage = (at_write_addr + BOOT_SIZE) / FLASH_PAGE_SIZE;
    uint32_t lastPage = (at_write_addr + BOOT_SIZE + at_write_size - 1) / FLASH_PAGE_SIZE;

    // 解锁 Flash
    HAL_FLASH_Unlock();

    // 配置擦除参数
    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.Banks = FLASH_BANK_1;  // L431 只有 1 个 Bank
    eraseInit.Page = firstPage;
    eraseInit.NbPages = (lastPage - firstPage) + 1;

    // 执行擦除
    status = HAL_FLASHEx_Erase(&eraseInit, &pageError);

    // 上锁 Flash
    HAL_FLASH_Lock();

    return status;
}

//HAL_StatusTypeDef Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
//{
//    HAL_StatusTypeDef status = HAL_OK;
//    uint64_t data64 = 0;
//    uint16_t i = 0;

//    if (((WriteAddr + NumByteToWrite) > 0x3ffff)) {
//				printf("WriteAddr + NumByteToWrite err\r\n");
//        return HAL_ERROR;
//    }

//    while (i < NumByteToWrite) {
//        // 复制 8 字节数据
//        memcpy(&data64, pBuffer + i, sizeof(uint64_t));

//        // 写入 Flash
//				HAL_FLASH_Unlock();
//        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WriteAddr+APP_START_ADDRESS, data64);
//				HAL_FLASH_Lock();
//				if (status != HAL_OK) {
//							printf("Flash Write Err\r\n");
//							break;
//					}
//        WriteAddr += 8;
//        i += 8;
//    }

//    return status;
//}

HAL_StatusTypeDef Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint64_t data64 = 0;
    static uint8_t s_buffer[8] = {0};      // 静态缓冲区存储暂存数据
    static uint16_t s_buffered = 0;        // 已缓冲的字节数
    static uint32_t s_currentAddr = 0xFFFFFFFF; // 缓冲区的起始地址

    // 地址范围检查（需包含APP_START_ADDRESS）
    if ((WriteAddr + NumByteToWrite) > 0x3FFFF) {
        printf("Write address out of range\r\n");
        return HAL_ERROR;
    }

    // 处理已存在的缓冲数据
    if (s_buffered > 0) {
        // 检查地址连续性
        if (WriteAddr != s_currentAddr + s_buffered) {
            printf("Address not continuous with buffered data\r\n");
            return HAL_ERROR;
        }

        // 计算可合并的字节数
        uint16_t space = 8 - s_buffered;
        uint16_t copyBytes = (NumByteToWrite >= space) ? space : NumByteToWrite;

        // 合并数据到缓冲区
        memcpy(s_buffer + s_buffered, pBuffer, copyBytes);
        s_buffered += copyBytes;

        // 若缓冲区满，则写入
        if (s_buffered == 8) {
            memcpy(&data64, s_buffer, 8);
            HAL_FLASH_Unlock();
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                                      s_currentAddr + APP_START_ADDRESS, data64);
            HAL_FLASH_Lock();
            if (status != HAL_OK) {
                printf("Flash write error\r\n");
                s_buffered = 0; // 清空缓冲
                s_currentAddr = 0xFFFFFFFF;
                return status;
            }
            s_buffered = 0;
            s_currentAddr = 0xFFFFFFFF;
        }

        // 更新已处理的数据指针
        pBuffer += copyBytes;
        WriteAddr += copyBytes;
        NumByteToWrite -= copyBytes;

        // 如果已经没有剩余数据，直接返回
        if (NumByteToWrite == 0) {
            return HAL_OK;
        }
    }

    // 写入完整的8字节块（地址不需要对齐，由调用者管理）
    uint16_t fullBlocks = NumByteToWrite / 8;
    for (uint16_t i = 0; i < fullBlocks; i++) {
        memcpy(&data64, pBuffer + (i * 8), 8);
        HAL_FLASH_Unlock();
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                                  WriteAddr + APP_START_ADDRESS + (i * 8), 
                                  data64);
        HAL_FLASH_Lock();
        if (status != HAL_OK) {
            printf("Flash write error\r\n");
            return status;
        }
    }

    // 处理剩余不足8字节的数据
    uint16_t remaining = NumByteToWrite % 8;
    if (remaining > 0) {
        memcpy(s_buffer, pBuffer + (fullBlocks * 8), remaining);
        s_buffered = remaining;
        s_currentAddr = WriteAddr + (fullBlocks * 8);
    }

    return status;
}

//跳转到用户程序
void JumpToUserApplication(void)
{

    uint32_t appStackPointer = *(volatile uint32_t *)APP_START_ADDRESS;
    void (*AppEntryPoint)(void);

    if ((appStackPointer < SRAM1_START && appStackPointer>SRAM2_END) || appStackPointer > SRAM1_END || appStackPointer < SRAM2_START) {
				printf("appStackPointer :0x%x",appStackPointer);
        return;
    }

    __disable_irq();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    for (uint32_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __DSB();
    __ISB();

    HAL_RCC_DeInit();
    HAL_DeInit();
    
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY);

    __set_MSP(appStackPointer);

    AppEntryPoint = (void (*)(void))(*(volatile uint32_t *)(APP_START_ADDRESS + 4));

    __enable_irq();

    AppEntryPoint();
    while (1);
}
//检测是否存在用户程序
uint8_t IsUserProgramValid(void) 
{
    uint32_t appStack = *(volatile uint32_t *)APP_START_ADDRESS;
    uint32_t appResetHandler = *(volatile uint32_t *)(APP_START_ADDRESS + 4);

    if ((appStack < SRAM1_START && appStack>SRAM2_END) || appStack > SRAM1_END || appStack < SRAM2_START) {
				printf("appStackPointer :0x%x",appStack);
        return 0;
    }
		
    if (appResetHandler < APP_START_ADDRESS || appResetHandler > APP_START_END) {
      printf("\r\nappResetHandler:0x%x",appResetHandler);  
			return 0;
    }
    return 1;
}


