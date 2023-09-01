#include "string.h"
#include "unity.h"

// FILE UNDER TEST
#include "led_controller.h"

// MOCK HAL
#include "mock_stm32f4xx_hal_gpio.h"

struct LedPinMapping m_pinMapping[LED_COUNT] = {};

void setUp(void) {
  // Set up any necessary initialization here.
  memset(m_pinMapping, 0, sizeof(m_pinMapping) / sizeof(m_pinMapping[0]));
}

void tearDown(void) {
  // Clean up after each test case.
}

void test_LED_CONTROLLER_set_led_with_valid_pin(void) {
  uint16_t pin = 5;

  m_pinMapping[0].port = (void *)10;
  m_pinMapping[0].pin = pin; // create a valid pin

  // Mocking HAL_GPIO_WritePin
  HAL_GPIO_WritePin_ExpectAnyArgs();

  int result = LED_CONTROLLER_set_led(pin, GPIO_PIN_SET);

  // Assert that the function returns 0 for a valid pin.
  TEST_ASSERT_EQUAL(0, result);
}

void test_LED_CONTROLLER_set_led_with_invalid_pin(void) {
  uint16_t pin = 10;

  m_pinMapping[0].port = (void *)10;
  m_pinMapping[0].pin = 0; // make pin invalid

  // we don't have to expect WritePin because it will never be called
  int result = LED_CONTROLLER_set_led(pin, GPIO_PIN_SET);

  // Assert that the function returns -1 for a invalid pin.
  TEST_ASSERT_EQUAL(-1, result);
}