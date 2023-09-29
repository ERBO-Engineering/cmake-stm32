#include "string.h"
#include "unity.h"

// FILE UNDER TEST
#include "led_game.h"

// MOCK HAL
#include "mock_stm32f4xx_hal.h"
#include "mock_stm32f4xx_hal_adc.h"
#include <stm32f411xe.h>

// MOCK led
#include "mock_led_controller.h"
#include "mock_led_pattern.h"

extern uint32_t m_currentState;
extern uint32_t m_level;
extern uint16_t m_selectedPin;

// prototypes for private functions you want to test
void _handle_input();
int _execute_waiting_for_game_start();
int _execute_countdown();
int _execute_playing();
int _execute_result();
int _get_semi_random_pin();
uint32_t _calculate_game_speed();
// set global pinmapping to allow reusing
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

void test_LED_GAME__handle_input__pressed(void) {
  struct GameSettings settings = {};
  uint8_t buttonIsPressed = 1;
  LED_GAME_setup(&settings, &buttonIsPressed);

  // waiting for start -> goto countdown on press
  m_currentState = 0;
  buttonIsPressed = 1;
  _handle_input();
  TEST_ASSERT_EQUAL(0, buttonIsPressed);
  TEST_ASSERT_EQUAL(1, m_currentState);

  // count down don't interrupt
  m_currentState = 1;
  buttonIsPressed = 1;

  _handle_input();
  TEST_ASSERT_EQUAL(0, buttonIsPressed);
  TEST_ASSERT_EQUAL(1, m_currentState);

  // playing -> go to result on press
  m_currentState = 2;
  buttonIsPressed = 1;
  _handle_input();
  TEST_ASSERT_EQUAL(0, buttonIsPressed);
  TEST_ASSERT_EQUAL(3, m_currentState);

  // result -> don't interrupt animation
  m_currentState = 3;
  buttonIsPressed = 1;
  _handle_input();
  TEST_ASSERT_EQUAL(0, buttonIsPressed);
  TEST_ASSERT_EQUAL(3, m_currentState);
}

// _execute_waiting_for_game_start() ----------------------------------------

void test_LED_GAME__execute_waiting_for_game_start_success() {
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer)

  };

  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  LED_GAME_setup(&settings, &buttonIsPressed);

  LED_PATTERN_create_blink_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, 250, settings.patternBuffer,
      settings.bufferSize, patternCount);

  LED_PATTERN_play_pattern_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.patternBuffer,
      patternCount, 0);

  int result = _execute_waiting_for_game_start();

  // no error
  TEST_ASSERT_EQUAL(0, result);
}

void test_LED_GAME__execute_waiting_for_game_start_play_pattern_failed() {
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer)

  };

  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  int error = -1;
  LED_GAME_setup(&settings, &buttonIsPressed);

  LED_PATTERN_create_blink_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, 250, settings.patternBuffer,
      settings.bufferSize, patternCount);

  // play pattern returns -1 as error
  LED_PATTERN_play_pattern_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.patternBuffer,
      patternCount, error);

  int result = _execute_waiting_for_game_start();

  // should have error error
  TEST_ASSERT_EQUAL(error, result);
}

// _execute_countdown() ----------------------------------------

void test_LED_GAME__execute_countdown_success() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2

  };

  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  LED_GAME_setup(&settings, &buttonIsPressed);

  // turn of all leds
  for (int i = 0; i < settings.ledCount; i++) {
    LED_CONTROLLER_turn_off_ExpectAnyArgsAndReturn(0);
  }

  // blink winning pin 4 times
  for (int i = 0; i < 4; i++) {
    LED_CONTROLLER_turn_off_ExpectAndReturn(
        settings.pinMapping, settings.ledCount, settings.winningPin, 0);
    HAL_Delay_Expect(250);
    LED_CONTROLLER_turn_on_ExpectAndReturn(
        settings.pinMapping, settings.ledCount, settings.winningPin, 0);
    HAL_Delay_Expect(250);
  }

  // create actual countdown animation
  LED_PATTERN_create_countdown_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, 250, settings.patternBuffer,
      settings.bufferSize, patternCount);

  // play pattern returns -1 as error
  LED_PATTERN_play_pattern_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.patternBuffer,
      patternCount, 0);

  int result = _execute_countdown();

  // should have error error
  TEST_ASSERT_EQUAL(0, result);
  // should have changed state
  TEST_ASSERT_EQUAL(2, m_currentState);
}

