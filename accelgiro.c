/*
 * accelgiro.c
 *
 *  Created on: Jul 23, 2014
 *      Author: adrianp
 */

#include "accelgiro.h"
#include "BluetoothCommunicationThread.h"
#include "MPU6050-ChibiOS-devel/MPU6050/MPU6050.h"
#include "MPU6050-ChibiOS-devel/MPU6050/helper_3dmath.h"
#include "MPU6050-ChibiOS-devel/MPU6050/MPU6050_6Axis_MotionApps20.h"

// MPU control/status vars
bool_t accelGyroOK=false;
bool_t dmpReady = false;  	// set true if DMP init was successful
uint8_t mpuIntStatus;   	// holds actual interrupt status byte from MPU
uint8_t devStatus;      	// return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    	// expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     	// count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; 	// FIFO storage buffer

// orientation/motion vars
Quaternion q;                          // quaternion for mpu output
VectorFloat gravity;                   // gravity vector for ypr
float ypr[3] = {0.0f,0.0f,0.0f};       // yaw pitch roll values
float yprLast[3] = {0.0f, 0.0f, 0.0f};

/* I2C interface #2 */
static const I2CConfig i2cfg2 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_2,
};

void initI2C2(void)
{
	println("Init I2C");

	// I2C interface #2
	// connect VIO to 3V on Discovery Board!!!; connect VDD to 3V on Discovery Board!!!
	i2cStart(&I2CD2, &i2cfg2);
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	// I2C2_SCL PB10
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	// I2C2_SDA PB11
}

void initMPU(void)
{
	// init accelerometer
	MPU6050(0x69);
	chThdSleepMilliseconds(100);
	MPUinitialize();

	// verify connection
	if (MPUtestConnection() == true) {
		println("MPUtestConnection (i2c) ok");
		accelGyroOK = true;
	} else {
		println("MPUtestConnection (i2c) not ok");
		accelGyroOK = false;
	}
}

void initDMP(void)
{
	if((devStatus = MPUdmpInitialize()) == 0) {
		println("DMP init ok");
		println("Enabling DMP...");
		MPUsetDMPEnabled(true);

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        println("DMP ready!");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = MPUdmpGetFIFOPacketSize();
        print("packetSize = ");
        printn(packetSize);
        println("");
	} else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
		switch(devStatus) {
		case 1:
			println("DMP error: initial memory load failed");
			break;
		case 2:
			println("DMP error: configuration updates failed");
			break;
		default:
			println("DMP error: unknown");
			break;
		}
	}
}

void getYPR()
{
	// get current FIFO count
	mpuIntStatus = MPUgetIntStatus();
	fifoCount = MPUgetFIFOCount();

	if (fifoCount == 1024) {
		// reset so we can continue cleanly
		MPUresetFIFO();
		// otherwise, check for DMP data ready interrupt (this should happen frequently)
	} else if (mpuIntStatus & 0x02) {

		while (fifoCount < packetSize) fifoCount = MPUgetFIFOCount();
		// read a packet from FIFO
		MPUgetFIFOBytes(fifoBuffer, packetSize);

		// display Euler angles in degrees
		MPUdmpGetQuaternion(&q, fifoBuffer);
		MPUdmpGetGravityVect(&gravity, &q);
		MPUdmpGetYawPitchRoll(ypr, &q, &gravity);
		print("y: ");
		printn(ypr[0] * 180/M_PI);
		print(" p: ");
		printn(ypr[1] * 180/M_PI);
		print(" r: ");
		printn(ypr[2] * 180/M_PI);
		println("");
	}
}
