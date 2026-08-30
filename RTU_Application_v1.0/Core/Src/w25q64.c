/*
 * w25q64.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "w25q64.h"
#include "spi1.h"

// Read the JEDEC ID.
void W25Q64_ReadJEDEC_ID(uint8_t *manufacturerID, uint8_t *memoryType,
		uint8_t *capacity) {
	uint8_t cmd = W25Q64_CMD_READ_JEDEC_ID;
	uint8_t rx[3] = { 0 };

	SPI1_CS_Select();
	SPI1_Transmit(&cmd, 1U);
	SPI1_Receive(rx, 3U);
	SPI1_CS_Deselect();

	*manufacturerID = rx[0];
	*memoryType = rx[1];
	*capacity = rx[2];
}

// Read the Status Register 1.
uint8_t W25Q64_ReadStatus(void) {
	uint8_t cmd = W25Q64_CMD_READ_STATUS_1;
	uint8_t status = 0U;

	SPI1_CS_Select();
	SPI1_Transmit(&cmd, 1U);
	SPI1_Receive(&status, 1U);
	SPI1_CS_Deselect();

	return status;
}

// Poll the status register until BUSY bit clears.
void W25Q64_WaitBusy(void) {
	while (W25Q64_ReadStatus() & 0x01U) {
		// Wait while BUSY is set
	}
}

// Send Write Enable command.
void W25Q64_WriteEnable(void) {
	uint8_t cmd = W25Q64_CMD_WRITE_ENABLE;
	SPI1_CS_Select();
	SPI1_Transmit(&cmd, 1U);
	SPI1_CS_Deselect();
}

// Read data from flash starting at address addr.
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t length) {
	uint8_t cmd[4] = {
	W25Q64_CMD_READ_DATA, (uint8_t) ((addr >> 16) & 0xFFU), (uint8_t) ((addr
			>> 8) & 0xFFU), (uint8_t) (addr & 0xFFU) };

	SPI1_CS_Select();
	SPI1_Transmit(cmd, 4U);
	SPI1_Receive(data, length);
	SPI1_CS_Deselect();
}

// Erase a 4 KB sector.
void W25Q64_SectorErase(uint32_t addr) {
	W25Q64_WriteEnable();

	uint8_t cmd[4] = {
	W25Q64_CMD_SECTOR_ERASE, (uint8_t) ((addr >> 16) & 0xFFU), (uint8_t) ((addr
			>> 8) & 0xFFU), (uint8_t) (addr & 0xFFU) };

	SPI1_CS_Select();
	SPI1_Transmit(cmd, 4U);
	SPI1_CS_Deselect();

	W25Q64_WaitBusy();   // erase takes some time
}

// Erase the entire chip
void W25Q64_ChipErase(void) {
	W25Q64_WriteEnable();

	uint8_t cmd = W25Q64_CMD_CHIP_ERASE;

	SPI1_CS_Select();
	SPI1_Transmit(&cmd, 1U);
	SPI1_CS_Deselect();

	W25Q64_WaitBusy();   // chip erase is slow
}

// Program up to 256 bytes (must not cross page boundary).
void W25Q64_PageProgram(uint32_t addr, uint8_t *data, uint16_t length) {
	if (length > 256U)
		length = 256U;   // limit to page size

	W25Q64_WriteEnable();

	uint8_t cmd[4] = {
	W25Q64_CMD_PAGE_PROGRAM, (uint8_t) ((addr >> 16) & 0xFFU), (uint8_t) ((addr
			>> 8) & 0xFFU), (uint8_t) (addr & 0xFFU) };

	SPI1_CS_Select();
	SPI1_Transmit(cmd, 4U);
	SPI1_Transmit(data, length);
	SPI1_CS_Deselect();

	W25Q64_WaitBusy();
}

//  Write arbitrary length data, handling sector erase and page boundaries.
void W25Q64_WriteData(uint32_t addr, uint8_t *data, uint32_t length) {
	uint32_t sector_start = addr & ~(0xFFFU);   // 4KB sector align
	uint32_t end_addr = addr + length;
	uint32_t current_sector = sector_start;

	// Erase all sectors that will be touched
	while (current_sector < end_addr) {
		W25Q64_SectorErase(current_sector);
		current_sector += 0x1000U;   // 4096 bytes
	}

	// Write data page by page
	uint32_t offset = 0U;
	while (offset < length) {
		uint32_t page_remaining = 256U - ((addr + offset) & 0xFFU);
		uint16_t chunk =
				(length - offset) < page_remaining ?
						(uint16_t) (length - offset) :
						(uint16_t) page_remaining;

		W25Q64_PageProgram(addr + offset, data + offset, chunk);
		offset += chunk;
	}
}
