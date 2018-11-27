/*
 * cli.c
 *
 *  Created on: Aug 4, 2017
 *      Author: wizath
 */

#include "config.h"

#include "lpc177x_8x_uart.h"

#include "ucli.h"
#include "cli.h"
#include "uart.h"

static ucli_cmd_t g_cmds[];

int printfn(char ch)
{
	UART_Send(LPC_UART0, (uint8_t *) &ch, 1, BLOCKING);
	return 1;
}

static void fh_reboot(void * a_data)
{
	NVIC_SystemReset();
}

void cli_init(void)
{
	ucli_init((void*) printfn, g_cmds);
}

static void fh_sw_version(void * a_data)
{
	printf("MMC_AFCZ v%.02f, built %s %s, for hardware revision: v%0.2f\r\n", 1.2f, __DATE__, __TIME__, 1.0f);
}

static void fh_devid(void * a_data)
{
	unsigned int b1;// = (*(uint32_t *) (0x1FFF7A10));
	unsigned int b2;// = (*(uint32_t *) (0x1FFF7A10 + 4));
	unsigned int b3;// = (*(uint32_t *) (0x1FFF7A10 + 8));
	printf("[devid] %08X%08X%08X\r\n", b1, b2, b3);
}

void vCommandConsoleTask(void *pvParameters)
{
	char ch;

	for (;;)
	{
		if (xQueueReceive(xUartRxQueue, &ch, portMAX_DELAY))
		{
			ucli_process_chr(ch);
		}
	}
}

static ucli_cmd_t g_cmds[] = {
	{ "devid", fh_devid, 0x00, "Print device unique id\r\n" },
	{ "reboot", fh_reboot, 0x00, "Reboot device\r\n" },
	{ "version", fh_sw_version, 0x00, "Print software version information\r\n" },

    // null
    { 0x00, 0x00, 0x00  }
};
