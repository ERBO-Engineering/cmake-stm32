#ifndef LED_GAME_H
#define LED_GAME_H

#include "led_controller.h"
#include "led_pattern.h"
#include <stdint.h>
#include <stm32f411xe.h>
#include <stm32f4xx_hal_adc.h>

/*PUBLIC TYPES*/
struct GameSettings {
  struct LedPinMapping *pinMapping;
  uint32_t ledCount;
  struct LedPatternStep *patternBuffer;
  uint32_t bufferSize;

  uint16_t winningPin;
  uint32_t startLevel;
  uint32_t startSpeed;
  uint32_t levelIncrement;

  ADC_HandleTypeDef *adc;
};

/* PUBLIC FUNCTIONS */

/**
 * @brief setup all the required settings for your awesome LED game
 *
 * @param settings pointer to the struct containing all the game settings
 * @param isButtonPressed pointer to the value that indicates button is pressed
 * or not
 */
void LED_GAME_setup(struct GameSettings *settings, uint8_t *isButtonPressed);

/**
 * @brief update game loop, should be called in the super loop
 *
 * @param selectedPin pin that is active in the pattern when button got
 * triggered through the Interrupt
 * @return int returns 0 if no errors and != 0 when there is an error
 */
int LED_GAME_update(uint16_t selectedPin);
#endif // LED_GAME_H