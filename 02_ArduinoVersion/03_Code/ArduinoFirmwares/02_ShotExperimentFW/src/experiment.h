//
// Created by alfy on 10/09/21.
//

#ifndef INC_01_TESTFW_SRC_EXPERIMENT_H
#define INC_01_TESTFW_SRC_EXPERIMENT_H
#include <Arduino.h>
#include "pack.h"
#include "globalDef.h"

#define ticConvert(ms) (ms * 1000UL) / dtExperiment

// Have to set pwm in pWrite
int controll(packArd2Linux *pWrite, unsigned long tic);

/// Base Signal
int ramp(uint64_t t, int vStart, uint64_t tStart, int vEnd, uint64_t tEnd);

/// Complex signal
int triangleSignal(uint64_t t, int msQuartPeriod);
int rapidShot(uint64_t t);
#endif // INC_01_TESTFW_SRC_EXPERIMENT_H
