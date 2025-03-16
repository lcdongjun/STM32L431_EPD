#ifndef __AHT20_H
#define __AHT20_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include "cmsis_os.h"

#define AHT20_I2C_ADDRESS        0x38

typedef struct {
    float humidity;
    float temperature;
} AHT20_Data;

bool AHT20_Init(void);
bool AHT20_ReadData(AHT20_Data *data);

#ifdef __cplusplus
}
#endif

#endif /* __AHT20_H */

