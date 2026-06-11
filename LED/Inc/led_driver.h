/**
 * @file led_driver.h
 * @brief LED driver interface for STM32 HAL-based GPIO control.
 * @author AGH Eko-Energia
 * @author Kacper Lasota
 *
 * This module provides a simple non-blocking LED state machine with
 * off, on and blinking modes.
 */
#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "main.h"

/**
 * @enum LED_STATE_e
 * @brief LED behavior states for LED driver.
 */
typedef enum {
    LED_OFF = 0,        /**< LED is turned off */
    LED_FAST_BLINK = 100, /**< LED toggles at fast blink interval (ms) */
    LED_BLINK = 1000,    /**< LED toggles at regular blink interval (ms) */
    LED_ON = 1001       /**< LED is permanently on (treated as stable state) */
} LED_STATE_e;

/**
 * @struct LED
 * @brief LED driver context structure.
 */
struct LED{
    LED_STATE_e state;        /**< Current LED state */
    GPIO_TypeDef* GPIO_Port;  /**< STM32 GPIO port */
    uint16_t GPIO_Pin;       /**< STM32 GPIO pin */
    uint32_t lastTick;        /**< Unused; kept for struct layout compatibility */
};

/**
 * @brief Update the network tick used to synchronize LED blinks.
 * @param tick Network-wide tick in milliseconds (e.g. from a CAN message).
 *
 * Call this whenever a sync tick is received on the CAN bus. All boards
 * sharing the same tick will blink in phase.
 */
void LED_SetSyncTick(uint32_t tick);

/**
 * @brief Process LED behavior based on state and timing.
 * @param led Pointer to LED context.
 *
 * This function should be called periodically from a main loop or timer.
 * In blinking modes it drives the GPIO from the network sync tick when set
 * via LED_SetSyncTick(), otherwise from HAL_GetTick().
 */
void LED_Handle(struct LED *led);

/**
 * @brief Apply LED state change and GPIO output.
 * @param led Pointer to LED context.
 *
 * This function updates the actual GPIO output according to the
 * selected state. Blinking modes align to the current sync tick phase.
 */
void LED_ChangeState(struct LED *led, LED_STATE_e state);

#endif /* LED_DRIVER_H */
