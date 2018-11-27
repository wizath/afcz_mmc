/*
 * i2c.c
 *
 *  Created on: Jun 11, 2017
 *      Author: wizath
 */

#include "i2c.h"

#include <SI324regs.h>
#include "Si5341-RevB-AFCZ-Registers.h"

void i2c_init()
{
	PINSEL_ConfigPin(0, 0, 3);
	PINSEL_ConfigPin(0, 1, 3);

	PINSEL_SetOpenDrainMode(0, 0, ENABLE);
	PINSEL_SetOpenDrainMode(0, 1, ENABLE);

	PINSEL_SetI2CMode(0, 0, PINSEL_I2CMODE_OPENDRAINIO);
	PINSEL_SetI2CMode(0, 1, PINSEL_I2CMODE_OPENDRAINIO);

	I2C_Init(I2C_1, 100000);
	I2C_Cmd(I2C_1, ENABLE);
}

uint8_t i2c_write(uint8_t slaveAddr, uint8_t * buff, uint8_t len)
{
	I2C_M_SETUP_Type xfer = { 0 };

	uint8_t retry_count = 10;

	xfer.sl_addr7bit = slaveAddr;
	xfer.tx_data = buff;
	xfer.tx_length = len;
	xfer.rx_length = 0;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) {
			printf("wt\r\n");break;
		}
	}

	return len - xfer.tx_count;
}

uint8_t i2c_read(uint8_t slaveAddr, uint8_t * buff, uint8_t len)
{
	I2C_M_SETUP_Type xfer = { 0 };
	uint8_t retry_count = 10;

	xfer.sl_addr7bit = slaveAddr;
	xfer.rx_data = buff;
	xfer.rx_length = len;
	xfer.tx_length = 0;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) {
			printf("rt\r\n");break;
		}
	}

	return len - xfer.rx_count;
}

/* Function that probes all available slaves connected to an I2C bus */
void i2c_probe_slaves(void)
{
	int i;
	uint8_t ch[2];

	printf("Probing available I2C devices...\r\n");
	printf("\r\n\t00\t01\t02\t03\t04\t05\t06\t07\t08\t09\t0A\t0B\t0C\t0D\t0E\t0F");
	printf("\r\n==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==");
	for (i = 0; i <= 0x7F; i++) {
		if (!(i & 0x0F))
		{
			printf("\r\n%02X  ", i >> 4);
		}

		if (i <= 7 || i > 0x78)
		{
			printf("\t");
			continue;
		}

		I2C_M_SETUP_Type xfer = { 0 };

		xfer.sl_addr7bit = i;
		xfer.rx_data = ch;
		xfer.rx_length = 1 + (i == 0x48);

		I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING);

		if(xfer.rx_count > 0)
		{
			printf("\t%02X", i);
		} else {
			printf("\t---");
		}
	}
	printf("\r\n");
}

void i2c_tcaselect(uint8_t i)
{
	uint8_t WrBuffer[4];
	WrBuffer[0] = 1 << i;
	i2c_write(TCAADDR, WrBuffer, 1);
}


void i2c_TCA9539(uint16_t i)
{
	//i2c_write(TCA9539ADDR, &i, 2);

	uint8_t WrBuffer[4];



	// set proper MUX address
	WrBuffer[0] = 1<<2;//select channel 2 only
	i2c_write(TCAADDR,WrBuffer, 1);


		WrBuffer[0] = 0x6; // configuration port 0
		WrBuffer[1] = 0x00; // all outputs
		i2c_write(TCA9539ADDR, WrBuffer, 2);
		WrBuffer[0] = 0x7; // configuration port 1
		WrBuffer[1] = 0x00; // all outputs
		i2c_write(TCA9539ADDR, WrBuffer, 2);

		WrBuffer[0] = 0x2; // output port 0
		WrBuffer[1] = i & 0xFF;
		WrBuffer[2] = (i >>8) & 0xFF;
		i2c_write(TCA9539ADDR, WrBuffer, 3);

	WrBuffer[0] = 0;
	i2c_write(TCAADDR,WrBuffer, 1); //de-select switch
}

void i2c_probe_slaves2(void)
{
	uint8_t ch[2];

	printf("TCAScanner start\n");

	for (uint8_t t = 0; t < 8; t++)
	{
		i2c_tcaselect(1 << t);
	    printf("\nTCA Port #%d\n", t);
	    for (uint8_t addr = 0; addr<=127; addr++)
	    {
	    	if (addr == TCAADDR) continue;
	    	I2C_M_SETUP_Type xfer = { 0 };

			xfer.sl_addr7bit = addr;
			xfer.rx_data = ch;
			xfer.rx_length = 1; // only one byte response

			I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING);

			if(xfer.rx_count > 0)
			{
				printf("Found I2C %02X\n", addr);
			}
	    }
	}
}



