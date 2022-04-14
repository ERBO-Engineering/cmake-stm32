#include "stm32f411xe.h"

void delay(uint32_t time);
void setupSystem(void);
void setupGPIO(void);

int main(void)
{
  setupSystem();
  setupGPIO();

  while (1)
  {
    GPIOD->BSRR |= GPIO_BSRR_BS_12;
    delay(1000000);
    GPIOD->BSRR |= GPIO_BSRR_BR_12;
    delay(1000000);
  }
}

void setupSystem(void)
{

#define PLL_M 4
#define PLL_N 72
#define PLL_P 1 // PLLP = 2
#define PLL_Q 4

  // setup HSE
  RCC->CR |= RCC_CR_HSEON;

  // wait until ready
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  // set the power enable clock and voltage regulator
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_VOS;

  // setup flash and latency
  FLASH->CR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS;

  // setup prescalars
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  // PLCK1
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

  // setup pll
  RCC->PLLCFGR = (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | RCC_PLLCFGR_PLLSRC_HSE;

  // ENABLE PLL
  RCC->CR |= RCC_CR_PLLON;

  // Wait for PLL to be ready
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  // Set clock source and wait for ready
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
  {
    __NOP();
  }
}

void setupGPIO(void)
{
  // ENABLE bank D
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  // Set output for 15 pd15 blue led
  GPIOD->MODER |= GPIO_MODER_MODE15_0;
  GPIOD->MODER |= GPIO_MODER_MODE12_0;
  GPIOD->OTYPER = 0;
  GPIOD->OSPEEDR = 0;
}

void delay(uint32_t time)
{
  while (time--)
  {
    __NOP();
  }
}