/*
 * utils.c
 *
 *  Created on: Nov 27, 2018
 *      Author: wizath
 */

#include "utils.h"

void delay_ms(unsigned int ms)
{
	for (int i = 0; i < SystemCoreClock / 1000; i++);
}
