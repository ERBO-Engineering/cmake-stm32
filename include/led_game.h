#ifndef LED_GAME_H
#define LED_GAME_H

#include "led_controller.h"
#include "led_pattern.h"
#include <stdint.h>
#include <stm32f411xe.h>
#include <stm32f4xx_hal_adc.h>

/**
 * @brief setup all the required settings for your awesome LED game
 *
 * @param pinMapping pinMapping of all your LEDs with port
 * @param ledCount total count of your LEDs
 * @param patternBuffer buffer in which all your patterns will be created
 * @param bufferSize size of the buffer
 * @param winningPin the GPIO that has to be active when the user clicks the
 * button to win the game
 * @param startLevel starting level count
 * @param startSpeed initial speed of the play pattern
 * @param levelIncrement how fast the speed of the play pattern increases with
 * each win
 * @param buttonIsPressed pointer to a uint8_t that is set through an interrupt
 * if the button is pressed.
 * @param adc
 */
void LED_GAME_setup(const struct LedPinMapping pinMapping[], uint32_t ledCount,
                    struct LedPatternStep patternBuffer[], uint32_t bufferSize,
                    uint16_t winningPin, uint32_t startLevel,
                    uint32_t startSpeed, uint32_t levelIncrement,
                    uint8_t *buttonIsPressed, ADC_HandleTypeDef *adc);

/**
 * @brief update game loop, should be called in the super loop
 *
 * @param selectedPin pin that is active in the pattern when button got
 * triggered through the Interrupt
 * @return int returns 0 if no errors and != 0 when there is an error
 */
int LED_GAME_update(uint16_t selectedPin);
#endif // LED_GAME_H