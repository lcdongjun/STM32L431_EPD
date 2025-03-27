#include "delay.h"
 
/**********************************************
//Delay_us
**********************************************/

void DWT_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }

    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

void Delay_us(unsigned long us) {
    volatile uint32_t* cyccnt = &DWT->CYCCNT;
    uint32_t start = *cyccnt;
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    while ((*cyccnt - start) < cycles);
}
