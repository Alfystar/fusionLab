/*
 * L298N.h - Library for L298N motor driver
 Created by Yohendry Hurtado, 28 dec 2014
 Released into the public domain.

 Created on: 25 mar 2019
 *      Author: Yohendry Hurtado, ADAPTION FOR PROJECT BY ALFY
 */
#include "DCdriver.h"

void setMotFreq(pwmFreq freq) {
// TCCR0B is for Timer 0
#define myTimer TCCR0B
  switch (freq) {
    // set timer 3 divisor to  1024 for PWM frequency of    30.64 Hz
  case hz30:
    myTimer = (myTimer & B11111000) | B00000101;
    break;
  case hz120:
    // set timer 3 divisor to   256 for PWM frequency of   122.55 Hz
    myTimer = (myTimer & B11111000) | B00000100;
    break;
  case hz490:
    // set timer 3 divisor to    64 for PWM frequency of   490.20 Hz
    myTimer = (myTimer & B11111000) | B00000011;

    break;
  case hz4k:
    // set timer 3 divisor to     8 for PWM frequency of  3921.16 Hz
    myTimer = (myTimer & B11111000) | B00000010;
    break;
  case hz30k:
    // set timer 3 divisor to     1 for PWM frequency of 31372.55 Hz
    myTimer = (myTimer & B11111000) | B00000001;
    break;
  default:
    setMotFreq(hz4k);
    break;
  }
#undef myTimer
}

DCdriver::DCdriver(byte ena, byte in1, byte in2) {
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  this->in1 = in1;
  this->in2 = in2;
  this->pwm = ena;
  this->state = free_Mot;
  this->delay_time = 0;
  this->time = 0;
  this->speed = 0;
  this->freeRun();
#ifdef SERIAL_PRINT
  Serial.println("DCDriver create");
  delay(100);
#endif
}

void DCdriver::updateMot() {
  // enum {moving, H_brake, S_brake, wait};
  switch (this->state) {
  case moving:
    // do notting
    break;
  case movingTiming:
    if (millis() > this->time + this->delay_time)
      this->hard_stop(100);
    break;
  case H_brake:
    if (millis() > this->time + this->delay_time)
      this->soft_stop(1000);
    break;
  case S_brake:
    if (millis() > this->time + this->delay_time)
      this->freeRun();
    break;
  case alwaysBrake:
    break;
  case free_Mot:
  default:
    break;
  }
}

int DCdriver::actuate(int pwm){
  this->speed = pwm;
  this->state = moving;
  if (this->speed < 0) {
    this->anticlockwise();
    analogWrite(this->pwm, -this->speed);
  } else {
    this->clockWise();
    analogWrite(this->pwm, this->speed);
  }
  return this->speed;
}

long map_f(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (long)((x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min);
}

// speed +-1000
int DCdriver::drive_motor(int speed) {
  if(speed == 0)
    this->speed = 0;
  else if(abs(speed) >= 1000)
    this->speed = sign(speed) * 255;
  else
    this->speed = sign(speed) * map_f(abs(speed),1,999,downLimitSat,upLimitSat);
  return this->actuate(this->speed);
}

int DCdriver::drive_motor(int speed, unsigned int delay_time) {
  this->drive_motor(speed);
  this->delay_time = delay_time;
  this->time = millis();
  this->state = movingTiming;
  return this->speed;
}

void DCdriver::reversDir() { drive_motor(-this->speed, 500); }

void DCdriver::hard_stop(unsigned int delay_time) {
  this->soft_stop(delay_time);
  /*
   * Commentato per sicurezza visto il driver provvisorio
  this->delay_time = delay_time;
  this->time = millis();
  this->state = H_brake;
  this->setup_motor(HIGH, HIGH);
  digitalWrite(this->pwm, 1);
  */
}

void DCdriver::soft_stop() {
  this->state = alwaysBrake;
  this->setup_motor(LOW, LOW);
  digitalWrite(this->pwm, 1);
}

void DCdriver::soft_stop(unsigned int delay_time) {
  this->soft_stop();
  this->delay_time = delay_time;
  this->time = millis();
  this->state = S_brake;
}

void DCdriver::freeRun() {
  this->state = free_Mot;
  this->setup_motor(LOW, LOW);
  digitalWrite(this->pwm, 0);
}

void DCdriver::setup_motor(byte in1, byte in2) {
  digitalWrite(this->in1, in1);
  digitalWrite(this->in2, in2);
}

void DCdriver::clockWise() { this->setup_motor(1, 0); }

void DCdriver::anticlockwise() { this->setup_motor(0, 1); }

/*per futuro upgrade a porte*/
void set_bits_func_correct(volatile uint8_t *port, uint8_t mask) {
  *port |= mask;
}
