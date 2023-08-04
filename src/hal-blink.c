#include "stm32f4xx_hal.h"



extern void blink(){
	GPIO_InitTypeDef GPIOD_Params; // Initilisation structure for GPIOD Settings
	
	uint32_t i; // Loop variable
	
	__HAL_RCC_GPIOD_CLK_ENABLE(); // Turn on Clock of GPIOD
	
	// Configure the GPIO Pins 12, 13, 14 and 15 used for LEDs
	GPIOD_Params.Pin = GPIO_PIN_14 | GPIO_PIN_15; // Select pins 12 to 15
	GPIOD_Params.Mode = GPIO_MODE_OUTPUT_PP; // Set pins to push pull output mode
	GPIOD_Params.Speed = GPIO_SPEED_LOW; // Set low output speed
	HAL_GPIO_Init(GPIOD, &GPIOD_Params); // Initialise GPIOD according to parameters on GPIOD_Params
	
	while(1){
		
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // Turn on LEDs
		
		for(i = 0; i < 2000000; i++){}; // Implement a 1 second (ish!) delay
		
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); // Turn off LEDs
			
		for(i = 0; i < 2000000; i++){}; // Implement a 1 second (ish!) delay
		
	}
}