#ifndef PACK_H
#define PACK_H
#include <stdint.h>

struct _packLinux2Ard {
  int16_t newRef;
} __attribute__((packed));
typedef struct _packLinux2Ard packLinux2Ard;

enum ardSendType : uint8_t { sampleType, meanOffsetType };
struct sample {
  int16_t pwm;
  int16_t V2_read;
  int16_t Isense_read;
} __attribute__((packed));

struct meanOffset {
  int16_t V2_mean;     // Adc read
  int16_t Isense_mean; // Adc read
  int16_t dt;          // Time in us (10^-6)
} __attribute__((packed));

struct _packArd2Linux {
  ardSendType type;
  union {
    struct sample read;
    struct meanOffset mean;
  };
} __attribute__((packed));
typedef struct _packArd2Linux packArd2Linux;

#endif
