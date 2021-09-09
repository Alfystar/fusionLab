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

EMP::MP_Uart<packArd2Linux, packLinux2Ard, LinuxMP_ConfMed(true)> *uart;
void uartOpen(){
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


int main(int argc, char *argv[]) {
  uartOpen();

  std::ofstream outfile("capture.txt");

  packArd2Linux pRead;
  packLinux2Ard pWrite {0}; // Start Pack
  struct timespec now, old, diff;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old);

  uart->packSend(&pWrite);
  uart->getData_wait(&pRead);
  outfile << "V2_mean\tIsense_mean" << std::endl;
  outfile << pRead.mean.V2_mean << "\t" << pRead.mean.Isense_mean << std::endl;
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
