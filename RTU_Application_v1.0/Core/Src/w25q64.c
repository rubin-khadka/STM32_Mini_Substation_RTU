/*
 * w25q64.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "w25q64.h"
#include "spi1.h"

void W25Q64_ReadJEDEC_ID(uint8_t *manufacturerID, uint8_t *memoryType,
		uint8_t *capacity) {
	uint8_t cmd = W25Q64_CMD_READ_JEDEC_ID;
	uint8_t rx[3] = { 0 };

	// Select flash (CS low)
	SPI1_CS_Select();

	// Send command byte
	SPI1_Transmit(&cmd, 1U);

	// Read 3 bytes (dummy clocks)
	SPI1_Receive(rx, 3U);

	// Deselect flash (CS high)
	SPI1_CS_Deselect();

	// Extract IDs
	*manufacturerID = rx[0];
	*memoryType = rx[1];
	*capacity = rx[2];
}
