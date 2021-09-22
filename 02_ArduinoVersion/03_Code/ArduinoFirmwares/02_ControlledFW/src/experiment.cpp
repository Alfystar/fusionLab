//
// Created by alfy on 10/09/21.
//

#include "experiment.h"

int controll(struct setUpPack *pMean, struct sample *pRead, unsigned long tic) {
  //    return triangleSignal(tic, 200);
  //      return rapidShot(tic);
  return ctrl(tic, pRead->V2_read - pMean->V2_mean);
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

int rampEps(uint64_t t, int vStart, uint64_t tStart, int vEnd, unsigned int eps) {
  // Saturazione
  if (t < tStart)
    return vStart;
  // Retta-eps
  unsigned int dt = t - tStart;
  int val;
  if (sign(vEnd - vStart) == 1) {
    val = vStart + eps * dt;
    if (val > vEnd)
      val = vEnd;
  } else {
    val = vStart - eps * dt;
    if (val < vEnd)
      val = vEnd;
  }

  return val;
}

/// Complex signal

int triangleSignal(uint64_t t, int msQuartPeriod) {
  static uint64_t startTic = 0;
  int dTic = t - startTic;
  int pwm = 0;
  if (dTic < ticConvert(msQuartPeriod))
    pwm = ramp(dTic, 0, 0, 1000, ticConvert(msQuartPeriod));
  else if (dTic < (ticConvert(msQuartPeriod) * 3))
    pwm = ramp(dTic, 1000, ticConvert(msQuartPeriod), -1000, ticConvert(msQuartPeriod) * 3);
  else if (dTic < (ticConvert(msQuartPeriod) * 4))
    pwm = ramp(dTic, -1000, ticConvert(msQuartPeriod) * 3, 0, ticConvert(msQuartPeriod) * 4);
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

#define t0 ticConvert(100)       // waiting start
#define t1 ticConvert(100) + t0  // Rise Ramp
#define t2 ticConvert(1000) + t1 // High set
#define t3 ticConvert(100) + t2  // falling Ramp
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

  if (dTic <= t0) {
    pwmRapidShot = 0;
  } else if (dTic <= t1) {
    // rising ramp
    pwmRapidShot = ramp(dTic, 0, t0, 1000, t1);
  } else if (dTic <= t2) {
    pwmRapidShot = 1000;
  } else if (dTic <= t3) {
    // falling ramp
    pwmRapidShot = ramp(dTic, 1000, t2, 0, t3);
  } else if (dTic <= t4) {
    pwmRapidShot = 0;
  }

  return pwmRapidShot;
}

#define upLimitSat 224  // 223 from matlab graph
#define downLimitSat 39 // 40 from matlab graph

#define t0Eps ticConvert(300)         // waiting start
#define t1Eps ticConvert(300) + t0Eps // Rise Ramp
#define t2Eps ticConvert(300) + t1Eps // High set
#define t3Eps ticConvert(300) + t2Eps // falling Ramp
#define t4Eps tQuiet + t3Eps          // 0 set

int rapidShotEps(uint64_t t) {
  static uint64_t startTic = 0;
  static int pwmRapidShot = 0;
  static int pwmAdd = 1;
  long dTic = t - startTic;

  if (dTic > t4Eps) {
    startTic = t;
    pwmRapidShot = 0;
    dTic = t - startTic;
  }

  if (dTic <= t0Eps) {
    pwmRapidShot = 0;
  } else if (dTic <= t1Eps) {
    // rising ramp
    pwmRapidShot = min(255, pwmRapidShot + 1);
  } else if (dTic <= t2Eps) {
    pwmRapidShot = 255;
  } else if (dTic <= t3Eps) {
    // falling ramp
    pwmRapidShot = max(0, pwmRapidShot - 1);
  } else if (dTic <= t4Eps) {
    pwmRapidShot = 0;
  }

  return pwmRapidShot;
}

doubleIntCTRL::doubleIntCTRL() : doubleIntCTRL(0.8, 0.05) {}
doubleIntCTRL::doubleIntCTRL(float k, float dk) {
  this->k = k;
  this->dk = dk;
  setNewRef(0, 0);
}
void doubleIntCTRL::setNewRef(uint64_t ticSet, int v2AdcNewRef) {
  V2currRef = v2AdcNewRef;
  tcStart = ticSet;
  tcEnd = ticConvert(500) + tcStart;
  ticSatCount = 0;
  stateReset(0);
}

int doubleIntCTRL::ctrlStep(uint64_t t, int v2Adc) {

  if (t < tcStart || t > tcEnd || ticSatCount >= ticConvert(50))
    return 0;
  if (V2currRef == 0)
    return 0;
  long e = (V2currRef - v2Adc);

  dIntegral1 = dIntegral1 + dIntegral2;
  dIntegral2 = dIntegral2 + e;

  int pwmCtrl = (int)((dk * (float)dIntegral1 + k * (float)dIntegral2));
  pwmCtrl = constrain(pwmCtrl, -1000, 1000);
  if (abs(pwmCtrl) == 1000) {
    ticSatCount++;
    stateReset(pwmCtrl);
  } else
    ticSatCount = 0;
  return pwmCtrl;
}
void doubleIntCTRL::stateReset(int setOutput) {
  float rap = (float)dIntegral1 / (float)dIntegral2;
  dIntegral2 = (int)((float)setOutput / (rap * dk + k));
  dIntegral1 = (int)(rap * (float)dIntegral1);
}
