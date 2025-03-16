
#ifndef _EPD_4IN2_V2_H_
#define _EPD_4IN2_V2_H_

#include "DEV_Config.h"

// Display resolution
extern uint16_t EPD_4IN2_V2_WIDTH;
extern uint16_t EPD_4IN2_V2_HEIGHT;

#define Seconds_1_5S      0
#define Seconds_1S        1

void EPD_4IN2_V2_ReadBusy(void);
void EPD_4IN2_V2_SendData(UBYTE Data);
void EPD_4IN2_V2_TurnOnDisplay(void);
void EPD_4IN2_V2_TurnOnDisplay_Fast(void);
void EPD_4IN2_V2_TurnOnDisplay_Partial(void);
void EPD_4IN2_V2_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
void EPD_4IN2_V2_SetCursor(UWORD Xstart, UWORD Ystart);
void EPD_4IN2_V2_Init(void);
void EPD_4IN2_V2_Init_Fast(UBYTE Mode);
void EPD_4IN2_V2_Clear(void);
void EPD_4IN2_V2_Display(UBYTE *Image);
void EPD_4IN2_V2_Display_Fast(UBYTE *Image);
void EPD_4IN2_V2_PartialDisplay(UBYTE *Image, UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
void EPD_4IN2_V2_Sleep(void);
void EPD_4IN2_V2_Set_DisplWindows(UWORD Xstart, UWORD Ystart,UWORD Xend, UWORD Yend);


#endif
