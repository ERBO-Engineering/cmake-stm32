#include "stm32f4xx.h"

#define PLL_M 4
#define PLL_N 180
#define PLL_P 0 // PLLP = 2

void SysConfig(void);
void GPIOConfig(void);

void delay(uint32_t time);

int main()
{

  SysConfig();
  GPIOConfig();

  while (1)
  {
    // now blink the led

    // delay(10);
    GPIOD->ODR |= (0xFF << 12); // write directly to the output register without the BSRR
    // GPIOD->BSRR |= (1 << 9); // SET PD12
    // GPIOD->BSRR |= (1<<13); // SET PD12
    // delay(10);
    // GPIOD->ODR =0 ; // write directly to the output register without the BSRR
    // GPIOD->BSRR |= ((1 << 9) << 16); // UNSET PD12
    // GPIOD->BSRR |= ((1<<13) <<16); // UNSET PD12
  }
}

void delay(uint32_t time)
{
  while (time--)
  {
    __NOP();
  }
}

// PD12 greend led
void GPIOConfig(void)
{

  RCC->AHB1ENR |= (1 << 3); // enable clock of GPIO bank D bit 3 RCC_AHB1ENR

  GPIOD->MODER |= (1 << 24); // set pin 12 as output (MODER12(01)) general purpose
  GPIOD->MODER |= (1 << 18); // set pin 9 as output (MODER12(01)) general purpose
  GPIOD->MODER |= (1 << 26); // set pin 13 as output (MODER12(01)) general purpose
  GPIOD->MODER |= (1 << 28); // set pin 14 as output (MODER12(01)) general purpose
  GPIOD->MODER |= (1 << 30); // set pin 15 as output (MODER12(01)) general purpose
  // now set output mode

  // speed mode is low by default
  GPIOD->OTYPER = 0;
  GPIOD->OSPEEDR = 1;
}

void SysConfig(void)
{

  // set rcc clock
  RCC->CR |= RCC_CR_HSEON;
  // wait till external clock is ready
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;
  RCC->APB1ENR |= RCC->APB1LPENR;

  // enable power regulator
  PWR->CR |= PWR_CR_VOS;

  // flash config
  FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  // APB1
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

  // PLL                                                     set clock src to HSE
  RCC->PLLCFGR = (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | (RCC_PLLCFGR_PLLSRC_HSE);

  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  // set clock source

  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}