#include "globalDef.h"
#include <Arduino.h>
#include <Motor/DCdriver.h>
// Message Pack
#include "pack.h"
#include <MP_Serial.h>

// Controll Function
#include "experiment.h"

void periodicTask(int time) {
  // time in micro secondi!!!!
  // Tempo massimo con prescaler a 1024: 16,32ms -- Time Definition 0.064ms
  //  Initial TIMER2 Fast PWM
  TCCR2A = (0x0 << COM2A0) | (0x0 << COM2B0) | (0x2 << WGM20); // non collegato pin pwm, motalità CTC
  TCCR2B = (0 << WGM22) | (0x6 << CS20);                       // Modalità CTC, Prescalere 256
  // T_cklock * Twant / Prescaler = valore Registro
  OCR2A = (int)(16UL * time / 256);
  TIMSK2 = (1 << OCIE2A); // attivo solo l'interrupt di OC2A

//  Serial.print("(dt=");
//  Serial.print(time);
//  Serial.print(", OCR2A=");
//  Serial.print(OCR2A);
//  Serial.println(", Prescaler=256)");
}

volatile u32 tic = 0;

ISR(TIMER2_COMPA_vect) { tic++; }

using namespace EMP;

MP_Serial<packLinux2Ard, packArd2Linux, ArduinoMP_templateDefault()> mpSerial(Serial);
DCdriver *mot;

packLinux2Ard pRead;
packArd2Linux pWrite;

void setup() {

  mpSerial.begin(2000000);
  memset(&pRead, 0, sizeof(pRead));
  memset(&pWrite, 0, sizeof(pWrite));

  pinMode(13, OUTPUT);

  // Motori
  setMotFreq(hz4k);
  mot = new DCdriver(enPwm, inA, inB);

  // Wait start Request
  mpSerial.getData_wait(&pRead);

  pWrite.type = meanOffsetType;
  // Calculate offset
  long read = 0;
  for (int i = 0; i < 1 << 5; i++)
    read += analogRead(V2);
  read = read >> 5;
  pWrite.mean.V2_mean = read;

  read = 0;
  for (int i = 0; i < 1 << 5; i++)
    read += analogRead(Isense);
  read = read >> 5;
  pWrite.mean.Isense_mean = read;

  pWrite.mean.dt = dtExperiment;
  mpSerial.packSend(&pWrite, sizeof(pWrite.type) + sizeof(pWrite.mean));

  // Start Delay
  pWrite.type = sampleType;
  delay(1000);

  periodicTask(pWrite.mean.dt);
  sei();
}

volatile u32 oldTic = tic;

void loop() {
  while (tic == oldTic)
    ;
  digitalWrite(13, !digitalRead(13));
  oldTic = tic;
  pWrite.read.V2_read = analogRead(V2);
  pWrite.read.Isense_read = analogRead(Isense);
  mot->drive_motor(controll(&pWrite,oldTic));

  mpSerial.packSend(&pWrite, sizeof(pWrite.type) + sizeof(pWrite.read));

}