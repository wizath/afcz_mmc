/*
 * gpio.h
 *
 *  Created on: Nov 26, 2018
 *      Author: wizath
 */

#ifndef INCLUDE_PLATFORM_GPIO_H_
#define INCLUDE_PLATFORM_GPIO_H_

/*
 * gpio.c
 *
 *  Created on: Nov 26, 2018
 *      Author: wizath
 */

#include "config.h"

uint8_t gpio_pin_read(uint8_t port, uint8_t pin);
void gpio_pin_set(uint8_t port, uint8_t pin);
void gpio_pin_clear(uint8_t port, uint8_t pin);
void gpio_pin_cfg_output(uint8_t port, uint8_t pin);
void gpio_pin_cfg_input(uint8_t port, uint8_t pin);
void gpio_pre_pwr_init(void);
void gpio_post_pwr_init(void);

#endif /* INCLUDE_PLATFORM_GPIO_H_ */
