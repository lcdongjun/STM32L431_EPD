#include "delay.h"



#define SYSCLK_MHZ    (SystemCoreClock/1000000) //ϵͳʱ��Ƶ��
 
/**********************************************
//Delay_us
**********************************************/
void  Delay_us(unsigned long nTim)
{
	uint32_t Tdata = nTim*SYSCLK_MHZ/5;
	for(uint32_t i=0;i<Tdata;i++)
	{
		__NOP();
	}
}

