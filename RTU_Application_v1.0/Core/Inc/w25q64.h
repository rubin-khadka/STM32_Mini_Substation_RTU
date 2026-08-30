/*
 * w25q64.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_W25Q64_H_
#define INC_W25Q64_H_

/* W25Q64 Commands */
#define W25Q64_CMD_WRITE_ENABLE      0x06U
#define W25Q64_CMD_WRITE_DISABLE     0x04U
#define W25Q64_CMD_READ_STATUS_1     0x05U
#define W25Q64_CMD_READ_DATA         0x03U
#define W25Q64_CMD_PAGE_PROGRAM      0x02U
#define W25Q64_CMD_SECTOR_ERASE      0x20U
#define W25Q64_CMD_BLOCK_ERASE_64K   0xD8U
#define W25Q64_CMD_CHIP_ERASE        0xC7U
#define W25Q64_CMD_READ_JEDEC_ID     0x9FU

/* Public Functions */
void W25Q64_ReadJEDEC_ID(uint8_t *manufacturerID, uint8_t *memoryType,
		uint8_t *capacity);
uint8_t W25Q64_ReadStatus(void);
void W25Q64_WaitBusy(void);
void W25Q64_WriteEnable(void);
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t length);
void W25Q64_SectorErase(uint32_t addr);
void W25Q64_ChipErase(void);
void W25Q64_PageProgram(uint32_t addr, uint8_t *data, uint16_t length);
void W25Q64_WriteData(uint32_t addr, uint8_t *data, uint32_t length);

// Public functions
void W25Q64_ReadJEDEC_ID(uint8_t *manufacturerID, uint8_t *memoryType,
		uint8_t *capacity);

#endif /* INC_W25Q64_H_ */
