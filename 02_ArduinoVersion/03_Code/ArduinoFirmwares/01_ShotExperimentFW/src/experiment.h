//
// Created by alfy on 10/09/21.
//

#ifndef INC_01_TESTFW_SRC_EXPERIMENT_H
#define INC_01_TESTFW_SRC_EXPERIMENT_H
#include "globalDef.h"
#include "pack-Experiment.h"
#include <Arduino.h>

#define volt2adc(x) ((int)(((x) / (5.0 / (1023)) + 0.5)))
#define V2Ref volt2adc(0.5)

#define ticConvert(ms) (ms * 1000UL) / dtExperiment
#define tQuiet ticConvert(1000)

// Have to set pwm in pWrite
int controll(struct setUpPack *pMean, struct sample *pRead, unsigned long tic);

/// Base Signal
int ramp(uint64_t t, int vStart, uint64_t tStart, int vEnd, uint64_t tEnd);
int rampEps(uint64_t t, int vStart, uint64_t tStart, int vEnd, unsigned int eps);
int square3StateDHZN(uint64_t dt, int msDelay, int msHigh, int msZero, int msNegHigh);
int square3StateZHN(uint64_t dt, int msZero, int msHigh, int msNegHigh);

/// Periodic signal
int triangleSignal(uint64_t t, int msQuartPeriod);
int triangleSignalEps(uint64_t t);

int rapidShot(uint64_t t);
int rapidShotEps(uint64_t t);

int estimateSignalSquare(uint64_t t, int msZero, int msHigh, int msNegHigh);

/// Controll
int ctrl(uint64_t t, int v2);
#endif // INC_01_TESTFW_SRC_EXPERIMENT_H
