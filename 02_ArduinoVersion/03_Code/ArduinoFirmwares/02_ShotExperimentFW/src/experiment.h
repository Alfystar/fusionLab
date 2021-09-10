//
// Created by alfy on 10/09/21.
//

#ifndef INC_01_TESTFW_SRC_EXPERIMENT_H
#define INC_01_TESTFW_SRC_EXPERIMENT_H
#include <Arduino.h>
#include "pack.h"
#include "globalDef.h"

// Have to set pwm in pWrite
int controll(packArd2Linux *pWrite, unsigned long tic);

int ramp(uint64_t t);

int rapidShot(uint64_t t);
#endif // INC_01_TESTFW_SRC_EXPERIMENT_H
