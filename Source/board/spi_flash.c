/*
 * spi_flash.c
 *
 *  Created on: Jan 24, 2018
 *      Author: wizath
 */

#include "lpc177x_8x_gpio.h"
#include "spi_flash.h"
#include "spi.h"

#define CSASSERT()  GPIO_ClearValue(1, (1 << 21))
#define CSRELEASE() GPIO_SetValue(1, (1 << 21))

uint8_t dummy_rx[256] = { 0 };

uint8_t spif_read_status(uint8_t * status_reg)
{
	CSASSERT();

	spi_transfer(SPIF_CMD_STATUS_REGISTER);
	*status_reg = spi_transfer(0x00);

	CSRELEASE();

	return FLASH_SUCCESS;
}

bool spif_busy(void)
{
	uint8_t status;

	spif_read_status(&status);
	return status & 0x01;
}

uint8_t spif_wait(void)
{
	uint32_t timeout = FLASH_WAIT_TIMEOUT;
	while (spif_busy()) {
		if (!timeout--) return FLASH_TIMEOUT;
	}

	return FLASH_SUCCESS;
}

uint8_t spif_write_enable(void)
{
	uint8_t reg, retries = 0x00;

	CSASSERT();
	spi_transfer(SPIF_CMD_WRITE_ENABLE);
	CSRELEASE();

	do {
		retries++;
		spif_read_status(&reg);
	} while ((~reg & 0x02) && (retries < 255));

	if (retries == 255) return FLASH_TIMEOUT;
	return FLASH_SUCCESS;
}

uint8_t spif_read(uint32_t udAddr, void * buf, uint32_t len)
{
	uint8_t *p = (uint8_t *) buf;
	memset(p, 0, len);

	uint8_t * dummy_tx = (uint8_t *) pvPortMalloc(len);
	memset(dummy_tx, 0, len);

	if (spif_wait() == FLASH_TIMEOUT) return FLASH_TIMEOUT;

	CSASSERT();

	spi_transfer(SPIF_CMD_READ_4BYTE_ADDR);
	spi_transfer(udAddr >> 24);
	spi_transfer(udAddr >> 16);
	spi_transfer(udAddr >> 8);
	spi_transfer(udAddr);

	spi_dma_transfer(dummy_tx, p, len);

	vPortFree(dummy_tx);

	CSRELEASE();

	return FLASH_SUCCESS;
}

uint8_t spif_page_write(uint32_t udAddr, void *buf, uint32_t len)
{
	uint8_t *p = (uint8_t *) buf;

	if (len > 256) return FLASH_ERROR;
	if (spif_write_enable() == FLASH_TIMEOUT) return FLASH_TIMEOUT;

	CSASSERT();

//	uint8_t tx_data[5] = { 0 };
//	uint8_t rx_data[5] = { 0 };
//
//	tx_data[0] = SPIF_CMD_PROGRAM_4BYTE_ADDR;
//	tx_data[1] = udAddr >> 24;
//	tx_data[2] = udAddr >> 16;
//	tx_data[3] = udAddr >> 8;
//	tx_data[4] = udAddr;
//
//	spi_dma_transfer(tx_data, rx_data, 5);

	spi_transfer(SPIF_CMD_PROGRAM_4BYTE_ADDR);
	spi_transfer(udAddr >> 24);
	spi_transfer(udAddr >> 16);
	spi_transfer(udAddr >> 8);
	spi_transfer(udAddr);

	spi_dma_transfer(p, dummy_rx, len);

	CSRELEASE();

	return spif_wait();
}

uint8_t spif_write(uint32_t udAddr, void *buf, uint32_t len)
{
	uint8_t *pArray = (uint8_t *) buf;
	uint16_t dataOffset;
	uint8_t ret_val;

	if (spif_write_enable() == FLASH_TIMEOUT) return FLASH_TIMEOUT;

	dataOffset = (FLASH_PAGE_SIZE - (udAddr & FLASH_ADDRESS_MASK) ) & FLASH_ADDRESS_MASK;
	if (dataOffset > len) dataOffset = len;

	if (dataOffset > 0) {
		ret_val = spif_page_write(udAddr, pArray, dataOffset);
		if (FLASH_SUCCESS != ret_val)
			return ret_val;
	}

	for ( ; (dataOffset + FLASH_PAGE_SIZE) < len; dataOffset += FLASH_PAGE_SIZE) {
		ret_val = spif_page_write(udAddr + dataOffset, pArray + dataOffset, FLASH_PAGE_SIZE);
		if (FLASH_SUCCESS != ret_val)
			return ret_val;
	}

	if (len > dataOffset) {
		ret_val = spif_page_write(udAddr + dataOffset, pArray + dataOffset, (len - dataOffset));
	}

	return ret_val;
}


uint8_t spif_erase_page(uint32_t udAddr)
{
	if (spif_write_enable() == FLASH_TIMEOUT) return FLASH_TIMEOUT;

	CSASSERT();
	spi_transfer(SPIF_CMD_ERASE_SUBSECTOR);
	spi_transfer(udAddr >> 24);
	spi_transfer(udAddr >> 16);
	spi_transfer(udAddr >> 8);
	spi_transfer(udAddr);
	CSRELEASE();

	return spif_wait();
}

uint8_t spif_erase_all(void)
{
	if (spif_write_enable() == FLASH_TIMEOUT) return FLASH_TIMEOUT;

	CSASSERT();
	spi_transfer(SPIF_CMD_BULK_ERASE);
	CSRELEASE();

	return FLASH_OP_IN_PROGRESS;
}

static uint8_t random(void)
{
	return rand() % 256;
}

#define DATA_SIZE	768
void spi_flash_tests(void)
{
	uint8_t data[DATA_SIZE] = { 0 };
	uint8_t verification[DATA_SIZE] = { 0 };
	for (int i = 0; i < DATA_SIZE; i++) data[i] = random();

	spif_erase_page(0);

	while(spif_busy());

	printf("ret %d\n", spif_write(0, data, DATA_SIZE));
	spif_read(0, verification, DATA_SIZE);

	for (int i = 0; i < DATA_SIZE; i++) {
		if (data[i] != verification[i]) {
			printf("%d -> %d == %d\n", i, data[i+256], verification[i]);
		}
	}
}
