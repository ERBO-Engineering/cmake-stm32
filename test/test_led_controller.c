#include "string.h"
#include "unity.h"

// FILE UNDER TEST
#include "led_controller.h"

// MOCK HAL
#include "mock_stm32f4xx_hal_gpio.h"
#include <stm32f411xe.h>

void setUp(void) {
  // Set up any necessary initialization here.
}

void tearDown(void) {
  // Clean up after each test case.
}

void test_LED_CONTROLLER_set_led_with_valid_pin(void) {
  uint16_t pin = 5;
  GPIO_TypeDef *gpioDef = (GPIO_TypeDef *)10;
  struct LedPinMapping pinMapping[LED_COUNT] = {};

  pinMapping[0].port = gpioDef;
  pinMapping[0].pin = pin;

  // Mocking HAL_GPIO_WritePin
  HAL_GPIO_WritePin_Expect(gpioDef, pin, GPIO_PIN_SET);

  int result = LED_CONTROLLER_set_led(pinMapping, LED_COUNT, pin, GPIO_PIN_SET);

  // Assert that the function returns 0 for a valid pin.
  TEST_ASSERT_EQUAL(0, result);
}

void test_LED_CONTROLLER_set_led_with_invalid_pin(void) {
  uint16_t pin = 5;
  GPIO_TypeDef *gpioDef = (GPIO_TypeDef *)10;
  struct LedPinMapping pinMapping[LED_COUNT] = {};

  pinMapping[0].port = gpioDef;
  pinMapping[0].pin = 0; // invalid pin

  // we don't have to expect WritePin because it will never be called
  int result = LED_CONTROLLER_set_led(pinMapping, LED_COUNT, pin, GPIO_PIN_SET);

  // Assert that the function returns -1 for a invalid pin.
  TEST_ASSERT_EQUAL(-1, result);
}

void test_LED_CONTROLLER_set_led_turn_on(void) {
  uint16_t pin = 5;
  GPIO_TypeDef *gpioDef = (GPIO_TypeDef *)10;
  struct LedPinMapping pinMapping[LED_COUNT] = {};

  pinMapping[0].port = gpioDef;
  pinMapping[0].pin = pin;

  // Mocking HAL_GPIO_WritePin
  HAL_GPIO_WritePin_Expect(gpioDef, pin, GPIO_PIN_SET);

  int result = LED_CONTROLLER_turn_on(pinMapping, LED_COUNT, pin);

  // Assert that the function returns 0 for a valid pin.
  TEST_ASSERT_EQUAL(0, result);
}

void test_LED_CONTROLLER_set_led_turn_off(void) {
  uint16_t pin = 5;
  GPIO_TypeDef *gpioDef = (GPIO_TypeDef *)10;
  struct LedPinMapping pinMapping[LED_COUNT] = {};

  pinMapping[0].port = gpioDef;
  pinMapping[0].pin = pin;

  // Mocking HAL_GPIO_WritePin
  HAL_GPIO_WritePin_Expect(gpioDef, pin, GPIO_PIN_RESET);

  int result = LED_CONTROLLER_turn_off(pinMapping, LED_COUNT, pin);

  // Assert that the function returns 0 for a valid pin.
  TEST_ASSERT_EQUAL(0, result);
}