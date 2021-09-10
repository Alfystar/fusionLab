//
// Created by alfy on 10/09/21.
//

#include "experiment.h"

// Have to set pwm in pWrite
int controll(packArd2Linux *pWrite, unsigned long tic) {
//  pWrite->read.pwm = triangleSignal(tic, 100);
  pWrite->read.pwm = rapidShot(tic);
  return pWrite->read.pwm;
}

/// Base Signal

// Equazione retta con valori e tempi:
//   - Vstart, tStart := start Point
//   - Vend, tEnd     := end Point
//  Satura ai valori Vstart e Vend prima e dopo l'intervallo di interesse

int ramp(uint64_t t, int vStart, uint64_t tStart, int vEnd, uint64_t tEnd) {
  if (t < tStart)
    return vStart;
  else if (t > tEnd)
    return vEnd;

  unsigned int dt = t - tStart;
  return vStart + int((vEnd - vStart) / float(tEnd - tStart) * dt);
}

/// Complex signal

int triangleSignal(uint64_t t, int msQuartPeriod) {
  static uint64_t startTic = 0;
  int dTic = t - startTic;
  int pwm = 0;
  if (dTic < ticConvert(msQuartPeriod))
    pwm = ramp(dTic, 0, 0, 255, ticConvert(msQuartPeriod));
  else if (dTic < (ticConvert(msQuartPeriod) * 3))
    pwm = ramp(dTic, 255, ticConvert(msQuartPeriod), -255, ticConvert(msQuartPeriod) * 3);
  else if (dTic < (ticConvert(msQuartPeriod) * 4))
    pwm = ramp(dTic, -255, ticConvert(msQuartPeriod) * 3, 0, ticConvert(msQuartPeriod) * 4);
  else {
    pwm = 0;
    startTic = t;
  }
  return pwm;
}

//: fai una variazione molto più rapida del PWM:
// da 0 dufy cycle passi a 100% in 100ms, rimani su per 1 s e
//: poi giù in 0.1s e ripeti. Poi mi chiami.
#define tQuiet ticConvert(1000)

#define t1 ticConvert(100)       // Rise Ramp
#define t2 ticConvert(1000) + t1 // High set
#define t3 ticConvert(100) + t2  // falling Ramp
#define t4 tQuiet + t3           // 0 set

#define UpLimit 128
#define downLimit 0

int rapidShot(uint64_t t) {
  static uint64_t startTic = 0;
  int pwmRapidShot;
  long dTic = t - startTic;
  if (dTic > t4) {
    startTic = t;
    pwmRapidShot = 0;
    dTic = t - startTic;
  }

  if (dTic <= t1) {
    pwmRapidShot = ramp(dTic, downLimit, 0, UpLimit, t1);
  } else if (dTic <= t2) {
    pwmRapidShot = UpLimit;
  } else if (dTic <= t3) {
    // falling ramp
    pwmRapidShot = ramp(dTic, UpLimit, t2, downLimit, t3);
  } else if (dTic <= t4) {
    pwmRapidShot = downLimit;
  }

  return pwmRapidShot;
}