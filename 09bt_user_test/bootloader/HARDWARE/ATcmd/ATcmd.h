#ifndef __ATCMD_H
#define __ATCMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
extern uint8_t AT_FlashWrite_Flag ;
extern uint32_t at_write_addr;
extern uint32_t at_write_size;

void AT_Command_Parser(char *input);


#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */

