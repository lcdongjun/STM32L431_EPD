#include "Button.h"
#include "cmsis_os.h"
ButtonData_t buttonData;

ButtonData_t scanButton(void) {
    ButtonData_t buttonData = {NO_BUTTON, BUTTON_STATE_NO_PRESS};

    static uint32_t pressTimeLeft = 0, pressTimeRight = 0, pressTimeMiddle = 0;

		GPIO_PinState leftState = HAL_GPIO_ReadPin(KEY_L_GPIO_Port, KEY_L_Pin);
    GPIO_PinState rightState = HAL_GPIO_ReadPin(KEY_R_GPIO_Port, KEY_R_Pin);
    GPIO_PinState middleState = HAL_GPIO_ReadPin(KEY_OK_GPIO_Port, KEY_OK_Pin);

    if (leftState == GPIO_PIN_RESET) {
        pressTimeLeft++;
			if (pressTimeLeft > BUTTON_LONG_PRESS_TIME) {
            buttonData.button_id = BUTTON_LEFT;
            buttonData.state = BUTTON_STATE_LONG_PRESS;
        }
    } else {
        if (pressTimeLeft > 3 &&pressTimeLeft<BUTTON_LONG_PRESS_TIME && buttonData.state != BUTTON_STATE_LONG_PRESS) {
            buttonData.button_id = BUTTON_LEFT;
            buttonData.state = BUTTON_STATE_SHORT_PRESS;
        }
        pressTimeLeft = 0;
    }
    if (rightState == GPIO_PIN_RESET) {
        pressTimeRight++;
				if (pressTimeRight > BUTTON_LONG_PRESS_TIME) {
            buttonData.button_id = BUTTON_RIGHT;
            buttonData.state = BUTTON_STATE_LONG_PRESS;
        }
    } else {
        if (pressTimeRight > 3 &&pressTimeRight<BUTTON_LONG_PRESS_TIME && buttonData.state != BUTTON_STATE_LONG_PRESS) {
            buttonData.button_id = BUTTON_RIGHT;
            buttonData.state = BUTTON_STATE_SHORT_PRESS;
        }
        pressTimeRight = 0;
    }
    if (middleState == GPIO_PIN_RESET) {
        pressTimeMiddle++;
				if (pressTimeMiddle > BUTTON_LONG_PRESS_TIME) {
            buttonData.button_id = BUTTON_MIDDLE;
            buttonData.state = BUTTON_STATE_LONG_PRESS;
        }
				
    } else {
        if (pressTimeMiddle > 3 &&pressTimeMiddle<BUTTON_LONG_PRESS_TIME && buttonData.state != BUTTON_STATE_LONG_PRESS) {
            buttonData.button_id = BUTTON_MIDDLE;
            buttonData.state = BUTTON_STATE_SHORT_PRESS;
        }
        pressTimeMiddle = 0;
    }
		
		vTaskDelay(pdMS_TO_TICKS(5));
    return buttonData;
}

void ButtonTask(void)
{
	while(1)
	{
		buttonData = scanButton();
		if (buttonData.button_id != NO_BUTTON) { 
				if (buttonData.state == BUTTON_STATE_SHORT_PRESS) {
						printf("Button %d short pressed\n", buttonData.button_id);
					
				} else if (buttonData.state == BUTTON_STATE_LONG_PRESS) {
					
						printf("Button %d long pressed\n", buttonData.button_id);
				}
		}
	}
}
