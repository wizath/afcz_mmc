/*
 * config.h
 *
 *  Created on: Nov 26, 2018
 *      Author: wizath
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "math.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* platform includes */
#include "LPC177x_8x.h"

#define BOARD_LED1			3, 3
#define BOARD_LED2			3, 4
#define BOARD_LED3			3, 5

#define I2C_MUX_RESETn		5, 0
#define I2C_MUX_ADDR1		2, 29
#define I2C_MUX_ADDR2		2, 28

#define EN_RTM_12V			1, 26
#define EN_VCCINT			1, 28
#define EN_P5V0				1, 29
#define EN_RTM_MP			1, 30
#define EN_PSU_CH			1, 31

#define PHY_RESETn			0, 23
#define PHY_SEL_RGMII		2, 2
#define PHY_MDIO_EN			0, 31

#define SCANSTA_RESET		4, 18
#define SCANSTA_STICHER		4, 19
#define SCANSTA_LPSEL0		4, 25
#define SCANSTA_LPSEL1		4, 26
#define SCANSTA_LPSEL2		4, 27
#define SCANSTA_LPSEL3		4, 28
#define SCANSTA_LPSEL4		4, 29
#define SCANSTA_LPSEL5		4, 30
#define SCANSTA_LPSEL6		4, 31

#define FPGA_RESETn			4, 9
#define FPGA_INITB			0, 13
#define FPGA_PROGB			0, 4
#define FPGA_BOOT0			2, 16
#define FPGA_BOOT1			2, 17
#define FPGA_BOOT2			2, 18
#define FPGA_BOOT3			2, 19
#define FPGA_FLASH_UPDATE	0, 12
#define FPGA_DONE			0, 5

#define RTM_PRESENT			0, 29
#define P12V_PRESENT		0, 30


#endif /* INCLUDE_CONFIG_H_ */
