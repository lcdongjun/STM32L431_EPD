#include "Button.h"
#include "cmsis_os.h"

ButtonData_t buttonData;



// ���ڰ�������ʱ��ľ�̬����
static uint32_t pressTimeLeft = 0, pressTimeRight = 0, pressTimeMiddle = 0;

// ��ť״̬����ͨ�ú���
static void updateButtonState(GPIO_PinState pinState, uint32_t *pressTime, ButtonID_t buttonId, ButtonData_t *buttonData)
{
    if (pinState == GPIO_PIN_RESET) {  // ��ť����
        (*pressTime)++;
        if (*pressTime > BUTTON_LONG_PRESS_TIME) {
            // ����
            if (buttonData->button_id != buttonId || buttonData->state != BUTTON_STATE_LONG_PRESS) {
                buttonData->button_id = buttonId;
                buttonData->state = BUTTON_STATE_LONG_PRESS;
            }
        }
    } else {  // ��ť�ͷ�
        if (*pressTime > BUTTON_SHORT_PRESS_TIME && *pressTime < BUTTON_LONG_PRESS_TIME) {
            // �̰�
            if (buttonData->button_id != buttonId || buttonData->state != BUTTON_STATE_LONG_PRESS) {
                buttonData->button_id = buttonId;
                buttonData->state = BUTTON_STATE_SHORT_PRESS;
            }
        }
        *pressTime = 0;  // ���ü�����
    }
		
		vTaskDelay(pdMS_TO_TICKS(10));
}

// ɨ�谴ť״̬
ButtonData_t scanButton(void) 
{
    ButtonData_t buttonData = {NO_BUTTON, BUTTON_STATE_NO_PRESS};  // ��ʼΪ�ް������ް�ѹ״̬

    GPIO_PinState leftState = HAL_GPIO_ReadPin(KEY_L_GPIO_Port, KEY_L_Pin);
    GPIO_PinState rightState = HAL_GPIO_ReadPin(KEY_R_GPIO_Port, KEY_R_Pin);
    GPIO_PinState middleState = HAL_GPIO_ReadPin(KEY_OK_GPIO_Port, KEY_OK_Pin);

    // ���°�ť״̬
    updateButtonState(leftState, &pressTimeLeft, BUTTON_LEFT, &buttonData);
    updateButtonState(rightState, &pressTimeRight, BUTTON_RIGHT, &buttonData);
    updateButtonState(middleState, &pressTimeMiddle, BUTTON_MIDDLE, &buttonData);

    // ���ذ�ť����
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
						HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_SET);
						vTaskDelay(pdMS_TO_TICKS(100));
						HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET);
					
				} else if (buttonData.state == BUTTON_STATE_LONG_PRESS) {
					
						printf("Button %d long pressed\n", buttonData.button_id);
						HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_SET);
						vTaskDelay(pdMS_TO_TICKS(20));
						HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET);
				}
		}
	}
}
