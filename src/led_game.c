#include <stm32f411xe.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_adc.h>

#include "led_controller.h"
#include "led_game.h"
#include "led_pattern.h"

/* DEFINES */
#if defined(TEST) || defined(TEST)
#undef STATIC
#define STATIC
#else
#define STATIC static
#endif

/* TYPES */
enum GameState { waiting_for_start = 0, countdown, playing, result };

/* PRIVATE VARIABLES */
STATIC struct GameSettings *m_gameSettings;
STATIC uint32_t m_level = 0;
STATIC uint16_t m_selectedPin = 0;
STATIC uint8_t *m_buttonIsPressed = NULL;

STATIC enum GameState m_currentState = waiting_for_start;

/* PROTOTYPES */

STATIC int _render_led();
STATIC void _handle_input();
STATIC int _execute_waiting_for_game_start();
STATIC int _execute_countdown();
STATIC int _execute_playing();
STATIC int _execute_result();
STATIC int _get_semi_random_pin();
STATIC uint32_t _calculate_game_speed();

/* PUBLIC FUNCTIONS */

void LED_GAME_setup(struct GameSettings *settings, uint8_t *isButtonPressed) {
  m_gameSettings = settings;
  m_buttonIsPressed = isButtonPressed;
}

int LED_GAME_update(uint16_t selectedPin) {

  if (m_gameSettings->pinMapping == NULL) {
    return -1;
  }
  if (m_gameSettings->patternBuffer == NULL) {
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

  patternCount = LED_PATTERN_create_blink(
      m_gameSettings->pinMapping, m_gameSettings->ledCount, 250,
      m_gameSettings->patternBuffer, m_gameSettings->bufferSize);
  err = LED_PATTERN_play_pattern(m_gameSettings->pinMapping,
                                 m_gameSettings->ledCount,
                                 m_gameSettings->patternBuffer, patternCount);
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
  for (int i = 0; i < m_gameSettings->ledCount; i++) {
    LED_CONTROLLER_turn_off(m_gameSettings->pinMapping,
                            m_gameSettings->ledCount,
                            m_gameSettings->pinMapping[i].pin);
  }
  for (int i = 0; i < 4; i++) {
    LED_CONTROLLER_turn_off(m_gameSettings->pinMapping,
                            m_gameSettings->ledCount,
                            m_gameSettings->winningPin);
    HAL_Delay(250);
    LED_CONTROLLER_turn_on(m_gameSettings->pinMapping, m_gameSettings->ledCount,
                           m_gameSettings->winningPin);
    HAL_Delay(250);
  }
  patternCount = LED_PATTERN_create_countdown(
      m_gameSettings->pinMapping, m_gameSettings->ledCount, 250,
      m_gameSettings->patternBuffer, m_gameSettings->bufferSize);
  err = LED_PATTERN_play_pattern(m_gameSettings->pinMapping,
                                 m_gameSettings->ledCount,
                                 m_gameSettings->patternBuffer, patternCount);
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
  int gameSpeed = 0;
  int err = 0;
  int patternCount;

  gameSpeed = _calculate_game_speed();
  patternCount = LED_PATTERN_create_circle(
      m_gameSettings->pinMapping, m_gameSettings->ledCount, gameSpeed,
      m_gameSettings->patternBuffer, m_gameSettings->bufferSize);
  err = LED_PATTERN_play_pattern(m_gameSettings->pinMapping,
                                 m_gameSettings->ledCount,
                                 m_gameSettings->patternBuffer, patternCount);
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
  if (m_selectedPin == m_gameSettings->winningPin) {
    m_level++;
    m_gameSettings->winningPin = _get_semi_random_pin();

    for (uint8_t i = 0; i < 5; i++) {
      patternCount = LED_PATTERN_create_circle(
          m_gameSettings->pinMapping, m_gameSettings->ledCount, 25 * i,
          m_gameSettings->patternBuffer, m_gameSettings->bufferSize);
      err = LED_PATTERN_play_pattern(
          m_gameSettings->pinMapping, m_gameSettings->ledCount,
          m_gameSettings->patternBuffer, patternCount);
      if (err != 0) {
        return err;
      }
    }
    m_currentState = countdown;
  } else {
    patternCount = LED_PATTERN_create_countdown(
        m_gameSettings->pinMapping, m_gameSettings->ledCount, 100,
        m_gameSettings->patternBuffer, m_gameSettings->bufferSize);
    err = LED_PATTERN_play_pattern(m_gameSettings->pinMapping,
                                   m_gameSettings->ledCount,
                                   m_gameSettings->patternBuffer, patternCount);
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
  HAL_ADC_Start(m_gameSettings->adc);
  // ADC poll for conversion
  if (HAL_OK != HAL_ADC_PollForConversion(m_gameSettings->adc, 100)) {
    return -1;
  }
  // get the ADC conversion value
  pin = m_gameSettings
            ->pinMapping[HAL_ADC_GetValue(m_gameSettings->adc) %
                         m_gameSettings->ledCount]
            .pin;
  // end ADC convertion
  HAL_ADC_Stop(m_gameSettings->adc);

  return pin;
}

STATIC uint32_t _calculate_game_speed() {
  int speedIncrease = 0;

  speedIncrease = m_level * m_gameSettings->levelIncrement;
  if (speedIncrease >= m_gameSettings->startSpeed) {
    return m_gameSettings->levelIncrement;
  }
  return m_gameSettings->startSpeed - speedIncrease;
}