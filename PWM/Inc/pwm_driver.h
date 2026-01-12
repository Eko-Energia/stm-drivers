#ifndef PWM_SIGNAL_H
#define PWM_SIGNAL_H

#include "main.h"
#include <math.h>
#include <stdbool.h>


typedef struct {
    uint32_t Frequency;
    float PWM_Width;
    bool Read_Flag;
    volatile uint32_t IC_Val1;
    volatile uint32_t IC_Val2;
    volatile uint8_t Capture_count;
    float Widths[5];
} PWM_Signal;

void PWM_Initialize(PWM_Signal* signal, int frequency);
void PWM_Update(TIM_HandleTypeDef *htim, PWM_Signal *PWM, uint32_t channel);

#endif /* PWM_SIGNAL_H */
