/*
 * PID.c
 *
 *  Created on: Nov 5, 2014
 *      Author: adrian
 */

#include "ch.h"
#include "PID.h"

void Compute(float input, float* output, float setPoint, PIDstructure* pidStruct)
{
	systime_t now = chTimeNow();	// system ticks
	systime_t timeChange = now - pidStruct->lastTime;	// system ticks

	if (timeChange >= S2ST(SAMPLE_TIME_S)) {
		/*Compute all the working error variables*/
		float error = setPoint - input;
		pidStruct->ITerm += (pidStruct->ki * error);

		if (pidStruct->ITerm > pidStruct->outMax)
			pidStruct->ITerm = pidStruct->outMax;
		else if (pidStruct->ITerm < pidStruct->outMin)
			pidStruct->ITerm = pidStruct->outMin;
		float dInput = (input - pidStruct->lastInput);

		/*Compute PID Output*/
		*output = pidStruct->kp * error + pidStruct->ITerm - pidStruct->kd * dInput;
		if (*output > pidStruct->outMax)
			*output = pidStruct->outMax;
		else if (*output < pidStruct->outMin)
			*output = pidStruct->outMin;

		/*Remember some variables for next time*/
		pidStruct->lastInput = input;
		pidStruct->lastTime = now;
	}
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
