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

/// Control Class
#define Ts (dtExperiment / 1000000.0) // second

class iiCTRL {
  // Saturation showdown experiment
  unsigned int ticSatCount = 0;
  // Adc Current Voltage reference
  int V2currRef = 0;

  float kp = 0.0;
  float k1 = 0.8;
  float k2 = 0.05;

  // State Space Cii(s)
  float xCii1 = 0, xCii2 = 0;
  // State Space Ci(s)
  float xCi1 = 0;
public:
  int lastCtrl;
  int lastErr;
  iiCTRL();
  iiCTRL(float k2, float k1, float kp);

  // State update
  void setNewRef(uint64_t ticSet, int v2AdcNewRef); // v2AdcNewRef senza offset
  int ctrlStep(uint64_t t, int v2Adc);              // v2Add gia' senza offset

  // For switching logic
  void changeK(float k2, float k1, float kp);
  void changeK2(float k2);
  void changeK1(float k1);
  void changeKp(float kp);

private:
  void stateReset(int setOutput);
};
#endif // INC_01_TESTFW_SRC_EXPERIMENT_H
