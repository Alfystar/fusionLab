/*
 * L298N.h - Library for L298N motor driver
 Created by Yohendry Hurtado, 28 dec 2014
 Released into the public domain.

 Created on: 25 mar 2019
 *      Author: Yohendry Hurtado, ADAPTION FOR PROJECT BY ALFY
 */

#ifndef DCdriver_h
#define DCdriver_h

#include <Arduino.h>
#include <globalDef.h>

#define upLimitSat 220  // 219.9 from matlab graph
#define downLimitSat 120 // 40.8 from matlab graph

void setMotFreq(pwmFreq freq);

enum motState { moving, movingTiming, H_brake, S_brake, alwaysBrake, free_Mot };

class DCdriver {
public:
  DCdriver(byte ena, byte in1, byte in2);
  void updateMot(); // no stopping Call, to update status

  /*State change motor*/
  int actuate(int pwm);
  int drive_motor(int speed);
  int drive_motor(int speed, unsigned int delay_time);
  void reversDir();
  void soft_stop();
  void soft_stop(unsigned int delay_time);
  void hard_stop(unsigned int delay_time); // MAKE HARD STOP FOR DELAYED TIME
  void freeRun();

protected:
  byte in1, in2, pwm;
  motState state;
  int speed;
  unsigned int delay_time;
  unsigned long time;

private:
  void clockWise();
  void anticlockwise();
  void setup_motor(byte in1, byte in2);
};

#endif // DCdriver_h
