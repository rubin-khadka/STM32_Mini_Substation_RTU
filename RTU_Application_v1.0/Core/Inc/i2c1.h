/*
 * i2c1.h
 *
 *  Created on: Aug 16, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_I2C1_H_
#define INC_I2C1_H_

#include <stdint.h>

#define I2C_TIMEOUT     20000U

typedef enum {
	I2C_OK = 0,
	I2C_ERR_BUSY,
	I2C_ERR_START,
	I2C_ERR_NACK,
	I2C_ERR_TIMEOUT,
	I2C_ERR_BTF
} I2C_Status;

// Function Protoypes
void I2C1_Init(void);
I2C_Status I2C1_Start(void);
void I2C1_Stop(void);
I2C_Status I2C1_SendAddr(uint8_t addr, uint8_t rw);
I2C_Status I2C1_WriteByte(uint8_t data);
I2C_Status I2C1_WriteByteWaitComplete(uint8_t data);
I2C_Status I2C1_ReadBurst(uint8_t devAddr, uint8_t regAddr, uint8_t *buf,
		uint8_t len);

#endif /* INC_I2C1_H_ */
