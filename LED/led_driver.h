#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "main.h"

#define LED_BUFF_SIZE 16

typedef enum {
    LED_OFF = 0,
    LED_FAST_BLINK = 100,
    LED_BLINK = 200,
    LED_ON = 1000
}LED_STATE_e;

struct {
    LED_STATE_e state,
    GPIO_TypeDef* GPIO_Port,
    uint16_t GPIO_Pin,
    uint32_t lastTick,
}LED;

void LED_Handle(LED *led);
void LED_ChangeState(LED *led);


#endif