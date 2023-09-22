#include "string.h"
#include "unity.h"

// FILE UNDER TEST
#include "led_pattern.h"

// MOCK HAL
#include "mock_stm32f4xx_hal.h"
// MOCK led controller
#include "mock_led_controller.h"
#include <stm32f4xx_hal_gpio.h>

struct LedPinMapping pinMapping[LED_COUNT] = {
    {.pin = 1, .port = (GPIO_TypeDef *)1},
    {.pin = 2, .port = (GPIO_TypeDef *)2},
    {.pin = 3, .port = (GPIO_TypeDef *)3},
    {.pin = 4, .port = (GPIO_TypeDef *)4}};

void setUp(void) {
  // Set up any necessary initialization here.
}

void tearDown(void) {
  // Clean up after each test case.
}

// Play a small pattern and verify if set_led calls and delay calls are correct
void test_LED_PATTERN_play_pattern_success(void) {
  // setup play pattern
  struct LedPatternStep patternBuffer[] = {
      {.pin = 0, .sleepTime = 100}, // should only sleep and not call set pin
      {.pin = 1, .state = GPIO_PIN_SET, .sleepTime = 200},
      {.pin = 2, .state = GPIO_PIN_SET, .sleepTime = 300},
      {.pin = 3, .state = GPIO_PIN_SET, .sleepTime = 400}};

  // based on the pattern it should first wait and then SET each pin and call
  // DELAY for x time
  HAL_Delay_Expect(100);

  // expected LED 1 to set and delay to be called
  LED_CONTROLLER_set_led_ExpectAndReturn(
      pinMapping, LED_COUNT, patternBuffer[1].pin, patternBuffer[1].state, 0);
  HAL_Delay_Expect(200);

  // expected LED 2 to set and delay to be called
  LED_CONTROLLER_set_led_ExpectAndReturn(
      pinMapping, LED_COUNT, patternBuffer[2].pin, patternBuffer[2].state, 0);
  HAL_Delay_Expect(300);

  // expected LED 3 to set and delay to be called
  LED_CONTROLLER_set_led_ExpectAndReturn(
      pinMapping, LED_COUNT, patternBuffer[3].pin, patternBuffer[3].state, 0);
  HAL_Delay_Expect(400);

  int result = LED_PATTERN_play_pattern(pinMapping, LED_COUNT, patternBuffer,
                                        sizeof(patternBuffer));
  TEST_ASSERT_EQUAL(0, result);
}

// make the set_led method error and expect the play_pattern to correctly return
// this error
void test_LED_PATTERN_play_pattern_failed_to_set_pin(void) {
  // setup play pattern
  struct LedPatternStep patternBuffer[] = {
      {.pin = 1, .state = GPIO_PIN_SET, .sleepTime = 100},
      {.pin = 2, .state = GPIO_PIN_SET, .sleepTime = 200}};
  int expectedError = -1;

  // based on the pattern it should set 1 pin but we will force it to fail by
  // returning an error in set_led
  LED_CONTROLLER_set_led_ExpectAndReturn(pinMapping, LED_COUNT,
                                         patternBuffer[0].pin,
                                         patternBuffer[0].state, expectedError);

  // don't expect other things to happen since the function should return on
  // error
  int result = LED_PATTERN_play_pattern(pinMapping, LED_COUNT, patternBuffer,
                                        sizeof(patternBuffer));
  TEST_ASSERT_EQUAL(expectedError, result);
}