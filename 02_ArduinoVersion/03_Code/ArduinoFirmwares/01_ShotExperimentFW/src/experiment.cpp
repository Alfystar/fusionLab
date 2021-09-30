//
// Created by alfy on 10/09/21.
//

#include "experiment.h"

int controll(struct setUpPack *pMean, struct sample *pRead, unsigned long tic) {
  //  return triangleSignal(tic, 200);
  //  return rapidShot(tic);
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

// Onda quadra a 3 stati, Delay, High, Zero, NegHigh
int square3StateDHZN(uint64_t dt, int msDelay, int msHigh, int msZero, int msNegHigh) {
  if (dt < ticConvert(msDelay))
    return 0;
  else if (dt < ticConvert(msDelay + msHigh))
    return 255;
  else if (dt < ticConvert(msDelay + msHigh + msZero))
    return 0;
  else if (dt < ticConvert(msDelay + msHigh + msZero + msNegHigh))
    return -255;
  else
    return 0;
}

// Onda quadra a 3 stati, Zero, High, NegHigh
int square3StateZHN(uint64_t dt, int msZero, int msHigh, int msNegHigh) {
  if (dt < ticConvert(msZero))
    return 0;
  else if (dt < ticConvert(msZero + msHigh))
    return 255;
  else if (dt < ticConvert(msZero + msHigh + msNegHigh))
    return -255;
  else
    return 0;
}

/// Periodic signal

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

int triangleSignalEps(uint64_t t) {
  static int pwm = 0;
  static int add = 1;
  pwm = constrain(pwm + add, -255, 255);
  if (abs(pwm) == 255)
    add *= -1;
  return pwm;
}

// Rapid Shot

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

int estimateSignalSquare(uint64_t t, int msZero, int msHigh, int msNegHigh) {
  static uint64_t startTic = 0;
  unsigned int dt = (t - startTic);
  if (dt < ticConvert(msZero + msHigh + msZero + msNegHigh))
    return square3StateDHZN(dt, msZero, msHigh, msZero, msNegHigh);
  else if (dt < ticConvert(msZero + msHigh + msZero + msNegHigh + msZero + msHigh + msNegHigh))
    return square3StateZHN(dt, msZero, msHigh, msNegHigh);
  else {
    startTic = t;
    return estimateSignalSquare(t, msZero, msHigh, msNegHigh);
  }
}

int halfFlop(uint64_t t, int msFlop){
  static uint64_t startTic = 0;
  static int pwm = 115;
  unsigned int dt = (t - startTic);
  if (dt < ticConvert(msFlop))
    return pwm;
  else {
    startTic = t;
    pwm *= -1;
    return halfFlop(t, msFlop);
  }
}


// Controllo
#define tcStart ticConvert(100)         // start Experiment
#define tcEnd ticConvert(150) + tcStart // stop Experiment

#define vScale (5.0 / 1023.0)
//// K Test0
//#define kp 0.0
//#define k1 0.8
//#define k2 0.05

// K Test1
#define kp 0.0
#define k1 0.55
#define k2 0.2

// K Test2
//#define kp 0.3
//#define k1 0.7
//#define k2 0.4

//// K Test3
//#define kp 0.8
//#define k1 1.0
//#define k2 0.8

long dIntegral2 = 0;
long dIntegral1 = 0;

// Driver motor take value from -1000 to 1000
int ctrl(uint64_t t, int v2) {
  if (t < tcStart || t > tcEnd)
    return 0;
  long e = (V2Ref - v2); // * vScale;

  dIntegral2 = dIntegral2 + dIntegral1;
  dIntegral1 = dIntegral1 + e;

  int pwmCtrl = (int)((e * kp + k1 * dIntegral1 + k2 * dIntegral2));
  pwmCtrl = constrain(pwmCtrl, -1000, 1000);
  return pwmCtrl;
}