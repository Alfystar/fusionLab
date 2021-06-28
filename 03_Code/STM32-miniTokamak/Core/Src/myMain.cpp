/*
 * myMain.cpp
 *
 *  Created on: Jun 28, 2021
 *      Author: alfy
 */

#include "myMain.h"

uint32_t a1_read;
uint32_t v2_read;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);	// Blue led
	/*
	if(htim->Instance == htim4.Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);	// Green led
		}if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);	// Orange led
		}
	}*/
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);	// Red led

	if(htim->Instance == htim4.Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);	// Green led
		}if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);	// Orange led
		}
	}
}

void setup(void) {

	HAL_ADC_Start_DMA(&hadc1, &a1_read, 1); // start adc in DMA mode
	HAL_ADC_Start_DMA(&hadc2, &v2_read, 1); // start adc in DMA mode
	HAL_TIM_Base_Start_IT(&htim4);
	//HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	//HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_2);
}

int val = 0;
void loop(void) {
	HAL_Delay(1000);
	val += 100;
	val = val % (TIM4->ARR + 1); // auto-reload register
	//__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, val);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, TIM4->ARR - val);



}
