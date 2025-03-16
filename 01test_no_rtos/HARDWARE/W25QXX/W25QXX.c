#include "w25qxx.h" 
#include "spi.h"   
#include "usart.h"	

uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData = 0;

    if (HAL_SPI_TransmitReceive(&hspi2, &TxData, &RxData, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        // ������
        return 0xFF;
    }

    return RxData;
}

uint16_t W25QXX_TYPE = W25Q128;	// Ĭ����W25Q128

// ��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);  // SPI FLASH��ѡ��
    W25QXX_TYPE = W25QXX_ReadID();  // ��ȡFLASH ID.
    printf("FLASH_ID: 0X%X\r\n", W25QXX_TYPE);
}

// ��ȡW25QXX��״̬�Ĵ���
uint8_t W25QXX_ReadSR(void)   
{  
    uint8_t byte = 0; 
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������   
    SPI2_ReadWriteByte(W25X_ReadStatusReg);    // ���Ͷ�ȡ״̬�Ĵ�������    
    byte = SPI2_ReadWriteByte(0Xff);             // ��ȡһ���ֽ�     
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ     
    return byte;   
} 

// дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(uint8_t sr)   
{   
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������   
    SPI2_ReadWriteByte(W25X_WriteStatusReg);   // ����дȡ״̬�Ĵ�������    
    SPI2_ReadWriteByte(sr);               // д��һ���ֽ�  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ    	      
}   

// W25QXXдʹ��
void W25QXX_Write_Enable(void)   
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������ 
    SPI2_ReadWriteByte(W25X_WriteEnable);      // ����дʹ��  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ   	      
}

// W25QXXд��ֹ
void W25QXX_Write_Disable(void)   
{  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������   
    SPI2_ReadWriteByte(W25X_WriteDisable);     // ����д��ָֹ��    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ    	      
} 		

// ��ȡоƬID
uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;	  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������	
    SPI2_ReadWriteByte(0x90);  // ���Ͷ�ȡID����	     
    SPI2_ReadWriteByte(0x00); 	    
    SPI2_ReadWriteByte(0x00); 	    
    SPI2_ReadWriteByte(0x00); 	  			   

    Temp |= SPI2_ReadWriteByte(0xFF) << 8;  
    Temp |= SPI2_ReadWriteByte(0xFF);	 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);	 // ȡ��Ƭѡ				    
    return Temp;
}

// ��ȡSPI FLASH  
void W25QXX_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)   
{ 
    uint16_t i;   										    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������   
    SPI2_ReadWriteByte(W25X_ReadData);         // ���Ͷ�ȡ����   
    SPI2_ReadWriteByte((uint8_t)((ReadAddr) >> 16));  // ����24bit��ַ    
    SPI2_ReadWriteByte((uint8_t)((ReadAddr) >> 8));   
    SPI2_ReadWriteByte((uint8_t)ReadAddr);   
    for (i = 0; i < NumByteToRead; i++) { 
        pBuffer[i] = SPI2_ReadWriteByte(0XFF);   // ѭ������  
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ   	      
}  

// SPI��һҳ(0~65535)��д������256���ֽڵ�����
void W25QXX_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;  
    W25QXX_Write_Enable();                  // SET WEL 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������ 
    SPI2_ReadWriteByte(W25X_PageProgram);      // ����дҳ����   
    SPI2_ReadWriteByte((uint8_t)((WriteAddr) >> 16));  // ����24bit��ַ    
    SPI2_ReadWriteByte((uint8_t)((WriteAddr) >> 8));   
    SPI2_ReadWriteByte((uint8_t)WriteAddr);   
    for (i = 0; i < NumByteToWrite; i++) SPI2_ReadWriteByte(pBuffer[i]);  // ѭ��д��  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ
    W25QXX_Wait_Busy();  // �ȴ�д�����
}

// �޼���дSPI FLASH 
void W25QXX_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    uint16_t pageremain;
    pageremain = 256 - WriteAddr % 256; // ��ҳʣ����ֽ���
    if (NumByteToWrite <= pageremain) pageremain = NumByteToWrite; // ������256�ֽ�

    while (1) {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        if (NumByteToWrite == pageremain) break; // д�����
        else {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;
            pageremain = (NumByteToWrite > 256) ? 256 : NumByteToWrite;
        }
    }
}

// дSPI FLASH  
void W25QXX_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t* W25QXX_BUF;

    // ����Ƿ񳬳� Flash ������
    if ((WriteAddr + NumByteToWrite) > W25QXX_TOTAL_SIZE) {
        return; // ��ַԽ��
    }

    // �����ڴ沢��������
    W25QXX_BUF = mymalloc(SRAMIN, 4096);
    if (W25QXX_BUF == NULL) {
        return; // �ڴ����ʧ��
    }

    secpos = WriteAddr / 4096;       // ������ַ
    secoff = WriteAddr % 4096;       // �������ڵ�ƫ��
    secremain = 4096 - secoff;       // ����ʣ��ռ��С
    if (NumByteToWrite <= secremain) secremain = NumByteToWrite; // ������4096�ֽ�

    while (1) {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); // ������������������
        for (i = 0; i < secremain; i++) {
            if (W25QXX_BUF[secoff + i] != 0xFF) break; // ��Ҫ����
        }
        if (i < secremain) {
            W25QXX_Erase_Sector(secpos); // �����������
            for (i = 0; i < secremain; i++) {
                W25QXX_BUF[i + secoff] = pBuffer[i]; // ��������
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); // д����������
        } else {
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); // д�Ѿ�������������
        }

        if (NumByteToWrite == secremain) break; // д�����
        else {
            secpos++;          // ��һ������
            secoff = 0;        // ƫ��Ϊ0
            pBuffer += secremain;
            WriteAddr += secremain;
            NumByteToWrite -= secremain;
            secremain = (NumByteToWrite > 4096) ? 4096 : NumByteToWrite;
        }
    }

    myfree(SRAMIN, W25QXX_BUF); // �ͷ��ڴ�
}

// ��������оƬ
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                  // SET WEL 
    W25QXX_Wait_Busy();   
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������  
    SPI2_ReadWriteByte(W25X_ChipErase);        // ����Ƭ��������  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ   	      
    W25QXX_Wait_Busy();   				   // �ȴ�оƬ��������
}   

// ����һ������
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
//    printf("fe:%x\r\n", Dst_Addr);	  
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();                  // SET WEL 	 
    W25QXX_Wait_Busy();   
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������   
    SPI2_ReadWriteByte(W25X_SectorErase);      // ������������ָ�� 
    SPI2_ReadWriteByte((uint8_t)((Dst_Addr) >> 16));  // ����24bit��ַ    
    SPI2_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));   
    SPI2_ReadWriteByte((uint8_t)Dst_Addr);  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ    	      
    W25QXX_Wait_Busy();   				   // �ȴ��������
}  

// �ȴ�����
void W25QXX_Wait_Busy(void)   
{   
    while((W25QXX_ReadSR() & 0x01) == 0x01);   // �ȴ�BUSYλ���
}  

// �������ģʽ
void W25QXX_PowerDown(void)   
{ 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������  
    SPI2_ReadWriteByte(W25X_PowerDown);        // ���͵�������  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ   	      
    HAL_Delay(1);                               // �ȴ�TPD  
}   

// ����
void W25QXX_WAKEUP(void)   
{  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // ʹ������  
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);   // ����W25X_ReleasePowerDown����    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // ȡ��Ƭѡ   	      
    HAL_Delay(1);                               // �ȴ�TRES1
}
