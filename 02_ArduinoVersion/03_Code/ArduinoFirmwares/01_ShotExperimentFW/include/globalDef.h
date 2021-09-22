/*
 * globalDef.h
 *
 *  Created on: 06 dic 2019
 *      Author: alfyhack
 */

#ifndef MYLIB_GLOBALDEF_H_
#define MYLIB_GLOBALDEF_H_

// set timer divisor to     1 for PWM frequency of 31372.55 Hz
// set timer divisor to     8 for PWM frequency of  3921.16 Hz
// set timer divisor to    64 for PWM frequency of   490.20 Hz
// set timer divisor to   256 for PWM frequency of   122.55 Hz
// set timer divisor to  1024 for PWM frequency of    30.64 Hz
enum pwmFreq
	: char {
		hz30, hz120, hz490, hz4k, hz30k
};
#define dtExperiment 500UL // us
#define sign(x) (((x) > 0) ? (1) : (((x) < 0) ? (-1) : (0)))

// AnalogRead
#define V2 A0
#define Isense A1

// H-bridge Define
#define enPwm 5
#define inA 6
#define inB 7


#endif /* MYLIB_GLOBALDEF_H_ */
