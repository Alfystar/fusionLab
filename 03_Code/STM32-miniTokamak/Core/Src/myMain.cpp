/*
 * myMain.cpp
 *
 *  Created on: Jun 28, 2021
 *      Author: alfy
 */

#include "myMain.h"
using namespace EMP;

uint32_t adc[2];
uint32_t a1_read = 0;
uint32_t v2_read = 0;

uint32_t periodCount = 0;
#define SampleEventperiodCount 21	// SampleTime/Tperiod = 21 cycle

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM3) {
		periodCount = (periodCount + 1) % SampleEventperiodCount;
		if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) != 0)
			//	Reload the High level at the end of the period, ready for new count
			HIGH_PWM();
		if (periodCount == 0)
			sampleEvent();

//			if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_2) != 0)
//				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);	// Orange led (GPIOD, GPIO_PIN_13)
	}

}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
//	On the pulse compare, change the state to LOW and wait the end of the period
	if (htim->Instance == TIM3) {
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
			if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) != TIM3->ARR)
				LOW_PWM();
		}
//		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
//			if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_2) != TIM3->ARR)
//				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);	// Orange led (GPIOD, GPIO_PIN_13)
//		}
	}
}

MP_ST_usb<packLinux2STM, packSTM2Linux, STM32MP_templateDefault()> MP_St;
packLinux2STM pRead;
packSTM2Linux pWrite;

void setup(void) {
	memset(adc, 0, sizeof(adc));
	HAL_ADC_Start_DMA(&hadc1, adc, 2); // start adc in DMA mode (PA2)
//	HAL_ADC_Start_DMA(&hadc1, &a1_read, 1); // start adc in DMA mode (PA2)
//	HAL_ADC_Start_DMA(&hadc2, &v2_read, 1); // start adc in DMA mode (PA3)

	memset(&pRead, 0, sizeof(packLinux2STM));
	memset(&pWrite, 0, sizeof(packSTM2Linux));
	HAL_Delay(1000);

	pwmSetUp();

}

void loop(void) {
	MP_St.getData_wait(&pRead);
	pwmSet(pRead.pwm);
}

void sampleEvent() {
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);	// Orange led (GPIOD, GPIO_PIN_13)
	pWrite.A1_read = (float) adc[0];
	pWrite.V2_read = (float) adc[1];
	MP_St.packSend(&pWrite);
}

void pwmSetUp() {
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
	//	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
}

void pwmSet(int pwm) {
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, abs(pwm)); 	// Green led (GPIOD, GPIO_PIN_12)
}

void HIGH_PWM() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);	// Green led (GPIOD, GPIO_PIN_12)
}

void LOW_PWM() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);	// Green led (GPIOD, GPIO_PIN_12)
}
