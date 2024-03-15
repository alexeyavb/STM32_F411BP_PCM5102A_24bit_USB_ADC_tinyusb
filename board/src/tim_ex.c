#include "tim.h"
#include "tim_ex.h"

void SetPWM(uint16_t value){
    TIM_OC_InitTypeDef sConfigOC;
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1); // таймер №4, канал №1
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}

