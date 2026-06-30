#include "led_driver.h"

static uint32_t syncTick = 0;

void LED_IncSyncTick(void)
{
    syncTick++;
}

void LED_SetSyncTick(uint32_t tick)
{
    syncTick = tick;
}

uint32_t LED_GetSyncTick(void)
{
    return syncTick;
}

static void LED_ApplyBlinkState(struct LED *led, uint32_t interval)
{
    const uint32_t pos = syncTick % (2u * interval);
    const GPIO_PinState pinState = (pos < interval) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, pinState);
}

void LED_Handle(struct LED *led)
{
    switch (led->state)
    {
        case LED_OFF:
        case LED_ON:
            break;
        case LED_FAST_BLINK:
            LED_ApplyBlinkState(led, LED_FAST_BLINK);
            break;
        case LED_BLINK:
            LED_ApplyBlinkState(led, LED_BLINK);
            break;
    }
}

void LED_ChangeState(struct LED *led, LED_STATE_e state)
{
    led->state = state;

    switch (led->state)
    {
        case LED_OFF:
            HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_RESET);
            break;
        case LED_FAST_BLINK:
            LED_ApplyBlinkState(led, LED_FAST_BLINK);
            break;
        case LED_BLINK:
            LED_ApplyBlinkState(led, LED_BLINK);
            break;
        case LED_ON:
            HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_SET);
            break;
    }
}
