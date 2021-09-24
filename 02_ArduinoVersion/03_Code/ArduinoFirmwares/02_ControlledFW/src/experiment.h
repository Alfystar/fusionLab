//
// Created by alfy on 10/09/21.
//

#ifndef INC_01_TESTFW_SRC_EXPERIMENT_H
#define INC_01_TESTFW_SRC_EXPERIMENT_H
#include "globalDef.h"
#include "pack-Controll.h"
#include <Arduino.h>


#define V2Ref volt2adc(0.5)

/// Base Signal
int ramp(uint64_t t, int vStart, uint64_t tStart, int vEnd, uint64_t tEnd);
int rampEps(uint64_t t, int vStart, uint64_t tStart, int vEnd, unsigned int eps);

/// Complex signal
int triangleSignal(uint64_t t, int msQuartPeriod);
int rapidShot(uint64_t t);
int rapidShotEps(uint64_t t);

/// Controll
int ctrl(uint64_t t, int v2);

class doubleIntCTRL {
  // Controllo
  uint64_t tcStart = 0; // start Experiment
  uint64_t tcEnd = 0;   // stop Experiment

#define vScale (5.0 / 1023.0)
  float kp = 0.0;
  float k1 = 0.8;
  float k2 = 0.05;

  int V2currRef = 0;
  long dIntegral2 = 0;
  long dIntegral1 = 0;
  unsigned int ticSatCount = 0;

public:
  int lastCtrl;
  int lastErr;
  doubleIntCTRL();
  doubleIntCTRL(float kp, float k1, float k2);

  void setNewRef(uint64_t ticSet, int v2AdcNewRef); //v2AdcNewRef senza offset
  int ctrlStep(uint64_t t, int v2Adc); // v2Add già senza offset

private:
  void stateReset(int setOutput);
};
#endif // INC_01_TESTFW_SRC_EXPERIMENT_H
