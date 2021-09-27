#ifndef PACK_H
#define PACK_H
#include <stdint.h>
// #######################################################
// ########        Companion to Arduino           ########
// #######################################################
struct newRef {
	int16_t newRef;
} __attribute__((packed));

struct setUpPackAsk {
	int8_t padding;
} __attribute__((packed));

enum LinuxSendType : uint8_t { newRefType, askType };

struct _packLinux2Ard {
	LinuxSendType type;
	union {
		struct newRef ref;
		struct setUpPackAsk ask;
	};
} __attribute__((packed));
typedef struct _packLinux2Ard packLinux2Ard;
// #######################################################
// ########        Arduino to Companion           ########
// #######################################################
struct sample {
	int16_t pwm;
	int16_t V2_read;
	int16_t Isense_read;
	int16_t err;
} __attribute__((packed));

struct setUpPack {
	int16_t V2_mean;     // Adc read
	int16_t Isense_mean; // Adc read
	int16_t dt;          // Time in us (10^-6)
} __attribute__((packed));
enum ardSendType : uint8_t { sampleType, setUpPackType };

struct _packArd2Linux {
	ardSendType type;
	union {
		struct sample read;
		struct setUpPack setUp;
	};
} __attribute__((packed));
typedef struct _packArd2Linux packArd2Linux;

// Common define
#define volt2adc(x) ((int)(((x) / (5.0 / (1023)) + 0.5)))
#define dtExperiment 500UL // us
#define ticConvert(ms) ((ms * 1000UL) / dtExperiment)

#endif
