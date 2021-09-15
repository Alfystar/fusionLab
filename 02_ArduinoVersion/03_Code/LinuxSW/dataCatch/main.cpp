//
// Created by alfy on 04/05/21.
//
#include "pack.h"

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
  packLinux2Ard pWrite{1}; // End Pack
  uart->packSend(&pWrite);
  uart->packSend(&pWrite);
  sleep(1);
  exit(-1);
}

int main(int argc, char *argv[]) {
  uartOpen();
  signal(SIGINT, intHandler);

  std::ofstream outfile("capture.txt");

  packArd2Linux pRead;
  packLinux2Ard pWrite{0}; // Start Pack
  struct timespec now, old, diff;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old);

  uart->packSend(&pWrite);
  uart->getData_wait(&pRead);
  outfile << "V2_mean\tIsense_mean\tdt\tV2Ref_set" << std::endl;
  outfile << pRead.setUp.V2_mean << "\t"
          << pRead.setUp.Isense_mean << "\t"
          << pRead.setUp.dt << "\t"
          << pRead.setUp.V2Ref_set
          << std::endl;
  outfile << "PWM\tV2_read\tIsense_read" << std::endl;

  while (true) {
    uart->getData_wait(&pRead);
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    timeSpecSub(now, old, diff);
    old = now;
    outfile << pRead.read.pwm << "\t" << pRead.read.V2_read << "\t" << pRead.read.Isense_read << std::endl;
    timeSpecPrint(diff, "diff");
  }
  outfile.close();
  delete uart;
  return 0;
}
