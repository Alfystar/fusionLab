#include "globalDef.h"
#include <Arduino.h>
#include <Motor/DCdriver.h>
// Message Pack
#include "pack.h"
#include <MP_Serial.h>

void periodicTask(int time) {
  // time in micro secondi!!!!
  // Tempo massimo con prescaler a 1024: 16,32ms -- Time Definition 0.064ms
  //  Initial TIMER2 Fast PWM
  TCCR2A = (0x0 << COM2A0) | (0x0 << COM2B0) | (0x2 << WGM20); // non collegato pin pwm, motalità CTC
  TCCR2B = (0 << WGM22) | (0x6 << CS20);                       // Modalità CTC, Prescalere 256
  // T_cklock * Twant / Prescaler = valore Registro
  OCR2A = (int)(16UL * time / 256);
  TIMSK2 = (1 << OCIE2A); // attivo solo l'interrupt di OC2A
  Serial.print("(dt=");
  Serial.print(time);
  Serial.print(", OCR2A=");
  Serial.print(OCR2A);
  Serial.println(", Prescaler=256)");
}

volatile u32 tic = 0;

ISR(TIMER2_COMPA_vect) { tic++; }


using namespace EMP;

MP_Serial<packLinux2Ard, packArd2Linux, ArduinoMP_templateDefault()> mpSerial(Serial);
DCdriver *mot;

packLinux2Ard pRead;

void setup() {
  // write your initialization code here
//   Serial.begin(2000000);
  mpSerial.begin(2000000);

  pinMode(13, OUTPUT);

  // Motori
  setMotFreq(hz4k);
  mot = new DCdriver(enPwm, inA, inB);


  mpSerial.getData_wait(&pRead);
  // Start Delay
  delay(1000);

  periodicTask(1000);
  sei();
}

volatile u32 oldTic = tic;
int V2_read, Isense_read;
int pwm = 0;
int add = 1;

packArd2Linux pWrite;

void loop() {
  while (tic == oldTic)
    ;
  digitalWrite(13, !digitalRead(13));
  oldTic = tic;
  pWrite.V2_read = analogRead(V2);
  pWrite.Isense_read = analogRead(Isense);

  if (oldTic % 10 == 0)
    pwm += add;
  if (abs(pwm) > 255) {
    pwm = (abs(pwm) / pwm) * 255;
    add *= -1;
  }
  mot->drive_motor(pwm);
  pWrite.pwm = pwm;

  mpSerial.packSend(&pWrite);

  // write your code here
}