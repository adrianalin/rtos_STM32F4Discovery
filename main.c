#include "ch.h"
#include "hal.h"
#include <math.h>

// my includes
#include "BluetoothCommunicationThread.h"
#include "accelgiro.h"
#include "PID.h"
#include "PWMsetup.h"

float ch1;	// throttle
float ch2;  // pitch
float ch3;
float ch4;
float ch5;

float ch1Last, ch2Last, ch3Last, ch4Last, velocityLast;

#define PITCH_MIN -30
#define PITCH_MAX 30
#define ROLL_MIN -30
#define ROLL_MAX 30
#define YAW_MIN -180
#define YAW_MAX 180
#define PID_PITCH_INFLUENCE 20
#define PID_ROLL_INFLUENCE 20
#define PID_YAW_INFLUENCE 20

#define RC_HIGH_CH1 1000
#define RC_LOW_CH1 2000
#define RC_HIGH_CH2 1000
#define RC_LOW_CH2 2000
#define RC_HIGH_CH3 1000
#define RC_LOW_CH3 2000
#define RC_HIGH_CH4 1000
#define RC_LOW_CH4 2000
#define RC_HIGH_CH5 1000
#define RC_LOW_CH5 2000
#define RC_ROUNDING_BASE 50

float ypr[3] = {0.0f,0.0f,0.0f};	// yaw pitch roll values
float yprLast[3] = {0.0f, 0.0f, 0.0f};

/*  Motor controll variables
 *
 */
int velocity;                          // global velocity
float bal_ac, bal_bd;                 // motor balances can vary between -100 & 100
float bal_axes;                       // throttle balance between axes -100:ac , +100:bd
int va, vb, vc, vd;                    //velocities
int v_ac, v_bd;                        // velocity of axes

/* ESC configuration
 *
 */
#define ESC_MIN 22
#define ESC_MAX 115
#define ESC_TAKEOFF_OFFSET 30

/*  PID configuration
 *
 */
#define PITCH_P_VAL 0.5f
#define PITCH_I_VAL 0.f
#define PITCH_D_VAL 1.f

#define ROLL_P_VAL 2.f
#define ROLL_I_VAL 5.f
#define ROLL_D_VAL 1.f

#define YAW_P_VAL 2.f
#define YAW_I_VAL 5.f
#define YAW_D_VAL 1.f

static WORKING_AREA(bluetoothCommunicationThread_WA, 128);

void initBalancing(void)
{
  bal_axes = 0;
  bal_ac = 0;
  bal_bd = 0;

}

void setTunings(PIDstructure* pidStruct, float P_val, float I_val, float D_val, int direction)
{
	I_val = I_val * SAMPLE_TIME_S;
	D_val = D_val / SAMPLE_TIME_S;

	if (direction == REVERSE) {
		pidStruct->kp = (0 - P_val);
		pidStruct->ki = (0 - I_val);
		pidStruct->kd = (0 - D_val);
	} else {
		pidStruct->kp = P_val;
		pidStruct->ki = I_val;
		pidStruct->kd = D_val;
	}
}

void setOutputLimits(PIDstructure* pidStruct, float min, float max)
{
	pidStruct->outMax = max;
	pidStruct->outMin = min;
}

void initRegulators(PIDstructure* yawStruct, PIDstructure* pitchStruct, PIDstructure* rollStruct)
{
	setOutputLimits(yawStruct, -PID_YAW_INFLUENCE, PID_YAW_INFLUENCE);
	setOutputLimits(pitchStruct, -PID_PITCH_INFLUENCE, PID_PITCH_INFLUENCE);
	setOutputLimits(rollStruct, -PID_ROLL_INFLUENCE, PID_ROLL_INFLUENCE);
}

/*  calculateVelocities function
 *
 *  calculates the velocities of every motor
 *  using the PID output
 */
void calculateVelocities(void)
{
	ch3 = floor(ch3 / RC_ROUNDING_BASE) * RC_ROUNDING_BASE;
	velocity = map(ch3, RC_LOW_CH3, RC_HIGH_CH3, ESC_MIN, ESC_MAX);

	if((velocity < ESC_MIN) || (velocity > ESC_MAX)) velocity = velocityLast;

	velocityLast = velocity;

	v_ac = (abs(-100 + bal_axes) / 100) * velocity;
	v_bd = ((100 + bal_axes) / 100) * velocity;

	va = ((100 + bal_ac) / 100) * v_ac;
	vb = ((100 + bal_bd) / 100) * v_bd;

	vc = (abs((-100 + bal_ac) / 100)) * v_ac;
	vd = (abs((-100 + bal_bd) / 100)) * v_bd;

	print("va: ");
	printn(va);
	println("");

	print("vb: ");
	printn(vb);
	println("");

	print(";  vc = ");
	printn(vc);
	println("");

	print(";  vd = ");
	printn(vd);
	println("");
	println("");

	if(velocity < ESC_TAKEOFF_OFFSET){

		va = ESC_MIN;
		vb = ESC_MIN;
		vc = ESC_MIN;
		vd = ESC_MIN;
	}
}

int main(void) {
	PIDstructure pitchStruct;
	PIDstructure rollStruct;
	PIDstructure yawStruct;

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	chThdCreateStatic(bluetoothCommunicationThread_WA, sizeof(bluetoothCommunicationThread_WA),
			NORMALPRIO, bluetoothCommunicationThread, NULL);

	initI2C2();
	initMPU();
	initDMP();

	startPWM();

	// PID tuning
	initBalancing();
	setTunings(&yawStruct, YAW_P_VAL, YAW_I_VAL, YAW_D_VAL, DIRECT);
	setTunings(&pitchStruct, PITCH_P_VAL, PITCH_I_VAL, PITCH_D_VAL, REVERSE);
	setTunings(&rollStruct, ROLL_P_VAL, ROLL_I_VAL, ROLL_D_VAL, REVERSE);
	initRegulators(&yawStruct, &pitchStruct, &rollStruct);

	ch1 = 1200.f;
	ch2 = 1100.f;
	ch3 = 1300.f;

	while(1)
	{
		getYPR(ypr);
//		print("y: ");
//		printn(m_ypr[0]);
//		print(" p: ");
//		printn(m_ypr[1]);
//		print(" r: ");
//		printn(m_ypr[2]);
//		println("");

		Compute(ypr[0], &bal_axes, 0, &yawStruct);
		Compute(ypr[1], &bal_bd, 0, &pitchStruct);
		Compute(ypr[2], &bal_ac, 0, &rollStruct);

		print(" p: ");
		printn(ypr[1]);
		println("");
		print(" bal_bd: ");
		printn(bal_bd);
		println("");

		chThdSleepMilliseconds(50);
	}
}
