/*
 * uart.h
 *
 *  Created on: Nov 21, 2018
 *      Author: wizath
 */

#ifndef INCLUDE_PLATFORM_UART_H_
#define INCLUDE_PLATFORM_UART_H_

#include "FreeRTOS.h"
#include "queue.h"

xQueueHandle	xUartRxQueue;

void uart_init(void);

#endif /* INCLUDE_PLATFORM_UART_H_ */
