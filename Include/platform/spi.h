/*
 * spi.h
 *
 *  Created on: Jan 23, 2018
 *      Author: wizath
 */

#ifndef SPI_H_
#define SPI_H_

#include "config.h"

void spi_init(void);
void spi_dma_transfer(uint8_t * tx_data, uint8_t * rx_data, uint32_t length);
void spi_dma_write(uint8_t * tx_data, uint32_t length);
uint8_t spi_transfer(uint8_t data);

#endif /* SPI_H_ */
