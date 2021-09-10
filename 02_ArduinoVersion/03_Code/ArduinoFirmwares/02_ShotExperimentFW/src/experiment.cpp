//
// Created by alfy on 10/09/21.
//

#include "experiment.h"

// Have to set pwm in pWrite
int controll(packArd2Linux *pWrite, unsigned long tic) {

  //  pWrite->read.pwm = ramp(tic);
  pWrite->read.pwm = rapidShot(tic);
  return pWrite->read.pwm;
}

//todo: riscrivere la rampa come una funzione che prende:
// Vstart, tStart
// Vend, tEnd
// e prima e dopo satura ai valori Vstart e Vend
int pwmRamp = 0;
int addRamp = 1;
int ramp(uint64_t t) {
  if (t % 10 == 0)
    pwmRamp += addRamp;
  if (abs(pwmRamp) > 255) {
    pwmRamp = (abs(pwmRamp) / pwmRamp) * 255;
    addRamp *= -1;
  }
}

//: fai una variazione molto più rapida del PWM:
// da 0 dufy cycle passi a 100% in 100ms, rimani su per 1 s e
//: poi giù in 0.1s e ripeti. Poi mi chiami.
#define ticConvert(ms) (ms * 1000UL) / dtExperiment

#define tQuiet ticConvert(1000)

#define t1 ticConvert(100)       // Rise Ramp
#define t2 ticConvert(1000) + t1 // High set
#define t3 ticConvert(100) + t2  // falling Ramp
#define t4 tQuiet + t3           // 0 set

#define addRampCalc(valStart, valEnd, msEnd) (valEnd - valStart) / msEnd
int pwmRapidShot = 0;
uint64_t startTic = 0;
int rapidShot(uint64_t t) {
  long dTic = t - startTic;
  if (dTic > t4) {
    startTic = t;
    pwmRapidShot = 0;
    dTic = t - startTic;
  }

  if (dTic <= t1) {
    pwmRapidShot += addRampCalc(0, 255, 100);
  } else if (dTic <= t2) {
    pwmRapidShot = 255;
  } else if (dTic <= t3) {
    // falling ramp
    pwmRapidShot += addRampCalc(255, 0, 100);
  }
  else if (dTic <= t4) {
    pwmRapidShot = 0;
  }

  return pwmRapidShot;
}