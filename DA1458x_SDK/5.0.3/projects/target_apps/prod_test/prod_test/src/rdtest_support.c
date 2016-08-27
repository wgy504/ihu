#include "rdtest_support.h" 

//core clock is XTAL16 and HCLK_DIV=0. so clock Cortex-M0 is 16Mhz
//so 1 clock cycle lasts 1/16 usec = 0.0625 usec
void delay_systick_us(unsigned long int delay) //delay in units of 1 usec
{
	delay *= 16;	        // see explanation above this function	

	if (delay>0xFFFFFF)
	{
		delay = 0xFFFFFF; 		// limit, max 24 bits
	}
	SysTick->CTRL = 0;	   	// Disable SysTick
	SysTick->LOAD = delay; 	// Count down from delay eg. 999 to 0
	SysTick->VAL  = 0;	   	// Clear current value to 0
	SysTick->CTRL = 0x5;   	// Enable SysTick, disable SysTick exception and use processor clock
	while ((SysTick->CTRL & 0x10000) == 0);
}
