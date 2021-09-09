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

  std::ofstream outfile("test.txt");
  outfile << "my text here!" << std::endl;
  outfile.close();

  packArd2Linux pRead;
  //  packLinux2Ard pWrite {0, "Hoy Arduino"};
  struct timespec now, old, diff;
  clock_gettime(CLOCK_MONOTONIC_RAW, &old);



  while (true) {
    uart->getData_wait(&pRead);
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    timeSpecSub(now, old, diff);
    old = now;

    timeSpecPrint(diff, "diff");
  }
  delete uart;
  return 0;
}
