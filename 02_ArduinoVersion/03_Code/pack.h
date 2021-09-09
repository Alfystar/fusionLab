#ifndef PACK_H
#define PACK_H
#include <stdint.h>

struct _packLinux2Ard {
  int16_t newRef;
} __attribute__((packed));
typedef struct _packLinux2Ard packLinux2Ard;

struct _packArd2Linux {
  int16_t pwm;
  int16_t V2_read;
  int16_t Isense_read;
} __attribute__((packed));
typedef struct _packArd2Linux packArd2Linux;

#endif
