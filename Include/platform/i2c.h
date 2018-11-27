/*
 * i2c.h
 *
 *  Created on: Jun 11, 2017
 *      Author: wizath
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include <stdio.h>


#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_gpio.h"



#define TCAADDR 			0x71
#define TCARTMADDR 			0x76

#define IIC_SLAVE_ADDR		0x68
#define SI5324_ADDR			0x68
#define SI5341_ADDR			0x76

#define TCA9539ADDR			0x74
#define CLK_CONFIG 1<<15
#define CLK_LOAD 1<<14
#define CLK_SOUT1 1<<13
#define CLK_SOUT0 1<<12
#define CLK_SIN1 1<<11
#define CLK_SIN0 1<<10
#define CLK_SI57X_OE2 1<<8
#define CLK_CLK_SW_RSTn 1<<7
#define CLK_SI57X_OE1 1<<6
#define CLK_SI53xx_RST 1<<5
#define CLK_IN_SEL1 1<<4
#define CLK_IN_SEL0 1<<3
#define CLK_SI5324_INT_ALM 1<<2
#define CLK_Si5341_INTR_N 1<<1
#define CLK_Si5341_SYNC_B 1<<0

#define IDT_8V54816 	0x58
#define IDT_IN 0
#define IDT_OUT 1<<7
#define IDT_TERM_ON 1<<6
#define IDT_TERM_OFF 0
#define IDT_POL_P 1<<5
#define IDT_POL_N 0
#define IDT_SRC_CH0 0
#define IDT_SRC_CH1 1
#define IDT_SRC_CH2 2
#define IDT_SRC_CH3 3
#define IDT_SRC_CH4 4
#define IDT_SRC_CH5 5
#define IDT_SRC_CH6 6
#define IDT_SRC_CH7 7
#define IDT_SRC_CH8 8
#define IDT_SRC_CH9 9
#define IDT_SRC_CH10 10
#define IDT_SRC_CH11 11
#define IDT_SRC_CH12 12
#define IDT_SRC_CH13 13
#define IDT_SRC_CH14 14
#define IDT_SRC_CH15 15


void i2c_init();
uint8_t i2c_write(uint8_t slaveAddr, uint8_t * buff, uint8_t len);
uint8_t i2c_read(uint8_t slaveAddr, uint8_t * buff, uint8_t len);
void i2c_tcaselect(uint8_t i);
/* Function that probes all available slaves connected to an I2C bus */
void i2c_probe_slaves(void);
void i2c_probe_slaves2(void);
void i2c_setSI5324(void);
void i2c_set8V54816(void);
void i2c_setSI5324_RTM(void);
void i2c_setSI5341(void);
void i2c_TCA9539(uint16_t i);
void i2c_RTM_PWRON(void);
void i2c_Exar_dump(void);



#endif /* INC_I2C_H_ */
