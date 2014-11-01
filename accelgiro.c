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

#define OUTPUT_READABLE_QUATERNION

// MPU control/status vars
bool_t accelGyroOK=false;
bool_t dmpReady = false;  	// set true if DMP init was successful
uint8_t mpuIntStatus;   	// holds actual interrupt status byte from MPU
uint8_t devStatus;      	// return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    	// expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     	// count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; 	// FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

/* I2C interface #2 */
static const I2CConfig i2cfg2 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_2,
};

bool_t init_i2c2(void)
{
//	println("Init I2C");

	// I2C interface #2
	i2cStart(&I2CD2, &i2cfg2);
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	// I2C2_SCL PB10; connect VIO to 3.3V !!!
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	// I2C2_SDA PB11

	// init accelerometer
	MPU6050(0x69);
	chThdSleepMilliseconds(100);
	MPUinitialize();

	// verify connection
	if(MPUtestConnection() == true) {
//		println("MPUtestConnection (i2c) ok");
		accelGyroOK = true;
	} else {
//		println("MPUtestConnection (i2c) not ok");
		accelGyroOK = false;
	}

	// init DMP
	if((devStatus = MPUdmpInitialize()) == 0) {
		println("DMP init ok");
		println("Enabling DMP...");
		MPUsetDMPEnabled(true);

		// enable Arduino interrupt detection
		//Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
		//attachInterrupt(0, dmpDataReady, RISING);
		mpuIntStatus = MPUgetIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        println("DMP ready!");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = MPUdmpGetFIFOPacketSize();
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

	return accelGyroOK;
}

void DMPdata(void) {
	// if programming failed, don't try to do anything
	if (!dmpReady)
		return;
	// get current FIFO count
	fifoCount = MPUgetFIFOCount();

	if (fifoCount == 1024) {
		// reset so we can continue cleanly
		MPUresetFIFO();
		println("FIFO overflow!");

		// otherwise, check for DMP data ready interrupt (this should happen frequently)
	} else if (fifoCount >= 42) {
		// read a packet from FIFO
		MPUgetFIFOBytes(fifoBuffer, packetSize);

#ifdef OUTPUT_READABLE_QUATERNION
		// display quaternion values in easy matrix form: w x y z
		MPUdmpGetQuaternion(&q, fifoBuffer);
//		println("quat %7.2f %7.2f %7.2f %7.2f    ", q.w,q.x,q.y,q.z);
//		print("q.w: ");
//		printn(q.w);
//		print(" q.x: ");
//		printn(q.x);
//		print(" q.y: ");
//		printn(q.y);
//		print(" q.z: ");
//		printn(q.z);
//		println("");
#endif

#ifdef OUTPUT_READABLE_EULER
		// display Euler angles in degrees
		MPUdmpGetQuaternion(&q, fifoBuffer);
		MPUdmpGetEuler(euler, &q);
//		println("euler %7.2f %7.2f %7.2f    ", euler[0] * 180/M_PI, euler[1] * 180/M_PI, euler[2] * 180/M_PI);
#endif

#ifdef OUTPUT_READABLE_YAWPITCHROLL
		// display Euler angles in degrees
		MPUdmpGetQuaternion(&q, fifoBuffer);
		MPUdmpGetGravity(&gravity, &q);
		MPUdmpGetYawPitchRoll(ypr, &q, &gravity);
		print("y: ");
		printn(ypr[0] * 180/M_PI);
		print(" p: ");
		printn(ypr[1] * 180/M_PI);
		print(" r: ");
		printn(ypr[2] * 180/M_PI);
		println("");
#endif

#ifdef OUTPUT_READABLE_REALACCEL
		// display real acceleration, adjusted to remove gravity
		MPUdmpGetQuaternion(&q, fifoBuffer);
		MPUdmpGetAccel(&aa, fifoBuffer);
		MPUdmpGetGravity(&gravity, &q);
		MPUdmpGetLinearAccel(&aaReal, &aa, &gravity);
//		printf("areal %6d %6d %6d    ", aaReal.x, aaReal.y, aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
		// display initial world-frame acceleration, adjusted to remove gravity
		// and rotated based on known orientation from quaternion
		MPUdmpGetQuaternion(&q, fifoBuffer);
		MPUdmpGetAccel(&aa, fifoBuffer);
		MPUdmpGetGravity(&gravity, &q);
		MPUdmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
//		printf("aworld %6d %6d %6d    ", aaWorld.x, aaWorld.y, aaWorld.z);
#endif

#ifdef OUTPUT_TEAPOT
		// display quaternion values in InvenSense Teapot demo format:
		teapotPacket[2] = fifoBuffer[0];
		teapotPacket[3] = fifoBuffer[1];
		teapotPacket[4] = fifoBuffer[4];
		teapotPacket[5] = fifoBuffer[5];
		teapotPacket[6] = fifoBuffer[8];
		teapotPacket[7] = fifoBuffer[9];
		teapotPacket[8] = fifoBuffer[12];
		teapotPacket[9] = fifoBuffer[13];
//		Serial.write(teapotPacket, 14);
		teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
#endif
	}
}

void getRawAccelGyro(void) {
	int16_t ax, ay, az, gx, gy, gz;

	MPUgetMotion6(&ax, &ay, &az, &gx, &gy, &gz);

//	sdWrite(&SD2, "x = ", ax);
}
