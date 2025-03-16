#include "EPD_Command.h"
#include "Debug.h"
#include "sys.h"

uint16_t EPD_4IN2_V2_WIDTH = 400;   // 初始化宽度
uint16_t EPD_4IN2_V2_HEIGHT = 300;  // 初始化高度
		
		
const unsigned char LUT_ALL[233]={							
0x01,	0x0A,	0x1B,	0x0F,	0x03,	0x01,	0x01,	
0x05,	0x0A,	0x01,	0x0A,	0x01,	0x01,	0x01,	
0x05,	0x08,	0x03,	0x02,	0x04,	0x01,	0x01,	
0x01,	0x04,	0x04,	0x02,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x0A,	0x1B,	0x0F,	0x03,	0x01,	0x01,	
0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
0x05,	0x48,	0x03,	0x82,	0x84,	0x01,	0x01,	
0x01,	0x84,	0x84,	0x82,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x0A,	0x1B,	0x8F,	0x03,	0x01,	0x01,	
0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
0x05,	0x48,	0x83,	0x82,	0x04,	0x01,	0x01,	
0x01,	0x04,	0x04,	0x02,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x8A,	0x1B,	0x8F,	0x03,	0x01,	0x01,	
0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
0x05,	0x48,	0x83,	0x02,	0x04,	0x01,	0x01,	
0x01,	0x04,	0x04,	0x02,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x8A,	0x9B,	0x8F,	0x03,	0x01,	0x01,	
0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
0x05,	0x48,	0x03,	0x42,	0x04,	0x01,	0x01,	
0x01,	0x04,	0x04,	0x42,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
0x02,	0x00,	0x00,	0x07,	0x17,	0x41,	0xA8,	
0x32,	0x30,						
};		

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_4IN2_V2_Reset(void)
{
	HAL_GPIO_WritePin(EPD_RES_GPIO_Port,EPD_RES_Pin,GPIO_PIN_SET);
	    DEV_Delay_ms(100);
	HAL_GPIO_WritePin(EPD_RES_GPIO_Port,EPD_RES_Pin,GPIO_PIN_RESET);
	    DEV_Delay_ms(100);
	HAL_GPIO_WritePin(EPD_RES_GPIO_Port,EPD_RES_Pin,GPIO_PIN_SET);
	    DEV_Delay_ms(100);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_4IN2_V2_SendCommand(UBYTE Reg)
{

	
		HAL_GPIO_WritePin(EPD_DC_GPIO_Port,EPD_DC_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EPD_CS_GPIO_Port,EPD_CS_Pin,GPIO_PIN_RESET);
		DEV_SPI_WriteByte(Reg);
		HAL_GPIO_WritePin(EPD_CS_GPIO_Port,EPD_CS_Pin,GPIO_PIN_SET);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
void EPD_4IN2_V2_SendData(UBYTE Data)
{

	
		HAL_GPIO_WritePin(EPD_DC_GPIO_Port,EPD_DC_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(EPD_CS_GPIO_Port,EPD_CS_Pin,GPIO_PIN_RESET);
		DEV_SPI_WriteByte(Data);
		HAL_GPIO_WritePin(EPD_CS_GPIO_Port,EPD_RES_Pin,GPIO_PIN_SET);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_4IN2_V2_ReadBusy(void)
{
    Debug("e-Paper busy\r\n");
    while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 1) {      //LOW: idle, HIGH: busy
        DEV_Delay_ms(10);
    }
    Debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void EPD_4IN2_V2_TurnOnDisplay(void)
{
    EPD_4IN2_V2_SendCommand(0x22);
		EPD_4IN2_V2_SendData(0xF7);
    EPD_4IN2_V2_SendCommand(0x20);
    EPD_4IN2_V2_ReadBusy();
}

void EPD_4IN2_V2_TurnOnDisplay_Fast(void)
{
    EPD_4IN2_V2_SendCommand(0x22);
		EPD_4IN2_V2_SendData(0xC7);
    EPD_4IN2_V2_SendCommand(0x20);
    EPD_4IN2_V2_ReadBusy();
}

void EPD_4IN2_V2_TurnOnDisplay_Partial(void)
{
    EPD_4IN2_V2_SendCommand(0x22);
		EPD_4IN2_V2_SendData(0xFF);
    EPD_4IN2_V2_SendCommand(0x20);
    EPD_4IN2_V2_ReadBusy();
}


/******************************************************************************
function :	Setting the display window
parameter:
******************************************************************************/
void EPD_4IN2_V2_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_4IN2_V2_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_4IN2_V2_SendData((Xstart>>3) & 0xFF);
    EPD_4IN2_V2_SendData((Xend>>3) & 0xFF);
	
    EPD_4IN2_V2_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_4IN2_V2_SendData(Ystart & 0xFF);
    EPD_4IN2_V2_SendData((Ystart >> 8) & 0xFF);
    EPD_4IN2_V2_SendData(Yend & 0xFF);
    EPD_4IN2_V2_SendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
******************************************************************************/
void EPD_4IN2_V2_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_4IN2_V2_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_4IN2_V2_SendData((Xstart>>3) & 0xFF);

    EPD_4IN2_V2_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_4IN2_V2_SendData(Ystart & 0xFF);
    EPD_4IN2_V2_SendData((Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_4IN2_V2_Init(void)
{
    EPD_4IN2_V2_Reset();

    EPD_4IN2_V2_ReadBusy();
    EPD_4IN2_V2_SendCommand(0x12);   // soft  reset
    EPD_4IN2_V2_ReadBusy();
		EPD_4IN2_V2_SendCommand(0xFF);
    EPD_4IN2_V2_SendCommand(0x21); //  Display update control
    EPD_4IN2_V2_SendData(0x40);		
    EPD_4IN2_V2_SendData(0x00);		

    EPD_4IN2_V2_SendCommand(0x3C); //BorderWavefrom
    EPD_4IN2_V2_SendData(0x05);
	
    EPD_4IN2_V2_SendCommand(0x11);	// data  entry  mode
    EPD_4IN2_V2_SendData(0x03);		// X-mode   
		
	EPD_4IN2_V2_SetWindows(0, 0, EPD_4IN2_V2_WIDTH-1, EPD_4IN2_V2_HEIGHT-1);
	 
	EPD_4IN2_V2_SetCursor(0, 0);
	
    EPD_4IN2_V2_ReadBusy();
		
}

/******************************************************************************
function :	Initialize Fast the e-Paper register
parameter:
******************************************************************************/
void EPD_4IN2_V2_Init_Fast(UBYTE Mode)
{
    EPD_4IN2_V2_Reset();

    EPD_4IN2_V2_ReadBusy();   
    EPD_4IN2_V2_SendCommand(0x12);   // soft  reset
    EPD_4IN2_V2_ReadBusy();

    EPD_4IN2_V2_SendCommand(0x21);	 
    EPD_4IN2_V2_SendData(0x40);
    EPD_4IN2_V2_SendData(0x00); 

    EPD_4IN2_V2_SendCommand(0x3C);     
    EPD_4IN2_V2_SendData(0x05);  

    if(Mode == Seconds_1_5S)
    {
        //1.5s
        EPD_4IN2_V2_SendCommand(0x1A); // Write to temperature register
        EPD_4IN2_V2_SendData(0x6E);		
    }
    else if(Mode == Seconds_1S)
    {
        //1s
        EPD_4IN2_V2_SendCommand(0x1A); // Write to temperature register
        EPD_4IN2_V2_SendData(0x5A);	
    }
                 
    EPD_4IN2_V2_SendCommand(0x22); // Load temperature value
    EPD_4IN2_V2_SendData(0x91);		
    EPD_4IN2_V2_SendCommand(0x20);	
    EPD_4IN2_V2_ReadBusy();   
	
    EPD_4IN2_V2_SendCommand(0x11);	// data  entry  mode
    EPD_4IN2_V2_SendData(0x03);		// X-mode   
		
	EPD_4IN2_V2_SetWindows(0, 0, EPD_4IN2_V2_WIDTH-1, EPD_4IN2_V2_HEIGHT-1);
	 
	EPD_4IN2_V2_SetCursor(0, 0);
	
    EPD_4IN2_V2_ReadBusy();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_4IN2_V2_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_4IN2_V2_WIDTH % 8 == 0)? (EPD_4IN2_V2_WIDTH / 8 ): (EPD_4IN2_V2_WIDTH / 8 + 1);
    Height = EPD_4IN2_V2_HEIGHT;

    EPD_4IN2_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_V2_SendData(0xFF);
        }
    }

    EPD_4IN2_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_V2_SendData(0xFF);
        }
    }
    EPD_4IN2_V2_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_4IN2_V2_Display(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_4IN2_V2_WIDTH % 8 == 0)? (EPD_4IN2_V2_WIDTH / 8 ): (EPD_4IN2_V2_WIDTH / 8 + 1);
    Height = EPD_4IN2_V2_HEIGHT;

    EPD_4IN2_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_V2_SendData(Image[i + j * Width]);
        }
    }

    EPD_4IN2_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_V2_SendData(Image[i + j * Width]);
        }
    }
    EPD_4IN2_V2_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and fast displays
