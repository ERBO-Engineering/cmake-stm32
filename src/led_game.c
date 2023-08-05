#include <stm32f411xe.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_adc.h>

#include "led_controller.h"
#include "led_game.h"
#include "led_pattern.h"

/* DEFINES */

#if defined(UNIT_TEST) || defined(UNIT_TEST_x86_64)
#undef STATIC
#define STATIC
#else
#define STATIC static
#endif

/* TYPES */

enum GameState { waiting_for_start = 0, countdown, playing, result };

/* VARIABLES */

STATIC uint32_t m_level = 0;
STATIC uint32_t m_selectedPin = 0;

STATIC uint32_t m_levelIncrementValue = 5;
STATIC uint32_t m_startLevel = 0;
STATIC uint32_t m_startSpeed = 250;
STATIC uint16_t m_winningPin = 0;

STATIC uint32_t m_ledCount = 0;
const STATIC struct LedPinMapping *m_pinMapping = NULL;

STATIC struct LedPatternStep *m_patternBuffer = NULL;
STATIC uint32_t m_bufferSize = 0;

STATIC enum GameState m_currentState = waiting_for_start;

STATIC uint8_t *m_buttonIsPressed = NULL;
STATIC ADC_HandleTypeDef *m_adc = NULL;

/* PROTOTYPES */

STATIC int _render_led();
STATIC void _handle_input();
STATIC int _execute_waiting_for_game_start();
STATIC int _execute_countdown();
STATIC int _execute_playing();
STATIC int _execute_result();
STATIC int _get_semi_random_pin();

/* PUBLIC FUNCTIONS */

void LED_GAME_setup(const struct LedPinMapping pinMapping[], uint32_t ledCount,
                    struct LedPatternStep patternBuffer[], uint32_t bufferSize,
                    uint16_t winningPin, uint32_t startLevel,
                    uint32_t startSpeed, uint32_t levelIncrement,
                    uint8_t *buttonIsPressed, ADC_HandleTypeDef *adc) {
  m_pinMapping = pinMapping;
  m_ledCount = ledCount;

  m_patternBuffer = patternBuffer;
  m_bufferSize = bufferSize;

  m_winningPin = winningPin;
  m_startLevel = startLevel;
  m_startSpeed = startSpeed;
  m_levelIncrementValue = levelIncrement;

  m_buttonIsPressed = buttonIsPressed;
  m_adc = adc;
}

int LED_GAME_update(uint16_t selectedPin) {

  if (m_pinMapping == NULL) {
    return -1;
  }
  if (m_patternBuffer == NULL) {
    return -1;
  }

  int err = 0;
  m_selectedPin = selectedPin;
  _handle_input();
  err = _render_led();

  if (err != 0) {
    return err;
  }

  return 0;
}

/* PRIVATE FUNCTIONS */

/**
 * @brief handle input actions
 */
STATIC void _handle_input() {
  if (*m_buttonIsPressed) {

    switch (m_currentState) {
    case waiting_for_start:
      m_currentState = countdown;
      break;
    case countdown:
      break;
    case playing:
      m_currentState = result;
      break;
    case result:
      break;
    }
    *m_buttonIsPressed = 0;
  }
}

/**
 * @brief based on state render the correct LED pattern
 */
STATIC int _render_led() {
  switch (m_currentState) {
  case waiting_for_start:
    return _execute_waiting_for_game_start();
  case countdown: {
    return _execute_countdown();
  }
  case playing: {
    return _execute_playing();
  }
  case result: {
    return _execute_result();
  }
  }

  return 0;
}

/**
 * @brief show blinking pattern indicating game is waiting input to start
 */
STATIC int _execute_waiting_for_game_start() {
  int patternCount = 0;
  int err = 0;
  m_level = 0;
  m_selectedPin = 0;

  patternCount = LED_PATTERN_create_blink(m_pinMapping, LED_COUNT, 250,
                                          m_patternBuffer, m_bufferSize);
  err = LED_PATTERN_play_pattern(m_patternBuffer, patternCount);
  if (err != 0) {
    return err;
  }

  return 0;
}

/**
 * @brief first only blink the pin you need tap to win and then show circular
 * countdown
 */
STATIC int _execute_countdown() {
  int patternCount = 0;
  int err = 0;
  for (int i = 0; i < m_ledCount; i++) {
    LED_CONTROLLER_turn_off(m_pinMapping[i].pin);
  }
  for (int i = 0; i < 4; i++) {
    LED_CONTROLLER_turn_off(m_winningPin);
    HAL_Delay(250);
    LED_CONTROLLER_turn_on(m_winningPin);
    HAL_Delay(250);
  }
  patternCount = LED_PATTERN_create_countdown(m_pinMapping, LED_COUNT, 250,
                                              m_patternBuffer, m_bufferSize);
  err = LED_PATTERN_play_pattern(m_patternBuffer, patternCount);
  if (err != 0) {
    return err;
  }
  m_currentState = playing;
  return 0;
}

/**
 * @brief based on current level play the circle pattern at a certain speed
 */
STATIC int _execute_playing() {
  int speedIncrease = 0;
  int err = 0;
  int patternCount;

  speedIncrease = m_level * m_levelIncrementValue;
  if (speedIncrease > m_startSpeed) {
    speedIncrease = m_startSpeed - m_levelIncrementValue;
  }
  patternCount = LED_PATTERN_create_circle(m_pinMapping, LED_COUNT,
                                           m_startSpeed - speedIncrease,
                                           m_patternBuffer, m_bufferSize);
  err = LED_PATTERN_play_pattern(m_patternBuffer, patternCount);
  if (err != 0) {
    return err;
  }

  return 0;
}

/**
 * @brief when you win show a happy spin and increase the level and select a new
 * winning pin and go to countdown
 *
 * on lose show to countdown pattern in a sad way and go back to
 * waiting for start
 */
STATIC int _execute_result() {
  int patternCount = 0;
  int err = 0;

  // on win increase level and get new winning pin
  if (m_selectedPin == m_winningPin) {
    m_level++;
    m_winningPin = _get_semi_random_pin();

    for (uint8_t i = 0; i < 5; i++) {
      patternCount = LED_PATTERN_create_circle(m_pinMapping, LED_COUNT, 25 * i,
                                               m_patternBuffer, m_bufferSize);
      err = LED_PATTERN_play_pattern(m_patternBuffer, patternCount);
      if (err != 0) {
        return err;
      }
    }
    m_currentState = countdown;
  } else {
    patternCount = LED_PATTERN_create_countdown(m_pinMapping, LED_COUNT, 100,
                                                m_patternBuffer, m_bufferSize);
    err = LED_PATTERN_play_pattern(m_patternBuffer, patternCount);
    if (err != 0) {
      return err;
    }
    m_currentState = waiting_for_start;
  }

  return 0;
}

/**
 * @brief use the ADC on an analog pin to get a somewhat random number and range
 * it between the available LEDS
 *
 * @return a random pin from the pinMapping with semi random index
 */
STATIC int _get_semi_random_pin() {
  int pin = -1;
  // start ADC convertion
  HAL_ADC_Start(m_adc);
  // ADC poll for conversion
  if (HAL_OK != HAL_ADC_PollForConversion(m_adc, 100)) {
    return -1;
  }
  // get the ADC conversion value
  pin = m_pinMapping[HAL_ADC_GetValue(m_adc) % m_ledCount].pin;
  // end ADC convertion
  HAL_ADC_Stop(m_adc);

  return pin;
}