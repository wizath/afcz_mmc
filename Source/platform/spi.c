/*
 * spi.c
 *
 *  Created on: Jan 23, 2018
 *      Author: wizath
 */

#include "config.h"
#include "spi.h"
#include "gpio.h"

#include "lpc177x_8x_ssp.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_gpdma.h"
#include "lpc177x_8x_gpio.h"

#define SPI_DMA_TIMEOUT 	200000

void spi_init(void)
{
	SSP_CFG_Type SSP_ConfigStruct;

	/* Initialize SSP pin connect
	 * P1.20 - SCK AL5
	 * P1.21 - SSEL AL3
	 * P1.23 - MISO AL5
	 * P1.24 - MOSI AL5
	 */

	PINSEL_ConfigPin(1, 20, 5);
	PINSEL_ConfigPin(1, 23, 5);
	PINSEL_ConfigPin(1, 24, 5);

	gpio_pin_cfg_output(1, 21);
	gpio_pin_set(1, 21);

	SSP_ConfigStructInit(&SSP_ConfigStruct);
	SSP_ConfigStruct.ClockRate = 50000000;
	SSP_Init(LPC_SSP0, &SSP_ConfigStruct);

	SSP_Cmd(LPC_SSP0, ENABLE);
	GPDMA_Init();
}

void spi_dma_transfer(uint8_t * tx_data, uint8_t * rx_data, uint32_t length)
{
	GPDMA_Channel_CFG_Type GPDMACfg;

	GPDMACfg.ChannelNum = 0; 								/* DMA Channel 0 */
	GPDMACfg.SrcMemAddr = (uint32_t) tx_data; 				// Source memory
	GPDMACfg.DstMemAddr = 0; 								// Destination memory - Not used
	GPDMACfg.TransferSize = length; 						// Transfer size
	GPDMACfg.TransferWidth = 0; 							// Transfer width - not used
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P; 		// Transfer type
	GPDMACfg.SrcConn = 0; 									// Source connection - unused
	GPDMACfg.DstConn = GPDMA_CONN_SSP0_Tx; 					// Destination connection
	GPDMACfg.DMALLI = 0; 									// Linker List Item - unused
	GPDMA_Setup(&GPDMACfg); 								// Setup channel with given parameter

	GPDMACfg.ChannelNum = 1; 								/* DMA Channel 1 */
	GPDMACfg.SrcMemAddr = 0;								// Source memory - not used
	GPDMACfg.DstMemAddr = (uint32_t) rx_data;				// Destination memory - Not used
	GPDMACfg.TransferSize = length;							// Transfer size
	GPDMACfg.TransferWidth = 0;								// Transfer width - not used
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;			// Transfer type
	GPDMACfg.SrcConn = GPDMA_CONN_SSP0_Rx;					// Source connection
	GPDMACfg.DstConn = 0;									// Destination connection - not used
	GPDMACfg.DMALLI = 0;									// Linker List Item - unused
	GPDMA_Setup(&GPDMACfg);									// Setup channel with given parameter

	SSP_DMACmd(LPC_SSP0, SSP_DMA_RX, ENABLE);
	SSP_DMACmd(LPC_SSP0, SSP_DMA_TX, ENABLE);
	GPDMA_ChannelCmd(0, ENABLE);
	GPDMA_ChannelCmd(1, ENABLE);

	uint32_t timeout = SPI_DMA_TIMEOUT;
	while (GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0) == RESET) {
		if (!timeout--) {
			printf("[log] Warning SPI DMA timeout\n");
			break;
		}
	}
	GPDMA_ClearIntPending(GPDMA_STATCLR_INTTC, 0);

	timeout = SPI_DMA_TIMEOUT;
	while (GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 1) == RESET) {
		if (!timeout--) {
			printf("[log] Warning SPI DMA timeout\n");
			break;
		}
	}
	GPDMA_ClearIntPending(GPDMA_STATCLR_INTTC, 1);

	SSP_DMACmd(LPC_SSP0, SSP_DMA_RX, DISABLE);
	SSP_DMACmd(LPC_SSP0, SSP_DMA_TX, DISABLE);

	GPDMA_ChannelCmd(0, DISABLE);
	GPDMA_ChannelCmd(1, DISABLE);
}

void spi_dma_write(uint8_t * tx_data, uint32_t length)
{
	GPDMA_Channel_CFG_Type GPDMACfg;

	GPDMACfg.ChannelNum = 0; 								/* DMA Channel 0 */
	GPDMACfg.SrcMemAddr = (uint32_t) tx_data; 				// Source memory
	GPDMACfg.DstMemAddr = 0; 								// Destination memory - Not used
	GPDMACfg.TransferSize = length; 						// Transfer size
	GPDMACfg.TransferWidth = 0; 							// Transfer width - not used
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P; 		// Transfer type
	GPDMACfg.SrcConn = 0; 									// Source connection - unused
	GPDMACfg.DstConn = GPDMA_CONN_SSP0_Tx; 					// Destination connection
	GPDMACfg.DMALLI = 0; 									// Linker List Item - unused
	GPDMA_Setup(&GPDMACfg); 								// Setup channel with given parameter

	SSP_DMACmd(LPC_SSP0, SSP_DMA_TX, ENABLE);
	GPDMA_ChannelCmd(0, ENABLE);

	while (GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0) == RESET);
	GPDMA_ClearIntPending(GPDMA_STATCLR_INTTC, 0);

	SSP_DMACmd(LPC_SSP0, SSP_DMA_TX, DISABLE);
	GPDMA_ChannelCmd(0, DISABLE);
}

uint8_t spi_transfer(uint8_t data)
{
	SSP_DATA_SETUP_Type xferConfig;
	uint8_t rx;

	xferConfig.tx_data = &data;
	xferConfig.rx_data = &rx;
	xferConfig.length = 1;
	SSP_ReadWrite(LPC_SSP0, &xferConfig, SSP_TRANSFER_POLLING);

	return rx;
}