parameter:
******************************************************************************/
void EPD_4IN2_V2_Display_Fast(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_4IN2_V2_WIDTH % 8 == 0)? (EPD_4IN2_V2_WIDTH / 8 ): (EPD_4IN2_V2_WIDTH / 8 + 1);
    Height = EPD_4IN2_V2_HEIGHT;

    EPD_4IN2_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_V2_SendData(Image[i + j * Width]);
        }
    }

    EPD_4IN2_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_V2_SendData(Image[i + j * Width]);
        }
    }
    EPD_4IN2_V2_TurnOnDisplay_Fast();
}

// Send partial data for partial refresh
void EPD_4IN2_V2_PartialDisplay(UBYTE *Image, UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    if((Xstart % 8 + Xend % 8 == 8 && Xstart % 8 > Xend % 8) || Xstart % 8 + Xend % 8 == 0 || (Xend - Xstart)%8 == 0)
    {
        Xstart = Xstart / 8 ;
        Xend = Xend / 8;
    }
    else
    {
        Xstart = Xstart / 8 ;
        Xend = Xend % 8 == 0 ? Xend / 8 : Xend / 8 + 1;
    }
    
		
	UWORD  Width;
	Width = Xend -  Xstart;
	UWORD IMAGE_COUNTER = Width * (Yend-Ystart);
	
	Xend -= 1;
	Yend -= 1;	

	EPD_4IN2_V2_SendCommand(0x91); 
	EPD_4IN2_V2_SendData(0x00);
	EPD_4IN2_V2_SendData(0x00);

	EPD_4IN2_V2_SendCommand(0x3C); 
	EPD_4IN2_V2_SendData(0x80); 

    EPD_4IN2_V2_SendCommand(0x11);	// data  entry  mode
    EPD_4IN2_V2_SendData(0x03);		// X-mode  

    EPD_4IN2_V2_SendCommand(0x44);       // set RAM x address start/end, in page 35
    EPD_4IN2_V2_SendData(Xstart & 0xff);    // RAM x address start at 00h;
    EPD_4IN2_V2_SendData(Xend & 0xff);    // RAM x address end at 0fh(15+1)*8->128 
    EPD_4IN2_V2_SendCommand(0x45);       // set RAM y address start/end, in page 35
    EPD_4IN2_V2_SendData(Ystart & 0xff);    // RAM y address start at 0127h;
    EPD_4IN2_V2_SendData((Ystart>>8) & 0x01);    // RAM y address start at 0127h;
    EPD_4IN2_V2_SendData(Yend & 0xff);    // RAM y address end at 00h;
    EPD_4IN2_V2_SendData((Yend>>8) & 0x01); 

    EPD_4IN2_V2_SendCommand(0x4E);   // set RAM x address count to 0;
    EPD_4IN2_V2_SendData(Xstart & 0xff); 
    EPD_4IN2_V2_SendCommand(0x4F);   // set RAM y address count to 0X127;    
    EPD_4IN2_V2_SendData(Ystart & 0xff);
    EPD_4IN2_V2_SendData((Ystart>>8) & 0x01);

    EPD_4IN2_V2_ReadBusy();

    
	
    EPD_4IN2_V2_SendCommand(0x24);
    for (UWORD j = 0; j < IMAGE_COUNTER; j++) {
            EPD_4IN2_V2_SendData(Image[j]);
    }

	EPD_4IN2_V2_TurnOnDisplay_Partial();
}
/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_4IN2_V2_Sleep(void)
{
    EPD_4IN2_V2_SendCommand(0x10); // DEEP_SLEEP
    EPD_4IN2_V2_SendData(0x01);
	DEV_Delay_ms(200);
}


void EPD_4IN2_V2_Set_DisplWindows(UWORD Xstart, UWORD Ystart,UWORD Xend, UWORD Yend)
{
	EPD_4IN2_V2_SetWindows(Xstart, Ystart, Xend-1, Yend-1);
	EPD_4IN2_V2_SetCursor(Xstart, Ystart);
	EPD_4IN2_V2_ReadBusy();
}


