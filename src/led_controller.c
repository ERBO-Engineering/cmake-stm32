#include "led_controller.h"
#include <stm32f411xe.h>
#include <stm32f4xx_hal_gpio.h>

#ifndef LED_COUNT
#error LED_COUNT not defined, please define LED_COUNT to match your pin mapping
#endif

// m_pinMapping has to be defined elsewhere like main.c
extern struct LedPinMapping m_pinMapping[LED_COUNT];

GPIO_TypeDef *_get_port_from_mapping(uint16_t pin);

int LED_CONTROLLER_set_led(uint16_t pin, GPIO_PinState state) {
  GPIO_TypeDef *port = _get_port_from_mapping(pin);
  if (NULL == port) {
    return -1;
  }

  HAL_GPIO_WritePin(port, pin, state);

  return 0;
}

int LED_CONTROLLER_turn_on(uint16_t pin) {
  return LED_CONTROLLER_set_led(pin, GPIO_PIN_SET);
}

int LED_CONTROLLER_turn_off(uint16_t pin) {
  return LED_CONTROLLER_set_led(pin, GPIO_PIN_RESET);
}

/**
 * @brief get the GPIO port belonging to the given pin
 *
 * @param pin
 * @return GPIO_TypeDef*
 */
GPIO_TypeDef *_get_port_from_mapping(uint16_t pin) {
  for (uint32_t i = 0; i < LED_COUNT; i++) {
    if (m_pinMapping[i].pin == pin) {
      return m_pinMapping[i].port;
    }
  }
  return NULL;
}
