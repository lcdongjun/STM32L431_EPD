#ifndef __BL_H__
#define __BL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


uint8_t IsUserProgramValid(void);
void JumpToUserApplication(void);

#ifdef __cplusplus
}
#endif

#endif /* __BL_H__ */