void test_LED_GAME__execute_countdown_fail_on_pattern_play() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2

  };

  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  int error = -1;
  LED_GAME_setup(&settings, &buttonIsPressed);

  // ignore non important calls
  LED_CONTROLLER_turn_off_IgnoreAndReturn(0);
  LED_CONTROLLER_turn_on_IgnoreAndReturn(0);
  HAL_Delay_Ignore();
  LED_PATTERN_create_countdown_IgnoreAndReturn(patternCount);

  // play pattern returns -1 as error
  LED_PATTERN_play_pattern_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.patternBuffer,
      patternCount, error);

  int result = _execute_countdown();

  // should have error
  TEST_ASSERT_EQUAL(error, result);
  // should not have changed state
  TEST_ASSERT_EQUAL(0, m_currentState);
}

// _execute_playing() ----------------------------------------

void test_LED_GAME__execute_playing_success() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };

  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  LED_GAME_setup(&settings, &buttonIsPressed);

  // create actual countdown animation
  LED_PATTERN_create_circle_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.startSpeed,
      settings.patternBuffer, settings.bufferSize, patternCount);

  // play pattern returns -1 as error
  LED_PATTERN_play_pattern_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.patternBuffer,
      patternCount, 0);

  int result = _execute_playing();

  // should have error error
  TEST_ASSERT_EQUAL(0, result);
}

void test_LED_GAME__execute_playing_fail_on_pattern() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };

  int error = -1;
  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  LED_GAME_setup(&settings, &buttonIsPressed);

  // create actual countdown animation
  LED_PATTERN_create_circle_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.startSpeed,
      settings.patternBuffer, settings.bufferSize, patternCount);

  // play pattern returns -1 as error
  LED_PATTERN_play_pattern_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.patternBuffer,
      patternCount, error);

  int result = _execute_playing();

  // should have error error
  TEST_ASSERT_EQUAL(error, result);
}

// _execute_result() ----------------------------------------

void test_LED_GAME__execute_result_win_success() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };

  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  uint16_t newWinningPinIndex = 3;
  LED_GAME_setup(&settings, &buttonIsPressed);
  m_selectedPin = settings.winningPin;
  m_level = 0;

  // new winning pin selecting mock
  HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_OK);
  HAL_ADC_PollForConversion_ExpectAnyArgsAndReturn(HAL_OK);
  HAL_ADC_GetValue_ExpectAnyArgsAndReturn(newWinningPinIndex);
  HAL_ADC_Stop_ExpectAnyArgsAndReturn(HAL_OK);

  // create winning spinner animation
  for (uint8_t i = 0; i < 5; i++) {
    // create actual countdown animation
    LED_PATTERN_create_circle_ExpectAndReturn(
        settings.pinMapping, settings.ledCount, 25 * i, settings.patternBuffer,
        settings.bufferSize, patternCount);

    // play pattern returns -1 as error
    LED_PATTERN_play_pattern_ExpectAndReturn(
        settings.pinMapping, settings.ledCount, settings.patternBuffer,
        patternCount, 0);
  }

  int result = _execute_result();

  // should have no error
  TEST_ASSERT_EQUAL(0, result);
  // winning pin should be updated
  TEST_ASSERT_EQUAL(pinMapping[newWinningPinIndex].pin, settings.winningPin);
  // state should go to countdown
  TEST_ASSERT_EQUAL(1, m_currentState);
  // level should be incremented
  TEST_ASSERT_EQUAL(1, m_level);
}

void test_LED_GAME__execute_result_win_fail_on_pattern() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };

  uint8_t buttonIsPressed = 1;
  uint16_t newWinningPinIndex = 3;
  int error = -1;
  LED_GAME_setup(&settings, &buttonIsPressed);
  m_selectedPin = settings.winningPin;
  m_level = 0;

  // new winning pin selecting mock
  HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_OK);
  HAL_ADC_PollForConversion_ExpectAnyArgsAndReturn(HAL_OK);
  HAL_ADC_GetValue_ExpectAnyArgsAndReturn(newWinningPinIndex);
  HAL_ADC_Stop_ExpectAnyArgsAndReturn(HAL_OK);

  // fail pattern playing on error
  LED_PATTERN_create_circle_ExpectAnyArgsAndReturn(0);
  // check that it did not get called multiple times
  LED_PATTERN_play_pattern_ExpectAnyArgsAndReturn(error);

  int result = _execute_result();

  // should have no error
  TEST_ASSERT_EQUAL(error, result);
  // winning pin should be updated
  TEST_ASSERT_EQUAL(pinMapping[newWinningPinIndex].pin, settings.winningPin);
  // state should not be changed because of error
  TEST_ASSERT_EQUAL(0, m_currentState);
}

