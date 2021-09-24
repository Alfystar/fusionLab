//
// Created by alfy on 04/05/21.
//
#include "pack-Controll.h"

#include <MP_Uart.h>
#include <iostream>
#include <unistd.h>

// using ofstream constructors.
#include <fstream>
#include <iostream>

// Signal include
#include <signal.h>

EMP::MP_Uart<packArd2Linux, packLinux2Ard, LinuxMP_ConfMed(true)> *uart;
void uartOpen() {
  std::cout << "Uart Start" << std::endl;
  auto list = EMP::UartDeviceList();
  if (list.empty()) {
    cout << "No device Detected" << endl;
    exit(-1);
  }
  int chose;
  if (list.size() == 1) {
    chose = 0;
    cout << "Connected with: " << list[0] << endl;

  } else {
    cout << "Possible device detected:" << endl;
    for (int i = 0; i < list.size(); i++) {
      cout << "[" << i << "] = " << list[i] << "\t{" << EMP::UartDeviceInfo(list[i]) << "}" << endl;
    }
    cout << "Please chose the index: ";
    cin >> chose;
  }
  uart = new EMP::MP_Uart<packArd2Linux, packLinux2Ard, LinuxMP_ConfMed(true)>(list[chose], B2000000, false);
  sleep(1); // Device can be reset after the connection
}

void intHandler(int dummy) {
  cout << "\n Ctrl+c catch, send stop pack" << std::endl;
  packLinux2Ard pWrite; // End Pack
  pWrite.type = newRefType;
  pWrite.ref.newRef = 0;
  uart->packSend(&pWrite);
  uart->packSend(&pWrite);
  sleep(1);
  exit(-1);
}


enum refFunxType { BiFronte, Random, Triangolo };
packArd2Linux pRead;
packLinux2Ard pWrite;
struct setUpPack setUp;
struct timespec now, old, diff;
refFunxType refSelect;


void help() {
  cout << "I possibili comandi sono:\n"
       << "b  := BiFronte (default)\n"
       << "r  := Random\n"
       << "t  := Triangolo\n"
       << "h  := this guide\n";
}
int main(int argc, char *argv[]) {
  // ~~~~~~~~~~~ Init Block ~~~~~~~~~~~
  if (argc >= 2) {
    if (strcmp(argv[1], "b") == 0)
      refSelect = BiFronte;
    if (strcmp(argv[1], "r") == 0)
      refSelect = Random;
    if (strcmp(argv[1], "t") == 0)
      refSelect = Triangolo;
    if (strcmp(argv[1], "h") == 0) {
      help();
      exit(-1);
    }
  } else {
    help();
    refSelect = BiFronte;
  }

  uartOpen();
  signal(SIGINT, intHandler);

  std::ofstream outfile("capture.txt");

  memset(&pRead, 0, sizeof(pRead));
  memset(&pWrite, 0, sizeof(pWrite));

  clock_gettime(CLOCK_MONOTONIC_RAW, &old);

  // ~~~~~~~~~~~ Setting Ask ~~~~~~~~~~~
  pWrite.type = askType;
  pWrite.ask.padding = 0;
  uart->packSend(&pWrite, sizeof(LinuxSendType) + sizeof(struct setUpPackAsk));
  do {
    uart->getData_wait(&pRead);
  } while (pRead.type != setUpPackType);
  setUp = pRead.setUp;


  outfile << "V2_mean\tIsense_mean\tdt" << std::endl;
  outfile << pRead.setUp.V2_mean << "\t" << pRead.setUp.Isense_mean << "\t" << pRead.setUp.dt << std::endl;
  outfile << "PWM\tV2_read\tIsense_read\te" << std::endl;

  // ~~~~~~~~~~~ Reset Experiment ~~~~~~~~~~~

  pWrite.ref.newRef = volt2adc(0);
  uart->packSend(&pWrite);

  // ~~~~~~~~~~~ Setup Tic variable ~~~~~~~~~~~
  ulong ticExp = 0;
  ulong ticSat = 0;
  int swingSign = 1;
  while (true) {
    // ~~~~~~~~~~~ Sample wait ~~~~~~~~~~~
    do {
      uart->getData_wait(&pRead);
    } while (pRead.type != sampleType);
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    timeSpecSub(now, old, diff);
    old = now;

    // ~~~~~~~~~~~ Sample Store ~~~~~~~~~~~
    outfile << pRead.read.pwm << "\t" << pRead.read.V2_read << "\t" << pRead.read.Isense_read << "\t" << pRead.read.err
            << std::endl;
    timeSpecPrint(diff, "diff");
    ticExp++;

    // ~~~~~~~~~~~ Reference Logic ~~~~~~~~~~~
    switch (refSelect) {
    case BiFronte:
      if (ticExp > ticConvert(200)) {
        ticExp = 0;
        memset(&pWrite, 0, sizeof(pWrite));
        pWrite.type = newRefType;
        pWrite.ref.newRef = volt2adc(0.5) * swingSign;
        swingSign *= -1;

        uart->packSend(&pWrite, sizeof(LinuxSendType) + sizeof(struct newRef));
      }
      break;
    case Random:
      if (ticExp > ticConvert(200)) {
        ticExp = 0;
        memset(&pWrite, 0, sizeof(pWrite));
        pWrite.type = newRefType;
        float randRef = ((rand() % 1200) - 600) / 1000.0; // +- 0.6 ref
        pWrite.ref.newRef = volt2adc(randRef);

        uart->packSend(&pWrite, sizeof(LinuxSendType) + sizeof(struct newRef));
      }
      break;
    case Triangolo:
      if (ticExp == ticConvert(200)) {
        memset(&pWrite, 0, sizeof(pWrite));
        pWrite.type = newRefType;
        pWrite.ref.newRef = volt2adc(0.5) * swingSign;
        swingSign *= -1;
        uart->packSend(&pWrite, sizeof(LinuxSendType) + sizeof(struct newRef));
      }

      if (abs(pRead.read.pwm) == 255)
        ticSat++;
      else
        ticSat = 0;

      if (ticSat > ticConvert(50)) {
        ticSat = 0;
        memset(&pWrite, 0, sizeof(pWrite));
        pWrite.type = newRefType;
        pWrite.ref.newRef = volt2adc(0.5) * swingSign;
        swingSign *= -1;
        uart->packSend(&pWrite, sizeof(LinuxSendType) + sizeof(struct newRef));
      }
      break;
    }
  }
  outfile.close();
  delete uart;
  return 0;
}
