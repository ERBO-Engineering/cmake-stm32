#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdint.h>
#include <stm32f411xe.h>
#include <stm32f4xx_hal_gpio.h>

/**
 * @brief struct that allows combining the GPIO bank with a pin for easy lookup
 *
 */
struct LedPinMapping {
  uint16_t pin;
  GPIO_TypeDef *port;
};

/**
 * @brief allow setting LED state through HAL
 *  uses external variable m_pinMapping to map a pin to PORT
 *
 * @param pinMapping pointer to the pinMapping struct
 * @param ledCount amount of leds in the pinmap
 * @param pin pin you want to change the state of
 * @param state state you want to change to
 * @return int returns 0 or pass and -1 if pin mapping not found
 */
int LED_CONTROLLER_set_led(struct LedPinMapping *pinMapping, int ledCount,
                           uint16_t pin, GPIO_PinState state);

/**
 * @brief turn on a specific LED
 *
 * @param pinMapping pointer to the pinMapping struct
 * @param ledCount amount of leds in the pinmap
 * @param pin pin of the led you want set
 * @return int
 */
int LED_CONTROLLER_turn_on(struct LedPinMapping *pinMapping, int ledCount,
                           uint16_t pin);

/**
 * @brief turn on a specific LED
 *
 * @param pinMapping pointer to the pinMapping struct
 * @param ledCount amount of leds in the pinmap
 * @param pin pin of the led you want to reset
 * @return int
 */
int LED_CONTROLLER_turn_off(struct LedPinMapping *pinMapping, int ledCount,
                            uint16_t pin);

#endif // !LED_CONTROLLER_H