#ifndef packTokamak_h
#define packTokamak_h

#include <stdint.h>

struct _packLinux2STM {
  int16_t pwm;
} __attribute__((packed));
typedef struct _packLinux2STM packLinux2STM;

// Post elaborate pack
struct _packSTM2Linux {
  float V2_read;
  float A1_read;
} __attribute__((packed));
typedef struct _packSTM2Linux packSTM2Linux;


#endif //packTokamak_h
