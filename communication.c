/*
 * serialConfig.c
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#include "communication.h"

//static const I2CConfig i2ccfg = {
//		OPMODE_I2C,
//		400000,
//		FAST_DUTY_CYCLE_2,
//};

//	//I2C setup
//	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4)  | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
//	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4)  | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */
//	txbuf[0] = 0x00;
//	txbuf[0] = 0x00;
//	i2cStart(&I2CD1, &i2ccfg);
//	i2cAcquireBus(&I2CD1);
//	i2cMasterTransmit(&I2CD1, 0x82 >> 1, txbuf, 1, rxbuf, 2);
//	i2cReleaseBus(&I2CD1);
//	chThdSleepMilliseconds(500);

void init_USART2(void){
	SerialConfig sConfig2;

	sConfig2.speed = 9600;
	sConfig2.cr1 = 0;
	sConfig2.cr2 = USART_CR2_STOP1_BITS | USART_CR2_LINEN;
	sConfig2.cr3 = 0;

	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	sdStart(&SD2, &sConfig2);
}
