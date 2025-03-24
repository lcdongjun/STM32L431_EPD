#ifndef __BL_H__
#define __BL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

uint8_t IsUserProgramValid(void);
void JumpToUserApplication(void);
uint8_t CheckBootFlag(void);
HAL_StatusTypeDef Flash_Erase_By_Size(uint32_t at_write_addr, uint32_t at_write_size);
HAL_StatusTypeDef Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

#ifdef __cplusplus
}
#endif

#endif /* __BL_H__ */

