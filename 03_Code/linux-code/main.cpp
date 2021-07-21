//
// Created by alfy on 13/07/21.
//
#include "../pack.h"
#include <MP_Uart.h>
#include <iostream>
#include <unistd.h>

int sign = 1;
void controller(packLinux2STM *pWrite, packSTM2Linux *pRead){
  if(abs(pWrite->pwm) > 399)
    sign *= -1;
  pWrite->pwm+= sign;
  pWrite->pwm = -20;
}

int main(int argc, char *argv[]) {
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

  auto *uart = new EMP::MP_Uart<packSTM2Linux , packLinux2STM , LinuxMP_ConfMed(true)>(list[chose], B115200,false);
  sleep(1); // Device can be reset after the connection
  packSTM2Linux pRead;
  packLinux2STM pWrite{0};
  struct timespec now, old, diff;

  clock_gettime(CLOCK_MONOTONIC_RAW, &old);
  while (true) {
//    std::cout << "Waiting pack..." << endl;
    uart->getData_wait(&pRead);
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    timeSpecSub(now, old, diff);
    old = now;

//    std::cout << "Sending response pack..." << endl;
    controller(&pWrite,&pRead);
    uart->packSend(&pWrite);
    usleep(40000);
    std::cout << "Pack send: {pwm = " << pWrite.pwm << "}|\t|";
    std::cout << "Pack recive: {A1_read = " << pRead.A1_read << "\tV2_read = " << pRead.V2_read <<  "\tpwm = " << pRead.pwm << "}|\t|";
    timeSpecPrint(diff, "diff");
  }
  delete uart;
  return 0;
}
