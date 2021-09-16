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
struct setUpPack pMean;

#define offsetCalc(pin, n)                                                                                             \
  ({                                                                                                                   \
    long read = 0;                                                                                                     \
    for (int i = 0; i < 1 << n; i++) {                                                                                 \
      read += analogRead(pin);                                                                                         \
      delay(1);                                                                                                        \
    }                                                                                                                  \
    read = read >> 5;                                                                                                  \
    read;                                                                                                              \
  })

void setup() {

  mpSerial.begin(2000000);

  memset(&pRead, 0, sizeof(pRead));
  memset(&pWrite, 0, sizeof(pWrite));

  pinMode(13, OUTPUT);

  // Motori
  setMotFreq(hz30k);
  mot = new DCdriver(enPwm, inA, inB);

  // Wait start Request
  mpSerial.getData_wait(&pRead);

  pWrite.type = setUpPackType;
  // Calculate offset
  //  long read = 0;
  //  for (int i = 0; i < 1 << 5; i++)
  //    read += analogRead(V2);
  //  read = read >> 5;
  //  pWrite.setUp.V2_mean = read;
  pWrite.setUp.V2_mean = offsetCalc(V2, 5);
  //  read = 0;
  //  for (int i = 0; i < 1 << 5; i++)
  //    read += analogRead(Isense);
  //  read = read >> 5;
  //  pWrite.setUp.Isense_mean = read;
  pWrite.setUp.Isense_mean = offsetCalc(Isense, 5);

  pWrite.setUp.dt = dtExperiment;
  pWrite.setUp.V2Ref_set = V2Ref;

  pMean = pWrite.setUp;
  mpSerial.packSend(&pWrite, sizeof(pWrite.type) + sizeof(pWrite.setUp));

  // Start Delay
  pWrite.type = sampleType;
  delay(1000);

  mpSerial.bufClear();
  periodicTask(pWrite.setUp.dt);
  sei();
}

volatile u32 oldTic = tic;

void loop() {
  mpSerial.updateState();
  while (tic == oldTic) {
    mpSerial.updateState();
  }
  if (mpSerial.dataAvailable()) {
    mot->freeRun();
    while (true) {
      delay(1000);
    };
  }
  digitalWrite(13, !digitalRead(13));
  pWrite.read.V2_read = analogRead(V2);
  pWrite.read.Isense_read = analogRead(Isense);
  pWrite.read.pwm = mot->drive_motor(controll(&pMean, &pWrite.read, oldTic));

  //  pWrite.read.pwm = mot->actuate(rapidShotEps(oldTic)); //controllo diretto di rapidShotEps
  oldTic++; // Suppose no over time

  mpSerial.packSend(&pWrite, sizeof(pWrite.type) + sizeof(pWrite.read));
}
