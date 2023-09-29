#include "led_pattern.h"
#include "led_controller.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>

// gets updated when a pattern is played to keep track of the current active
// pattern step
struct LedPatternStep *m_currentStep = NULL;

int LED_PATTERN_create_circle(const struct LedPinMapping pinMap[],
                              uint32_t ledCount, uint32_t speed,
                              struct LedPatternStep outBuffer[],
                              uint32_t bufferSize) {
  uint32_t patternCount = 0;

  memset(outBuffer, 0, bufferSize);

  for (uint32_t i = 0; i < ledCount; i++) {

    if (i > 0) {
      outBuffer[patternCount].pin = pinMap[i - 1].pin;
      outBuffer[patternCount].state = GPIO_PIN_RESET;
      outBuffer[patternCount].sleepTime = 0;
      patternCount++;
    }

    outBuffer[patternCount].pin = pinMap[i].pin;
    outBuffer[patternCount].state = GPIO_PIN_SET;
    outBuffer[patternCount].sleepTime = speed;
    patternCount++;

    if (i == ledCount - 1) {
      outBuffer[patternCount].pin = pinMap[i].pin;
      outBuffer[patternCount].state = GPIO_PIN_RESET;
      outBuffer[patternCount].sleepTime = 0;
      patternCount++;
    }
  }

  return patternCount;
}

int LED_PATTERN_create_blink(const struct LedPinMapping pinMap[],
                             uint32_t ledCount, uint32_t speed,
                             struct LedPatternStep outBuffer[],
                             uint32_t bufferSize) {
  uint32_t patternCount = 0;

  memset(outBuffer, 0, bufferSize);

  for (uint32_t i = 0; i < ledCount; i++) {

    outBuffer[patternCount].pin = pinMap[i].pin;
    outBuffer[patternCount].state = GPIO_PIN_RESET;
    outBuffer[patternCount].sleepTime = 0;
    patternCount++;
  }

  outBuffer[patternCount].sleepTime = speed;
  patternCount++;

  for (uint32_t i = 0; i < ledCount; i++) {

    outBuffer[patternCount].pin = pinMap[i].pin;
    outBuffer[patternCount].state = GPIO_PIN_SET;
    outBuffer[patternCount].sleepTime = 0;
    patternCount++;
  }

  outBuffer[patternCount].sleepTime = speed;
  patternCount++;

  return patternCount;
}

int LED_PATTERN_create_countdown(const struct LedPinMapping pinMap[],
                                 uint32_t ledCount, uint32_t speed,
                                 struct LedPatternStep outBuffer[],
                                 uint32_t bufferSize) {
  uint32_t patternCount = 0;

  memset(outBuffer, 0, bufferSize);

  for (uint32_t i = 0; i < ledCount; i++) {
    outBuffer[patternCount].pin = pinMap[i].pin;
    outBuffer[patternCount].state = GPIO_PIN_SET;
    outBuffer[patternCount].sleepTime = 0;
    patternCount++;
  }

  for (uint32_t i = ledCount; i > 0; --i) {

    outBuffer[patternCount].pin = pinMap[i - 1].pin;
    outBuffer[patternCount].state = GPIO_PIN_RESET;
    outBuffer[patternCount].sleepTime = speed;
    patternCount++;
  }
  return patternCount;
}

int LED_PATTERN_play_pattern(struct LedPinMapping *pinMapping, int ledCount,
                             struct LedPatternStep patternBuffer[],
                             uint32_t stepCount) {
  int err = 0;
  for (uint32_t stepIndex = 0; stepIndex < stepCount; stepIndex++) {
    m_currentStep = &patternBuffer[stepIndex];
    if (m_currentStep->pin == 0) {
      HAL_Delay(m_currentStep->sleepTime);
      continue;
    }

    err = LED_CONTROLLER_set_led(pinMapping, ledCount, m_currentStep->pin,
                                 m_currentStep->state);
    if (err != 0) {
      return err;
    }
    if (m_currentStep->sleepTime > 0) {
      HAL_Delay(m_currentStep->sleepTime);
    }
  }

  return 0;
}

struct LedPatternStep *LED_PATTERN_get_current_step() { return m_currentStep; }
