/*
 * spi_flash.h
 *
 *  Created on: Jan 24, 2018
 *      Author: wizath
 */

#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_

#include "config.h"

#define TEST_LENGTH						1024

#define SPIF_CMD_GET_IDENTIFICATION		0x9F
#define SPIF_CMD_READ_4BYTE_ADDR		0x13
#define SPIF_CMD_PROGRAM_4BYTE_ADDR		0x12
#define SPIF_CMD_WRITE_ENABLE			0x06
#define SPIF_CMD_WRITE_DISABLE			0x04
#define SPIF_CMD_ERASE_SUBSECTOR		0x21
#define SPIF_CMD_BULK_ERASE				0xC7
#define SPIF_CMD_STATUS_REGISTER		0x05

#define FLASH_WAIT_TIMEOUT				1000
#define FLASH_ADDRESS_MASK  			0xFF
#define FLASH_PAGE_SIZE					0x100

#define FLASH_SUCCESS					0
#define FLASH_ERROR						1
#define FLASH_TIMEOUT					2
#define FLASH_OP_IN_PROGRESS			3

uint8_t spif_read_status(uint8_t * status_reg);
bool spif_busy(void);
uint8_t spif_wait(void);
uint8_t spif_write_enable(void);
uint8_t spif_read(uint32_t addr, void * buf, uint32_t len);
uint8_t spif_page_write(uint32_t addr, void *buf, uint32_t len);
uint8_t spif_write(uint32_t udAddr, void *buf, uint32_t len);
uint8_t spif_erase_page(uint32_t udAddr);
uint8_t spif_erase_all(void);
void spi_flash_tests(void);

#endif /* SPI_FLASH_H_ */
