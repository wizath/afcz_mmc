/*
 * gpio.c
 *
 *  Created on: Nov 26, 2018
 *      Author: wizath
 */

#include "gpio.h"
#include "lpc177x_8x_gpio.h"

static void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for (int i = 0; i < SystemCoreClock / 1000; i++);
//
//    for(i=0;i<ms;i++)
//         for(j=0;j<20000;j++);
}

uint8_t gpio_pin_read(uint8_t port, uint8_t pin)
{
	uint32_t value = GPIO_ReadValue(port);
	return (value >> pin) & 0x01;
}

void gpio_pin_set(uint8_t port, uint8_t pin)
{
	GPIO_SetValue(port, 1 << pin);
}

void gpio_pin_clear(uint8_t port, uint8_t pin)
{
	GPIO_ClearValue(port, 1 << pin);
}

void gpio_pin_cfg_output(uint8_t port, uint8_t pin)
{
	GPIO_SetDir(port, 1 << pin, 1);
}

void gpio_pin_cfg_input(uint8_t port, uint8_t pin)
{
	GPIO_SetDir(port, 1 << pin, 0);
}

void gpio_pre_pwr_init(void)
{
	gpio_pin_cfg_output(I2C_MUX_RESETn);
	gpio_pin_cfg_output(I2C_MUX_ADDR1);
	gpio_pin_cfg_output(I2C_MUX_ADDR2);

	// reset I2C mux
	gpio_pin_clear(I2C_MUX_RESETn);
	delay_ms(10);
	gpio_pin_set(I2C_MUX_RESETn);

	// mux addressing
	// addr1 = 0
	// addr2 = 0
	gpio_pin_clear(I2C_MUX_ADDR1);
	gpio_pin_clear(I2C_MUX_ADDR2);
}

void gpio_post_pwr_init(void)
{
	gpio_pin_cfg_output(SCANSTA_RESET);
	gpio_pin_cfg_output(SCANSTA_LPSEL0);
	gpio_pin_cfg_output(SCANSTA_LPSEL1);
	gpio_pin_cfg_output(SCANSTA_LPSEL2);
	gpio_pin_cfg_output(SCANSTA_LPSEL3);
	gpio_pin_cfg_output(SCANSTA_LPSEL4);
	gpio_pin_cfg_output(SCANSTA_LPSEL5);
	gpio_pin_cfg_output(SCANSTA_LPSEL6);

	gpio_pin_cfg_output(FPGA_DONE);
	gpio_pin_cfg_output(FPGA_RESETn);
	gpio_pin_cfg_input(FPGA_INITB);
	gpio_pin_cfg_input(FPGA_PROGB);

	gpio_pin_cfg_input(RTM_PRESENT);
	gpio_pin_cfg_input(P12V_PRESENT);

	gpio_pin_cfg_output(FPGA_BOOT0);
	gpio_pin_cfg_output(FPGA_BOOT1);
	gpio_pin_cfg_output(FPGA_BOOT2);
	gpio_pin_cfg_output(FPGA_BOOT3);
	gpio_pin_cfg_output(FPGA_FLASH_UPDATE);

	gpio_pin_cfg_output(PHY_RESETn);
	gpio_pin_cfg_output(PHY_SEL_RGMII);
}
