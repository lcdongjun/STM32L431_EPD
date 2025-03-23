#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


#define BUTTON_LONG_PRESS_TIME 20
#define BUTTON_SHORT_PRESS_TIME 1

typedef enum {
		NO_BUTTON = 0,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_MIDDLE
} ButtonID_t;

typedef enum {
		BUTTON_STATE_NO_PRESS = 0,
    BUTTON_STATE_SHORT_PRESS,
    BUTTON_STATE_LONG_PRESS
} ButtonState_t;

typedef struct {
    ButtonID_t button_id;
    ButtonState_t state;
} ButtonData_t;

extern ButtonData_t buttonData;


ButtonData_t scanButton(void);
void ButtonTask(void);


#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */

