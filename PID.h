/*
 * PID.h
 *
 *  Created on: Nov 5, 2014
 *      Author: adrian
 */

#ifndef PID_H_
#define PID_H_

typedef struct {
	systime_t lastTime;
	float ITerm;
	float lastInput;
	float outMin;
	float outMax;
	float kp;
	float ki;
	float kd;
} PIDstructure;

#define DIRECT 0
#define REVERSE 1
#define SAMPLE_TIME_S 0.1f // seconds

void Compute(float input, float* output, float setPoint, PIDstructure* pidStruct);
float map(float x, float in_min, float in_max, float out_min, float out_max);

#endif /* PID_H_ */
