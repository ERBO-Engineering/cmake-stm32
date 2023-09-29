#ifndef LED_PATTERN_H
#define LED_PATTERN_H

#include "led_controller.h"
#include <stdint.h>
#include <stm32f4xx_hal_gpio.h>

/**
 * @brief defines a single step in a pattern
 *
 */
struct __attribute__((packed)) LedPatternStep {
  uint16_t pin;
  uint32_t state;
  uint32_t sleepTime;
};

/**
 * @brief fills the outBuffer with a new pattern that matches an Led going in a
 * circle if you have your LEDS laid out in a circle ofcourse ;)
 * The pattern just turns on the next LED and turns the previous LED off with a
 * regulated speed
 * @param pinMap map of your available led pins
 * @param ledCount total count of your LEDS should match your pinMap
 * @param speed speed in ms for each transition
 * @param outBuffer buffer you want to have filled with the pattern
 * @param bufferSize size of the buffer :D
 * @return int returns the actual count of steps in the pattern
 */
int LED_PATTERN_create_circle(const struct LedPinMapping pinMap[],
                              uint32_t ledCount, uint32_t speed,
                              struct LedPatternStep outBuffer[],
                              uint32_t bufferSize);

/**
 * @brief fills the outBuffer with a new pattern that matches all LEDs blinking

 * @param pinMap map of your available led pins
 * @param ledCount total count of your LEDS should match your pinMap
 * @param speed speed in ms for each transition
 * @param outBuffer buffer you want to have filled with the pattern
 * @param bufferSize size of the buffer :D
 * @return int returns the actual count of steps in the pattern
 */
int LED_PATTERN_create_blink(const struct LedPinMapping pinMap[],
                             uint32_t ledCount, uint32_t speed,
                             struct LedPatternStep outBuffer[],
                             uint32_t bufferSize);

/**
 * @brief fills the outBuffer with a new pattern that turns all LEDs on and then
 * with each defined speed interval turn them off in a reverse order

 * @param pinMap map of your available led pins
 * @param ledCount total count of your LEDS should match your pinMap
 * @param speed speed in ms for each transition
 * @param outBuffer buffer you want to have filled with the pattern
 * @param bufferSize size of the buffer :D
 * @return int returns the actual count of steps in the pattern
 */
int LED_PATTERN_create_countdown(const struct LedPinMapping pinMap[],
                                 uint32_t ledCount, uint32_t speed,
                                 struct LedPatternStep outBuffer[],
                                 uint32_t bufferSize);

/**
 * @brief execute the pattern found in the outBuffer
 *
 * @param pinMapping pointer to the pinMapping struct
 * @param ledCount amount of leds in the pinmap
 * @param patternBuffer
 * @param stepCount how many steps have to be played from the buffer
 * @return int
 */
int LED_PATTERN_play_pattern(struct LedPinMapping *pinMapping, int ledCount,
                             struct LedPatternStep patternBuffer[],
                             uint32_t stepCount);

struct LedPatternStep *LED_PATTERN_get_current_step();

#endif // !LED_PATTERN_H