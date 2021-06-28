/*
 * myMain.h
 *
 *  Created on: Jun 28, 2021
 *      Author: alfy
 */

#ifndef SRC_MYMAIN_H_
#define SRC_MYMAIN_H_

#include "adc.h"
#include "dma.h"
#include "tim.h"
#include <MP_ST_usb.h>

extern uint32_t a1_read;
extern uint32_t v2_read;
extern int val;

void setup(void);

void loop(void);



#endif /* SRC_MYMAIN_H_ */
