/*
 * serialConfig.c
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#include <stdlib.h>
#include "communication.h"
#include "MPU6050-ChibiOS-devel/i2cdev_chibi/i2cdev_chibi.h"
#include "MPU6050-ChibiOS-devel/MPU6050/MPU6050.h"
#include "MPU6050-ChibiOS-devel/MPU6050/MPU6050_6Axis_MotionApps20.h"
#include "MPU6050-ChibiOS-devel/MPU6050/helper_3dmath.h"

/* I2C interface #2 */
static const I2CConfig i2cfg2 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_2,
};

/**
 *
 */
static void print(char *p) {

	while (*p) {
		sdPut(&SD2, *p++);
	}
}

/**
 *
 */
static void println(char *p) {

	while (*p) {
		sdPut(&SD2, *p++);
	}
	sdWriteTimeout(&SD2, (uint8_t *)"\r\n", 2, TIME_INFINITE);
}

/**
 *
 */
static void printn(int16_t n) {
	char buf[16], *p;

	if (n > 0)
		sdPut(&SD2, '+');
	else{
		sdPut(&SD2, '-');
		n = abs(n);
	}

	if (!n)
		sdPut(&SD2, '0');
	else {
		p = buf;
		while (n)
			*p++ = (n % 10) + '0', n /= 10;
		while (p > buf)
			sdPut(&SD2, *--p);
	}
}

void init_i2c2(void)
{
	println("Init I2C");

	/*
	 * I2C interface #2
	 */
	i2cStart(&I2CD2, &i2cfg2);
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	// I2C2_SCL PB10; connect VIO to 3.3V !!!
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	// I2C2_SDA PB11

	MPU6050(0x69);
	chThdSleepMilliseconds(100);
	MPUinitialize();

	if(MPUtestConnection() == true)
		println("MPUtestConnection (i2c) ok");
	else
		println("MPUtestConnection (i2c) not ok");

	/*
	 * Normal main() thread activity, nothing in this test.
	 */
	while (TRUE) {
		chThdSleepMilliseconds(2000);

		println("debug");
	}
}

void init_USART2(void)
{
	SerialConfig sConfig2;

	sConfig2.speed = 9600;
	sConfig2.cr1 = 0;
	sConfig2.cr2 = USART_CR2_STOP1_BITS | USART_CR2_LINEN;
	sConfig2.cr3 = 0;

	//	PA2(TX) and PA3(RX) are routed to USART2
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	//	Activates the serial driver 2 using the driver default configuration.
	sdStart(&SD2, &sConfig2);
}
