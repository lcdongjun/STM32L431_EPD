#include "w25qxx.h" 
#include "spi.h"   
#include "usart.h"	

uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData = 0;

    if (HAL_SPI_TransmitReceive(&hspi2, &TxData, &RxData, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        // 错误处理
        return 0xFF;
    }

    return RxData;
}

uint16_t W25QXX_TYPE = W25Q128;	// 默认是W25Q128

// 初始化SPI FLASH的IO口
void W25QXX_Init(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);  // SPI FLASH不选中
    W25QXX_TYPE = W25QXX_ReadID();  // 读取FLASH ID.
    printf("FLASH_ID: 0X%X\r\n", W25QXX_TYPE);
}

// 读取W25QXX的状态寄存器
uint8_t W25QXX_ReadSR(void)   
{  
    uint8_t byte = 0; 
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件   
    SPI2_ReadWriteByte(W25X_ReadStatusReg);    // 发送读取状态寄存器命令    
    byte = SPI2_ReadWriteByte(0Xff);             // 读取一个字节     
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选     
    return byte;   
} 

// 写W25QXX状态寄存器
void W25QXX_Write_SR(uint8_t sr)   
{   
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件   
    SPI2_ReadWriteByte(W25X_WriteStatusReg);   // 发送写取状态寄存器命令    
    SPI2_ReadWriteByte(sr);               // 写入一个字节  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选    	      
}   

// W25QXX写使能
void W25QXX_Write_Enable(void)   
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件 
    SPI2_ReadWriteByte(W25X_WriteEnable);      // 发送写使能  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选   	      
}

// W25QXX写禁止
void W25QXX_Write_Disable(void)   
{  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件   
    SPI2_ReadWriteByte(W25X_WriteDisable);     // 发送写禁止指令    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选    	      
} 		

// 读取芯片ID
uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;	  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件	
    SPI2_ReadWriteByte(0x90);  // 发送读取ID命令	     
    SPI2_ReadWriteByte(0x00); 	    
    SPI2_ReadWriteByte(0x00); 	    
    SPI2_ReadWriteByte(0x00); 	  			   

    Temp |= SPI2_ReadWriteByte(0xFF) << 8;  
    Temp |= SPI2_ReadWriteByte(0xFF);	 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);	 // 取消片选				    
    return Temp;
}

// 读取SPI FLASH  
void W25QXX_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)   
{ 
    uint16_t i;   										    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件   
    SPI2_ReadWriteByte(W25X_ReadData);         // 发送读取命令   
    SPI2_ReadWriteByte((uint8_t)((ReadAddr) >> 16));  // 发送24bit地址    
    SPI2_ReadWriteByte((uint8_t)((ReadAddr) >> 8));   
    SPI2_ReadWriteByte((uint8_t)ReadAddr);   
    for (i = 0; i < NumByteToRead; i++) { 
        pBuffer[i] = SPI2_ReadWriteByte(0XFF);   // 循环读数  
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选   	      
}  

// SPI在一页(0~65535)内写入少于256个字节的数据
void W25QXX_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;  
    W25QXX_Write_Enable();                  // SET WEL 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件 
    SPI2_ReadWriteByte(W25X_PageProgram);      // 发送写页命令   
    SPI2_ReadWriteByte((uint8_t)((WriteAddr) >> 16));  // 发送24bit地址    
    SPI2_ReadWriteByte((uint8_t)((WriteAddr) >> 8));   
    SPI2_ReadWriteByte((uint8_t)WriteAddr);   
    for (i = 0; i < NumByteToWrite; i++) SPI2_ReadWriteByte(pBuffer[i]);  // 循环写数  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选
    W25QXX_Wait_Busy();  // 等待写入结束
}

// 无检验写SPI FLASH 
void W25QXX_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    uint16_t pageremain;
    pageremain = 256 - WriteAddr % 256; // 单页剩余的字节数
    if (NumByteToWrite <= pageremain) pageremain = NumByteToWrite; // 不大于256字节

    while (1) {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        if (NumByteToWrite == pageremain) break; // 写入结束
        else {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;
            pageremain = (NumByteToWrite > 256) ? 256 : NumByteToWrite;
        }
    }
}

// 写SPI FLASH  
void W25QXX_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t* W25QXX_BUF;

    // 检查是否超出 Flash 总容量
    if ((WriteAddr + NumByteToWrite) > W25QXX_TOTAL_SIZE) {
        return; // 地址越界
    }

    // 申请内存并检查分配结果
    W25QXX_BUF = mymalloc(SRAMIN, 4096);
    if (W25QXX_BUF == NULL) {
        return; // 内存分配失败
    }

    secpos = WriteAddr / 4096;       // 扇区地址
    secoff = WriteAddr % 4096;       // 在扇区内的偏移
    secremain = 4096 - secoff;       // 扇区剩余空间大小
    if (NumByteToWrite <= secremain) secremain = NumByteToWrite; // 不大于4096字节

    while (1) {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); // 读出整个扇区的内容
        for (i = 0; i < secremain; i++) {
            if (W25QXX_BUF[secoff + i] != 0xFF) break; // 需要擦除
        }
        if (i < secremain) {
            W25QXX_Erase_Sector(secpos); // 擦除这个扇区
            for (i = 0; i < secremain; i++) {
                W25QXX_BUF[i + secoff] = pBuffer[i]; // 更新数据
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); // 写入整个扇区
        } else {
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); // 写已经擦除过的数据
        }

        if (NumByteToWrite == secremain) break; // 写入结束
        else {
            secpos++;          // 下一个扇区
            secoff = 0;        // 偏移为0
            pBuffer += secremain;
            WriteAddr += secremain;
            NumByteToWrite -= secremain;
            secremain = (NumByteToWrite > 4096) ? 4096 : NumByteToWrite;
        }
    }

    myfree(SRAMIN, W25QXX_BUF); // 释放内存
}

// 擦除整个芯片
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                  // SET WEL 
    W25QXX_Wait_Busy();   
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件  
    SPI2_ReadWriteByte(W25X_ChipErase);        // 发送片擦除命令  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选   	      
    W25QXX_Wait_Busy();   				   // 等待芯片擦除结束
}   

// 擦除一个扇区
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
//    printf("fe:%x\r\n", Dst_Addr);	  
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();                  // SET WEL 	 
    W25QXX_Wait_Busy();   
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件   
    SPI2_ReadWriteByte(W25X_SectorErase);      // 发送扇区擦除指令 
    SPI2_ReadWriteByte((uint8_t)((Dst_Addr) >> 16));  // 发送24bit地址    
    SPI2_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));   
    SPI2_ReadWriteByte((uint8_t)Dst_Addr);  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选    	      
    W25QXX_Wait_Busy();   				   // 等待擦除完成
}  

// 等待空闲
void W25QXX_Wait_Busy(void)   
{   
    while((W25QXX_ReadSR() & 0x01) == 0x01);   // 等待BUSY位清空
}  

// 进入掉电模式
void W25QXX_PowerDown(void)   
{ 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件  
    SPI2_ReadWriteByte(W25X_PowerDown);        // 发送掉电命令  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选   	      
    HAL_Delay(1);                               // 等待TPD  
}   

// 唤醒
void W25QXX_WAKEUP(void)   
{  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // 使能器件  
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);   // 发送W25X_ReleasePowerDown命令    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // 取消片选   	      
    HAL_Delay(1);                               // 等待TRES1
}