void test_LED_GAME__execute_result_lose_success() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };

  uint8_t buttonIsPressed = 1;
  uint8_t patternCount = 5;
  LED_GAME_setup(&settings, &buttonIsPressed);
  m_selectedPin = 1;
  m_level = 0;

  // create loser countdown animation
  LED_PATTERN_create_countdown_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, 100, settings.patternBuffer,
      settings.bufferSize, patternCount);

  LED_PATTERN_play_pattern_ExpectAndReturn(
      settings.pinMapping, settings.ledCount, settings.patternBuffer,
      patternCount, 0);

  int result = _execute_result();

  // should have no error
  TEST_ASSERT_EQUAL(0, result);
  // state should go to game start
  TEST_ASSERT_EQUAL(0, m_currentState);
  // level should not be incremented
  TEST_ASSERT_EQUAL(0, m_level);
}

void test_LED_GAME__execute_result_lose_fail_on_pattern() {
  m_currentState = 0;
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };

  uint8_t buttonIsPressed = 1;
  int error = -1;
  LED_GAME_setup(&settings, &buttonIsPressed);
  m_selectedPin = 1;
  m_level = 0;

  // new winning pin selecting mock

  // fail pattern playing on error
  LED_PATTERN_create_countdown_ExpectAnyArgsAndReturn(0);
  // check that it did not get called multiple times
  LED_PATTERN_play_pattern_ExpectAnyArgsAndReturn(error);

  int result = _execute_result();

  // should have no error
  TEST_ASSERT_EQUAL(error, result);
  // winning pin should not be updated
  TEST_ASSERT_EQUAL(2, settings.winningPin);
  // state should not be changed because of error
  TEST_ASSERT_EQUAL(0, m_currentState);
}

// _get_semi_random_pin() ----------------------------------------

void test_LED_GAME__get_semi_random_pin_success() {
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };
  uint16_t newPinIndex = 2;
  LED_GAME_setup(&settings, 0);

  // mock hal ADC for random pin
  HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_OK);
  HAL_ADC_PollForConversion_ExpectAnyArgsAndReturn(HAL_OK);
  HAL_ADC_GetValue_ExpectAnyArgsAndReturn(newPinIndex);
  HAL_ADC_Stop_ExpectAnyArgsAndReturn(HAL_OK);

  int pin = _get_semi_random_pin();

  TEST_ASSERT_EQUAL(pinMapping[2].pin, pin);
}

void test_LED_GAME__get_semi_random_pin_fail_during_conversion() {
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };
  LED_GAME_setup(&settings, 0);

  // mock hal ADC for random pin
  HAL_ADC_Start_ExpectAnyArgsAndReturn(HAL_OK);
  HAL_ADC_PollForConversion_ExpectAnyArgsAndReturn(HAL_ERROR);

  int pin = _get_semi_random_pin();

  TEST_ASSERT_EQUAL(-1, pin);
}

// _calculate_game_speed() ----------------------------------------
void test_LED_GAME__calculate_game_speed() {
  struct LedPatternStep buffer[10] = {};
  struct GameSettings settings = {.pinMapping = pinMapping,
                                  .ledCount = LED_COUNT,
                                  .patternBuffer = buffer,
                                  .bufferSize = sizeof(buffer),
                                  .winningPin = 2,
                                  .startSpeed = 250,
                                  .levelIncrement = 10,
                                  .startLevel = 0

  };
  LED_GAME_setup(&settings, 0);

  // speed should be 250 because no level increment
  TEST_ASSERT_EQUAL(settings.startSpeed, _calculate_game_speed());

  // speed should be 240 because level 1
  m_level = 1;
  TEST_ASSERT_EQUAL(settings.startSpeed - settings.levelIncrement,
                    _calculate_game_speed());

  // if you are such an high level it cannot go higher in speed then maintain
  // highest speed
  m_level = 25;
  TEST_ASSERT_EQUAL(settings.levelIncrement, _calculate_game_speed());
}
