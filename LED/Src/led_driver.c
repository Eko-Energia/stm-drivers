#include "led_driver.h"

static uint32_t s_syncTick;
static uint32_t s_syncLocalTick;
static uint8_t s_syncTickValid;

static uint32_t LED_GetBlinkTick(void)
{
    if (!s_syncTickValid)
    {
        return HAL_GetTick();
    }

    return s_syncTick + (HAL_GetTick() - s_syncLocalTick);
}

static void LED_ApplyBlinkState(struct LED *led, uint32_t interval, uint32_t tick)
{
    const uint32_t pos = tick % (2u * interval);
    const GPIO_PinState pinState = (pos < interval) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, pinState);
}

void LED_SetSyncTick(uint32_t tick)
{
    s_syncTick = tick;
    s_syncLocalTick = HAL_GetTick();
    s_syncTickValid = 1;
}

void LED_Handle(struct LED *led)
{
    const uint32_t tick = LED_GetBlinkTick();
    switch(led->state)
    {
        case LED_OFF:
        case LED_ON:
        break;
        case LED_FAST_BLINK:
            LED_ApplyBlinkState(led, LED_FAST_BLINK, tick);
            break;
        case LED_BLINK:
            LED_ApplyBlinkState(led, LED_BLINK, tick);
            break;
    }
}

void LED_ChangeState(struct LED *led, LED_STATE_e state)
{
    const uint32_t tick = LED_GetBlinkTick();
    led->state = state;
    switch(led->state)
    {
        case LED_OFF:
            HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_RESET);
            break;
        case LED_FAST_BLINK:
            LED_ApplyBlinkState(led, LED_FAST_BLINK, tick);
            break;
        case LED_BLINK:
            LED_ApplyBlinkState(led, LED_BLINK, tick);
            break;
        case LED_ON:
            HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_SET);
        break;
    }
}
