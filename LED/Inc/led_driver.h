/**
 * @file led_driver.h
 * @brief LED driver interface for STM32 HAL-based GPIO control.
 * @author AGH Eko-Energia
 * @author Kacper Lasota
 *
 * This module provides a simple non-blocking LED state machine with
 * off, on and blinking modes synchronized via a shared syncTick counter.
 */
#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "main.h"

/**
 * @enum LED_STATE_e
 * @brief LED behavior states for LED driver.
 */
typedef enum {
    LED_OFF = 0,          /**< LED is turned off */
    LED_FAST_BLINK = 100, /**< LED toggles at fast blink interval (ms) */
    LED_BLINK = 1000,     /**< LED toggles at regular blink interval (ms) */
    LED_ON = 1001         /**< LED is permanently on (treated as stable state) */
} LED_STATE_e;

/**
 * @struct LED
 * @brief LED driver context structure.
 */
struct LED {
    LED_STATE_e state;       /**< Current LED state */
    GPIO_TypeDef *GPIO_Port; /**< STM32 GPIO port */
    uint16_t GPIO_Pin;       /**< STM32 GPIO pin */
};

/**
 * @brief Increment the shared sync tick (call from HAL_IncTick).
 *
 * Hook this into HAL_IncTick so syncTick advances once per millisecond:
 * @code
 * void HAL_IncTick(void)
 * {
 *     uwTick += uwTickFreq;
 *     LED_IncSyncTick();
 * }
 * @endcode
 */
void LED_IncSyncTick(void);

/**
 * @brief Overwrite syncTick (e.g. when a slave receives it on CAN).
 * @param tick Network-wide tick in milliseconds.
 */
void LED_SetSyncTick(uint32_t tick);

/**
 * @brief Read the current sync tick (e.g. for broadcasting on CAN).
 * @return Current syncTick value.
 */
uint32_t LED_GetSyncTick(void);

/**
 * @brief Process LED behavior based on state and syncTick phase.
 * @param led Pointer to LED context.
 */
void LED_Handle(struct LED *led);

/**
 * @brief Apply LED state change and GPIO output.
 * @param led Pointer to LED context.
 * @param state New LED state.
 */
void LED_ChangeState(struct LED *led, LED_STATE_e state);

#endif /* LED_DRIVER_H */
