/*
 * w25q64.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_W25Q64_H_
#define INC_W25Q64_H_

// W25Q64 Commands
#define W25Q64_CMD_READ_JEDEC_ID     0x9FU

// Public functions
void W25Q64_ReadJEDEC_ID(uint8_t *manufacturerID, uint8_t *memoryType, uint8_t *capacity);

#endif /* INC_W25Q64_H_ */
