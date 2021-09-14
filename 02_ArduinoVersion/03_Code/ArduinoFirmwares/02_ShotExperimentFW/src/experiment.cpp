//
// Created by alfy on 10/09/21.
//

#include "experiment.h"

int controll(struct meanOffset *pMean, struct sample *pRead, unsigned long tic) {
//    return triangleSignal(tic, 200);
//  return rapidShot(tic);
  return prop(tic,pRead->V2_read - pMean->V2_mean);
}

/// Base Signal

// Equazione retta con valori e tempi:
//   - Vstart, tStart := start Point
//   - Vend, tEnd     := end Point
//  Satura ai valori Vstart e Vend prima e dopo l'intervallo di interesse

int ramp(uint64_t t, int vStart, uint64_t tStart, int vEnd, uint64_t tEnd) {
  // Saturazione
  if (t < tStart)
    return vStart;
  else if (t > tEnd)
    return vEnd;
  // Retta
  unsigned int dt = t - tStart;
  return vStart + int((vEnd - vStart) / float(tEnd - tStart) * dt);
}

/// Complex signal

int triangleSignal(uint64_t t, int msQuartPeriod) {
  static uint64_t startTic = 0;
  int dTic = t - startTic;
  int pwm = 0;
  if (dTic < ticConvert(msQuartPeriod))
    pwm = ramp(dTic, 0, 0, 100, ticConvert(msQuartPeriod));
  else if (dTic < (ticConvert(msQuartPeriod) * 3))
    pwm = ramp(dTic, 100, ticConvert(msQuartPeriod), -100, ticConvert(msQuartPeriod) * 3);
  else if (dTic < (ticConvert(msQuartPeriod) * 4))
    pwm = ramp(dTic, -100, ticConvert(msQuartPeriod) * 3, 0, ticConvert(msQuartPeriod) * 4);
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

#define t1 ticConvert(500)       // Rise Ramp
#define t2 ticConvert(200) + t1 // High set
#define t3 ticConvert(500) + t2  // falling Ramp
#define t4 tQuiet + t3           // 0 set

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
    pwmRapidShot = ramp(dTic, 0, 0, 100, t1);
  } else if (dTic <= t2) {
    pwmRapidShot = 100;
  } else if (dTic <= t3) {
    // falling ramp
    pwmRapidShot = ramp(dTic, 100, t2, 0, t3);
  } else if (dTic <= t4) {
    pwmRapidShot = 0;
  }

  return pwmRapidShot;
}

// Controllo
#define V2Ref volt2adc(1)
#define tcStart ticConvert(100)       // start Experiment
#define tcEnd ticConvert(1000) + tcStart // stop Experiment

int e;
int prop(uint64_t t, int v2){
  if(t<tcStart || t>tcEnd)
    return 0;

  e = v2 - V2Ref;
  long pwmCtrl = -(e)/15;
  return pwmCtrl;
}