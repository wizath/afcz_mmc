/*
 * xr77192.h
 *
 *  Created on: Nov 26, 2018
 *      Author: wizath
 */

#ifndef INCLUDE_BOARD_XR77192_H_
#define INCLUDE_BOARD_XR77192_H_

/*
 * xr77129.c
 *
 *  Created on: Nov 26, 2018
 *      Author: wizath
 */

#define HW_ID_XR77129		4868

typedef struct {
	uint16_t address;
	uint8_t data;
} xr_data;

void xr77129_runtime_write(uint16_t address, uint8_t data);
void xr77129_runtime_read(uint16_t address, uint8_t * data);
void xr77129_set_power_chip_ready(uint8_t val);
uint16_t xr77129_get_power_chip_ready(void);
void xr77129_power_restart(void);
uint8_t xr77129_runtime_load(xr_data * exar_data, uint32_t len);
void xr77129_read_value(uint8_t reg_address, uint16_t *read );
void xr77129_write_value(uint8_t reg_address, uint16_t value );
void xr77129_load_runtimes(void);

#endif /* INCLUDE_BOARD_XR77192_H_ */