void i2c_setSI5341(void)
{
	uint8_t WrBuffer[2];
	int Index;


	// set proper MUX address
	WrBuffer[0] = 1<<2;
	i2c_write(TCAADDR,WrBuffer, 1); //select channel 4 only

	for (Index = 0; Index < 6; Index++) {
		//extract page address
		WrBuffer[0] = 0x01; // page address register
		WrBuffer[1] = ((InitTable_Si5341[Index].RegIndex) >>8) & 0xFF;
		i2c_write(SI5341_ADDR, WrBuffer, 2); // set page

		WrBuffer[0] = ((InitTable_Si5341[Index].RegIndex) ) & 0xFF;
		WrBuffer[1] = InitTable_Si5341[Index].Value;
		i2c_write(SI5341_ADDR, WrBuffer, 2);
	}
	delay_ms(300);
	for (Index = 6; Index < SI5341_REVB_REG_CONFIG_NUM_REGS; Index++) {
		//extract page address
		WrBuffer[0] = 0x01; // page address register
		WrBuffer[1] = ((InitTable_Si5341[Index].RegIndex) >>8) & 0xFF;
		i2c_write(SI5341_ADDR, WrBuffer, 2); // set page

		WrBuffer[0] = ((InitTable_Si5341[Index].RegIndex) ) & 0xFF;
		WrBuffer[1] = InitTable_Si5341[Index].Value;
		i2c_write(SI5341_ADDR, WrBuffer, 2);
	}

	WrBuffer[0] = 0;
	i2c_write(TCAADDR,WrBuffer, 1); //deselect switch
}



void i2c_set8V54816(void)
{
	uint8_t WrBuffer[16];
	int Index;


	// set proper MUX address
	WrBuffer[0] = 1<<2;
	i2c_write(TCAADDR,WrBuffer, 1); //select channel 4 only


		//CH0 config, TCLKA IN
		WrBuffer[0] = IDT_IN | IDT_TERM_ON | IDT_POL_P;
		//CH1 config, TCLKB IN
		WrBuffer[1] = IDT_IN | IDT_TERM_ON | IDT_POL_P;
		//CH2 config, not used, SILABS output routed
		WrBuffer[2] = IDT_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH15;
		//CH3 config, SI5325 in
		WrBuffer[3] = IDT_IN | IDT_TERM_ON | IDT_POL_P ;
		//CH4 config, GTX CLK connected to CDR CLK (Si5325)
		WrBuffer[4] = IDT_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH3 ;
		//CH5 config, FMC Crossbar 3 input
		WrBuffer[5] = IDT_IN | IDT_TERM_ON | IDT_POL_P ;
		//CH6 config, Si5341 CLKO1 input (300MHz)
		WrBuffer[6] = IDT_IN | IDT_TERM_ON | IDT_POL_P ;
		//CH7 config, RTM_CLK input
		WrBuffer[7] = IDT_IN | IDT_TERM_ON | IDT_POL_P ;
		//CH8 config, GTX CLK 2 connected to  CDR CLK (Si5325)
		WrBuffer[8] = IDT_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH3 ;
		//CH9 config, FPGA FMC bank CLK   connected to  CDR CLK (Si5325)
		WrBuffer[9] = IDT_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH3 ;
		//CH10 config, FMC1 CLK2 in
		WrBuffer[10] = IDT_IN | IDT_TERM_ON | IDT_POL_P ;
		//CH11 config, GTX CLK 4 connected to  CDR CLK (Si5325)
		WrBuffer[11] = IDT_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH3 ;
		//CH12 config, FPGA_CLK1 (SDRAM) routed to CH6, Si5341 125MHz output
		WrBuffer[12] = IDT_OUT | IDT_TERM_OFF | IDT_POL_P  | IDT_SRC_CH6 ;
		//CH13 config, Silabs Si571_1 input
		WrBuffer[13] = IDT_IN | IDT_TERM_ON | IDT_POL_P ;
		//CH14 config, FMC2 CLK2 in
		WrBuffer[14] = IDT_IN | IDT_TERM_OFF | IDT_POL_P ;
		//CH15 config, Silabs Si571_2 input
		WrBuffer[15] = IDT_IN | IDT_TERM_OFF | IDT_POL_P ;
		i2c_write(IDT_8V54816, WrBuffer, 16);


	WrBuffer[0] = 0;
	i2c_write(TCAADDR,WrBuffer, 1); //deselect switch
}





void i2c_setSI5324(void)
{
	uint8_t WrBuffer[2];
	int Index;


	// set proper MUX address
	WrBuffer[0] = 1<<2;
	i2c_write(TCAADDR,WrBuffer, 1); //select channel 4 only

	for (Index = 0; Index < sizeof(InitTable)/2; Index++) {
		WrBuffer[0] = InitTable[Index].RegIndex;
		WrBuffer[1] = InitTable[Index].Value;

		// TODO CHECK SI5324 ADDRESS
		i2c_write(SI5324_ADDR, WrBuffer, 2);
	}
	WrBuffer[0] = 0;
	i2c_write(TCAADDR,WrBuffer, 1); //deselect switch
}

