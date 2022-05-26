#include "stm32f4xx.h"

extern void blink(){
// Loop Variables
	uint32_t i;
	
	// Configue LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable the clock of port D of the GPIO
	
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Green LED, set pin 12 as output
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Orange LED, set pin 13 as output
	
	while(1){
		// Turn on LEDs
		GPIOD->BSRR = 1<<12; // Set the BSRR bit 12 to 1 to turn respective LED on
		GPIOD->BSRR = 1<<13; // Set the BSRR bit 13 to 1 to turn respective LED on
		
		// Delay
		for(i = 0; i < 200000; i++){}; // Loop repeats 2,000,000 implementing a delay
			
		// Turn off LEDs
		GPIOD->BSRR = 1<<(12+16); // Set the BSRR bit 12 + 16 to 1 to turn respective LED off
		GPIOD->BSRR = 1<<(13+16); // Set the BSRR bit 13 + 16 to 1 to turn respective LED off
				
		// Delay
		for(i = 0; i < 200000; i++){}; // Loop repeats 2,000,000 implementing a delay
	}
}