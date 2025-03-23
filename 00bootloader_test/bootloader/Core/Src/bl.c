#include "bl.h"

#define APP_START_ADDRESS 0x8010000 // 用户程序起始地址

// 标志位定义
#define BOOT_FLAG_NONE 0x00000000		// 无标志，直接进入用户程序
#define BOOT_FLAG_ADDRESS 0x2001FFF0  // 与用户程序中一致
#define BOOT_FLAG_VALUE   0xDEADBEEF  // 与用户程序中一致

// 设置进入 Bootloader 标志
void SetBootFlag(void) {
    *(volatile uint32_t *)BOOT_FLAG_ADDRESS = BOOT_FLAG_VALUE;
}

// 清除进入 Bootloader 标志
void ClearBootFlag(void) {
    *(volatile uint32_t *)BOOT_FLAG_ADDRESS = 0;
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


//void JumpToUserApplication(void)
//{
//	uint32_t appStackPointer = *(volatile uint32_t *)APP_START_ADDRESS; // 用户程序的 MSP 值
//	void (*AppEntryPoint)(void);										// 声明一个函数指针

//	// 判断用户程序是否有效（通常是检查 MSP 值是否合理）
//	if ((appStackPointer & 0x2FFE0000) != 0x20000000)
//	{			// RAM 区域起始地址检查
//		return; // 如果无效，不跳转
//	}

//	// 关闭全局中断
//	__disable_irq();

//	// 关闭 SysTick，复位到默认值
//	SysTick->CTRL = 0;
//	SysTick->LOAD = 0;
//	SysTick->VAL = 0;

//	// 关闭外设时钟
//	RCC->AHB1ENR = 0;
//	RCC->AHB2ENR = 0;
//	RCC->AHB3ENR = 0;
//	RCC->APB1ENR1 = 0;
//	RCC->APB1ENR2 = 0;
//	RCC->APB2ENR = 0;

//	// 切换系统时钟到 HSI
//	RCC->CR |= RCC_CR_HSION; // 启用 HSI
//	while ((RCC->CR & RCC_CR_HSIRDY) == 0)
//		;					// 等待 HSI 就绪
//	RCC->CFGR = 0x00000000; // 系统时钟切换到 HSI

//	// 关闭 PLL
//	RCC->CR &= ~RCC_CR_PLLON;
//	while (RCC->CR & RCC_CR_PLLRDY)
//		; // 等待 PLL 关闭

//	// 关闭所有中断并清除挂起标志
//	for (uint32_t i = 0; i < 8; i++)
//	{
//		NVIC->ICER[i] = 0xFFFFFFFF; // 禁用所有中断
//		NVIC->ICPR[i] = 0xFFFFFFFF; // 清除所有挂起中断
//	}

//	// 设置主堆栈指针（MSP）
//	__set_MSP(appStackPointer);

//	// 设置用户程序入口地址（复位向量地址）
//	AppEntryPoint = (void (*)(void))(*(volatile uint32_t *)(APP_START_ADDRESS + 4));

//	// 启用全局中断（用户程序可能需要）
//	__enable_irq();

//	// 跳转到用户程序
//	AppEntryPoint();

//	// 如果跳转失败（理论上不会执行到这里）
//	while (1)
//		;
//}

void JumpToUserApplication(void)
{
    #define SRAM_START   0x20000000
    #define SRAM_END     0x20010000

    uint32_t appStackPointer = *(volatile uint32_t *)APP_START_ADDRESS; // 用户程序的 MSP 值
    void (*AppEntryPoint)(void);                                        // 函数指针

//    if (appStackPointer < SRAM_START || appStackPointer > SRAM_END) {
//        return;
//    }

    __disable_irq();

    // 关闭 SysTick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    // 关闭所有外设时钟（适配 STM32L431 的 RCC 寄存器）
    RCC->AHB1ENR = 0;
    RCC->AHB2ENR = 0;
    RCC->AHB3ENR = 0;
    RCC->APB1ENR1 = 0; // 注意：L431 只有 APB1ENR 和 APB2ENR
		RCC->APB2ENR = 0;
    RCC->APB2ENR = 0;

    // 切换系统时钟到 HSI
    RCC->CR |= RCC_CR_HSION;        // 启用 HSI
    while (!(RCC->CR & RCC_CR_HSIRDY)); // 等待 HSI 就绪
    RCC->CFGR = 0x00000000;         // 切换系统时钟到 HSI

    // 关闭 PLL（可选，确保无残留时钟）
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY); // 等待 PLL 关闭

    // 关闭所有中断并清除挂起标志
    for (uint32_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF; // 禁用中断
        NVIC->ICPR[i] = 0xFFFFFFFF; // 清除挂起中断
    }

    // 设置主堆栈指针（MSP）
    __set_MSP(appStackPointer);

    // 设置用户程序入口地址（复位向量地址）
    AppEntryPoint = (void (*)(void))(*(volatile uint32_t *)(APP_START_ADDRESS + 4));

    // 启用全局中断（可选，用户程序可自行配置）
    __enable_irq();

    // 跳转到用户程序
    AppEntryPoint();

    // 跳转失败处理（通常不会执行到这里）
    while (1);
}

uint8_t IsUserProgramValid(void) 
{
    uint32_t appStack = *(volatile uint32_t *)APP_START_ADDRESS; // 用户程序栈顶地址
    uint32_t appResetHandler = *(volatile uint32_t *)(APP_START_ADDRESS + 4); // 用户程序复位向量

    // 栈指针检查（是否在 SRAM 范围内）
    if (appStack < 0x20000000 || appStack > 0x20008000) {
        return 0; // 无效
    }

    // 复位向量检查（是否在 Flash 范围内）
    if (appResetHandler < APP_START_ADDRESS || appResetHandler > 0x08003FFF) {
        return 0; // 无效
    }

    return 1; // 有效
}