void i2c_setSI5324_RTM(void)
{
	uint8_t WrBuffer[2];
	int Index;


	// set proper MUX address
	WrBuffer[0] = 128;
	i2c_write(TCAADDR, WrBuffer, 1); //select channel 7 only
	// set proper MUX RTM address
	WrBuffer[0] = 32;
	i2c_write(TCARTMADDR ,WrBuffer, 1); //select channel 5 only


	for (Index = 0; Index < sizeof(InitTable_RTM)/2; Index++) {
		WrBuffer[0] = InitTable_RTM[Index].RegIndex;
		WrBuffer[1] = InitTable_RTM[Index].Value;

		// TODO CHECK SI5324 ADDRESS
		i2c_write(SI5324_ADDR, WrBuffer, 2);
	}


	WrBuffer[0] = 0;
	i2c_write(TCARTMADDR ,WrBuffer, 1); //deselect switch
	// set proper MUX address
	WrBuffer[0] = 0;
	i2c_write(TCAADDR, WrBuffer, 1); //deselect channel
}


void i2c_Exar_dump(void)
{
	uint8_t WrBuffer[2];
	//int Index;

	printf("------------ Exar 1 Dump ----------\n");
	// set proper MUX address
	WrBuffer[0] = 32;
	i2c_write(TCAADDR, WrBuffer, 1); //select channel 5 only - Exar chip

	WrBuffer[0] = 0x02;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
    printf("GET_HOST_STS 0x%x 0x%x 0x%x\n", 0x02, WrBuffer[0],WrBuffer[1]);

	WrBuffer[0] = 0x05;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("GET_FAULT_STS 0x%x 0x%x 0x%x\n", 0x05, WrBuffer[0],WrBuffer[1]);

	WrBuffer[0] = 0x09;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_GET_STATUS 0x%x 0x%x 0x%x\n", 0x09, WrBuffer[0],WrBuffer[1]);

	WrBuffer[0] = 0x10;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH1 0x%x %.2f V\n", 0x10, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x11;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH2 0x%x %.2f V\n", 0x11, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x12;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH3 0x%x %.2f V\n", 0x12, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x13;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH4 0x%x %.2f V\n", 0x13, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x14;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_IN 0x%x %.2f V\n", 0x14, (WrBuffer[0]*256+WrBuffer[1])*0.0125);

	printf("------------ Exar Dump 2 ----------\n");
	// set proper MUX address
	WrBuffer[0] = 16;
	i2c_write(TCAADDR, WrBuffer, 1); //select channel 5 only - Exar chip

	WrBuffer[0] = 0x02;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("GET_HOST_STS 0x%x 0x%x 0x%x\n", 0x02, WrBuffer[0],WrBuffer[1]);

	WrBuffer[0] = 0x05;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("GET_FAULT_STS 0x%x 0x%x 0x%x\n", 0x05, WrBuffer[0],WrBuffer[1]);

	WrBuffer[0] = 0x09;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_GET_STATUS 0x%x 0x%x 0x%x\n", 0x09, WrBuffer[0],WrBuffer[1]);

	WrBuffer[0] = 0x10;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH1 0x%x %.2f V\n", 0x10, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x11;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH2 0x%x %.2f V\n", 0x11, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x12;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH3 0x%x %.2f V\n", 0x12, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x13;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_CH4 0x%x %.2f V\n", 0x13, (WrBuffer[0]*256+WrBuffer[1])*0.015);

	WrBuffer[0] = 0x14;
	i2c_write(0x28 ,WrBuffer, 1); //write to Exar chip
	i2c_read(0x28, WrBuffer, 2);
	printf("PWR_READ_VOLTAGE_IN 0x%x %.2f V\n", 0x14, (WrBuffer[0]*256+WrBuffer[1])*0.0125);
}


void i2c_RTM_PWRON(void)
{
	uint8_t WrBuffer[2];
	//int Index;


	// set proper MUX address
	WrBuffer[0] = 128;
	i2c_write(TCAADDR, WrBuffer, 1); //select channel 7 only
	// set proper MUX RTM address
	WrBuffer[0] = 1<<7 | 0<<6 | 0<<5 | 0<<4 | 0<<3 | 0<<2 | 0<<1 | 1;
	i2c_write(0x3E ,WrBuffer, 1); //set IO extender reset, LEDs and power
	delay_ms(200);
	WrBuffer[0] = 1<<7 | 0<<6 | 1<<5 | 1<<4 | 0<<3 | 0<<2 | 0<<1 | 1;
	i2c_write(0x3E ,WrBuffer, 1); //set IO extender, LEDs and power

	// set proper MUX address
	WrBuffer[0] = 0;
	i2c_write(TCAADDR, WrBuffer, 1); //deselect channel
}
