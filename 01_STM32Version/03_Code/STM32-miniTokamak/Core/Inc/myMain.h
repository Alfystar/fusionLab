/*
 * myMain.h
 *
 *  Created on: Jun 28, 2021
 *      Author: alfy
 */

#ifndef SRC_MYMAIN_H_
#define SRC_MYMAIN_H_

// HAL include
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "stm32f4xx.h"

// Emp Include
#include <MP_ST_usb.h>
#include "pack.h"
using namespace EMP;

void setup(void);

void loop(void);

// ADC OP
void adcSetup();
// Event to send ADC read to the Companion
void sampleEvent();


// PWM period 2Khz, TIMER 3 USED
void pwmSetUp();
void pwmSet(int16_t pwm); // val 0 -> 999 (0% to 100%)
void HIGH_PWM();
void LOW_PWM();

#endif /* SRC_MYMAIN_H_ */
