/*
 * uart.c
 *
 *  Created on: Nov 21, 2018
 *      Author: wizath
 */

#include "uart.h"
#include "LPC177x_8x.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_pinsel.h"

void uart_init(void)
{
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;

	PINSEL_ConfigPin (0, 2, 1);
	PINSEL_ConfigPin (0, 3, 1);

	UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate = 115200u;
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);

	LPC_GPIO2->DIR |= (1 << 26);

	UART_Init(LPC_UART0, &UARTConfigStruct);
	UART_TxCmd(LPC_UART0, ENABLE);

//	UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);
//	NVIC_EnableIRQ(UART0_IRQn);
//
//	xUartRxQueue = xQueueCreate(64, sizeof(uint16_t));
}

void UART0_IRQHandler(void)
{
	uint32_t intsrc, tmp;
	char tmpc;
	intsrc = UART_GetIntId(LPC_UART0);
	tmp = intsrc & UART_IIR_INTID_MASK;
	uint32_t rLen;

	// Receive Data Available or Character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)) {

		while (1) {
			// Call UART read function in UART driver
			rLen = UART_Receive(LPC_UART0, &tmpc, 1, NONE_BLOCKING);
			// If data received
			if (rLen) {
				xQueueSendFromISR(xUartRxQueue, &tmpc, 0);
			} else
				break; // no more data
		}
	}

	NVIC_ClearPendingIRQ(UART0_IRQn);
}
