#include "globalDef.h"
#include <Motor/DCdriver.h>
// Message Pack
#include "pack-Controll.h"
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

doubleIntCTRL Ctrl = doubleIntCTRL();

void serialExe(packLinux2Ard *p){
  packArd2Linux send;
  switch (p->type) {
  case newRefType:
    Ctrl.setNewRef(tic,p->ref.newRef);
    break;
  case askType:
    send.type=setUpPackType;
    send.setUp=pMean;
    mpSerial.packSend(&send,sizeof(pWrite.type) + sizeof(pWrite.setUp));
    break;
  }
}


void setup() {

  mpSerial.begin(2000000);

  memset(&pRead, 0, sizeof(pRead));
  memset(&pWrite, 0, sizeof(pWrite));

  pinMode(13, OUTPUT);

  // Motori
  setMotFreq(hz30k);
  mot = new DCdriver(enPwm, inA, inB);

  // Mean find
  pMean.V2_mean = offsetCalc(V2, 5);
  pMean.Isense_mean = offsetCalc(Isense, 5);
  pMean.dt = dtExperiment;

  // Ctrl Reference
  Ctrl.setNewRef(tic, 0);

  // ################# Start Experiment #################
  mpSerial.bufClear();
  // Start Delay
  memset(&pWrite,0,sizeof(pWrite));
  pWrite.type = sampleType;
  delay(1000);
  periodicTask(pMean.dt);
  sei();
}

volatile u32 oldTic = tic;
int readData;
int pwm;
void loop() {
  mpSerial.updateState();
  do {
    readData = mpSerial.getData_try(&pRead);
    if (readData >= 0) {
      serialExe(&pRead);
    }
  } while (tic == oldTic);

  digitalWrite(13, !digitalRead(13));
  pWrite.read.V2_read = analogRead(V2);
  pWrite.read.Isense_read = analogRead(Isense);
  pwm = mot->drive_motor(Ctrl.ctrlStep(oldTic, pWrite.read.V2_read - pMean.V2_mean));
  pWrite.read.pwm = pwm;
  pWrite.read.err = Ctrl.lastErr;

  oldTic++; // Suppose no over time
  mpSerial.packSend(&pWrite, sizeof(pWrite.type) + sizeof(pWrite.read));
}
