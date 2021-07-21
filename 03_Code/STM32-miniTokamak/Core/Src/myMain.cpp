/*
 * myMain.cpp
 *
 *  Created on: Jun 28, 2021
 *      Author: alfy
 */

#include "myMain.h"
using namespace EMP;

MP_ST_usb<packLinux2STM, packSTM2Linux, STM32MP_templateDefault()> MP_St;
packLinux2STM pRead;
packSTM2Linux pWrite;

void setup(void) {
	adcSetup();
	memset(&pRead, 0, sizeof(packLinux2STM));
	memset(&pWrite, 0, sizeof(packSTM2Linux));
	HAL_Delay(1000);

	pwmSetUp();

}

void loop(void) {
	MP_St.getData_wait(&pRead);
	pwmSet(pRead.pwm);
}

#define Vsense 0.020	//Variazione tensione [V/A]
#define PartitoreScale 0.598	//Scale creato da R1 e R2 da 9.99K e 14.83K reali
#define adcBit 12
#define sysVolt 2.944 //AnalogRef [V]
uint32_t adc[2]; // a1_read, v2_read
uint32_t adcOff[2]; // a1_Off, v2_Off

void adcSetup() {
	memset(adc, 0, sizeof(adc));
	memset(adcOff, 0, sizeof(adcOff));
	HAL_ADC_Start_DMA(&hadc1, adc, 2); // start adc in DMA mode (PA2)
	HAL_Delay(1);
	for (int i = 0; i < (1 << 5); i++) {
		adcOff[0] += adc[0];
		adcOff[1] += adc[1];
		HAL_Delay(1);
	}
	// Media dei 2^5 campioni
	adcOff[0] = adcOff[0] >> 5;
	adcOff[1] = adcOff[1] >> 5;

//	HAL_ADC_Start_DMA(&hadc1, &a1_read, 1); // start adc in DMA mode (PA2)
//	HAL_ADC_Start_DMA(&hadc2, &v2_read, 1); // start adc in DMA mode (PA3)
}

#define AnalogConversion(adc) ((sysVolt)/((1 << adcBit)-1)) * adc

void sampleEvent() {
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);	// Orange led (GPIOD, GPIO_PIN_13)

	pWrite.A1_read = (float) ((AnalogConversion(((int32_t )(adc[0] - adcOff[0])))) / (Vsense * PartitoreScale));
	pWrite.V2_read = (float) AnalogConversion((int32_t )(adc[1] - adcOff[1]));
//	pWrite.V2_read = (float) ((int32_t )(adc[1] - adcOff[1]));
	pWrite.pwm = pRead.pwm;
	MP_St.packSend(&pWrite);
}

void pwmSetUp() {
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
	//	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
}

#define motorPort GPIOE
#define INA GPIO_PIN_9
#define INB GPIO_PIN_7
#define PWMA GPIO_PIN_11
#define PWMB GPIO_PIN_13

volatile bool dir = 0;
volatile unsigned int nextSpeed = 0;
void pwmSet(int16_t pwm) {
	if (pwm > 0)
		dir = 1;
	else
		dir = 0;
	nextSpeed = abs(pwm);

}

void HIGH_PWM() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);	// Green led (GPIOD, GPIO_PIN_12)
//	HAL_GPIO_WritePin(motorPort, INA | INB | PWMA | PWMB, GPIO_PIN_RESET);	// Hbridge soft break configuration
//	if (dir)
//		HAL_GPIO_WritePin(motorPort, INA | PWMA, GPIO_PIN_SET);	// Hbridge Positive current conf
//	else
//		HAL_GPIO_WritePin(motorPort, INB | PWMB, GPIO_PIN_SET);	// Hbridge Negative current conf

}

void LOW_PWM() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);	// Green led (GPIOD, GPIO_PIN_12)
//	HAL_GPIO_WritePin(motorPort, INA | INB | PWMA | PWMB, GPIO_PIN_RESET);	// Hbridge soft break configuration

}

uint32_t periodCount = 0;
#define SampleEventperiodCount 21	// SampleTime/Tperiod = 21 cycle

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM3) {
		periodCount = (periodCount + 1) % SampleEventperiodCount;
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, nextSpeed); 	// Green led (GPIOD, GPIO_PIN_12)

		if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) != 0)
			//	Reload the High level at the end of the period, ready for new count
			HIGH_PWM();
		else {
			LOW_PWM();
		}
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
			if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) < TIM3->ARR)
				LOW_PWM();
			else {
				HIGH_PWM();
			}
		}
//		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
//			if (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_2) != TIM3->ARR)
//				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);	// Orange led (GPIOD, GPIO_PIN_13)
//		}
	}
}

