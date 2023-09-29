#include "led_controller.h"
#include <stm32f411xe.h>
#include <stm32f4xx_hal_gpio.h>

#ifndef LED_COUNT
#error LED_COUNT not defined, please define LED_COUNT to match your pin mapping
#endif

// m_pinMapping has to be defined elsewhere like main.c
static GPIO_TypeDef *_get_port_from_mapping(struct LedPinMapping *pinMapping,
                                            int ledCount, uint16_t pin);

int LED_CONTROLLER_set_led(struct LedPinMapping *pinMapping, int ledCount,
                           uint16_t pin, GPIO_PinState state) {
  GPIO_TypeDef *port = _get_port_from_mapping(pinMapping, ledCount, pin);
  if (NULL == port) {
    return -1;
  }

  HAL_GPIO_WritePin(port, pin, state);
  return 0;
}

int LED_CONTROLLER_turn_on(struct LedPinMapping *pinMapping, int ledCount,
                           uint16_t pin) {
  return LED_CONTROLLER_set_led(pinMapping, ledCount, pin, GPIO_PIN_SET);
}

int LED_CONTROLLER_turn_off(struct LedPinMapping *pinMapping, int ledCount,
                            uint16_t pin) {
  return LED_CONTROLLER_set_led(pinMapping, ledCount, pin, GPIO_PIN_RESET);
}

/**
 * @brief get the GPIO port belonging to the given pin
 *
 * @param pin
 * @return GPIO_TypeDef*
 */
static GPIO_TypeDef *_get_port_from_mapping(struct LedPinMapping *pinMapping,
                                            int ledCount, uint16_t pin) {
  for (uint32_t i = 0; i < ledCount; i++) {
    if (pinMapping[i].pin == pin) {
      return pinMapping[i].port;
    }
  }
  return NULL;
}
