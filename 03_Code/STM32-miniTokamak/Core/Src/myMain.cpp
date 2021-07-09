/*
 * myMain.cpp
 *
 *  Created on: Jun 28, 2021
 *      Author: alfy
 */

#include "myMain.h"
#include "stm32f4xx.h"

uint32_t a1_read;
uint32_t v2_read;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//	Reload the High level at the end of the period, ready for new count

	if (htim->Instance == TIM3) {
		if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) != 0)
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);	// Green led (GPIOD, GPIO_PIN_12)
		if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_2) != 0)
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);	// Orange led (GPIOD, GPIO_PIN_13)
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
//	On the pulse compare, change the state to LOW and wait the end of the period
	if (htim->Instance == TIM3) {
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
			if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) != TIM3->ARR)
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);	// Green led (GPIOD, GPIO_PIN_12)
		}
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
			if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_2) != TIM3->ARR)
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);	// Orange led (GPIOD, GPIO_PIN_13)
		}
	}
}

void setup(void) {

	//HAL_ADC_Start_DMA(&hadc1, &a1_read, 1); // start adc in DMA mode
	//HAL_ADC_Start_DMA(&hadc2, &v2_read, 1); // start adc in DMA mode

	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);

	//HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2);
	//__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 900);

}

int val = 0;
void loop(void) {
	//flash_green_led_forever();
	HAL_Delay(50);
	val += 10;
	val = val % (TIM3->ARR + 1); // auto-reload register
//	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, val);
//	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (TIM3->ARR + 1) - val);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, val); 	// Green led (GPIOD, GPIO_PIN_12)
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (TIM3->ARR + 1) - val); 	// Orange led (GPIOD, GPIO_PIN_13)

}
