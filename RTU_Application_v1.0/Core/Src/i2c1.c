/*
 * i2c1.c
 *
 *  Created on: Aug 16, 2026
 *      Author: Rubin Khadka
 */

#include "i2c1.h"
#include "main.h"

void I2C1_Init(void) {
	// Enable Clocks
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN ;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// Configure GPIO
	GPIOB->MODER &= ~((3UL << (6U * 2U)) | (3UL << (7U * 2U)));
	GPIOB->MODER |= ((2UL << (6U * 2U)) | (2UL << (7U * 2U)));

	// Open Drain
	GPIOB->OTYPER |= ((1UL << 6U) | (1UL << 7U));

	// High Speed
	GPIOB->OSPEEDR |= ((3UL << (6U * 2U)) | (3UL << (7U * 2U)));

	// No internal pull up
	GPIOB->PUPDR &= ~((3UL << (6U * 2U)) | (3UL << (7U * 2U)));

	// Alternate function for I2C
	GPIOB->AFR[0] &= ~((0xFUL << (6U * 4U)) | (0xFUL << (7U * 4U)));
	GPIOB->AFR[0] |= ((4UL << (6U * 4U)) | (4UL << (7U * 4U)));

	// Disable before peripheral configuration
	I2C1->CR1 &= ~I2C_CR1_PE;

	I2C1->CR2 = 50U; 	// 50 MHz
	I2C1->CCR = 250U; 	// 100 KHz
	I2C1->TRISE = 51U; 	// 50 + 1

	I2C1->CR1 = I2C_CR1_ACK;	// Enable ACK
	I2C1->CR1 |= I2C_CR1_PE;	// Enable Peripheral
}

// Start Condition
I2C_Status I2C1_Start(void) {
	uint32_t timeout = I2C_TIMEOUT;

	/* Wait until bus is free */
	while (I2C1->SR2 & I2C_SR2_BUSY) {
		if (--timeout == 0U) {
			return I2C_ERR_BUSY;
		}
	}

	/* Generate START */
	I2C1->CR1 |= I2C_CR1_START;

	/* Wait for Start Bit */
	timeout = I2C_TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_SB)) {
		if (--timeout == 0U) {
			return I2C_ERR_START;
		}
	}
	return I2C_OK;
}

// Stop Condition
void I2C1_Stop(void) {
	I2C1->CR1 |= I2C_CR1_STOP;

	/* Wait for bus to go idle */
	uint32_t timeout = I2C_TIMEOUT;
	while (I2C1->SR2 & I2C_SR2_BUSY) {
		if (--timeout == 0U) {
			break;
		}
	}
}

// Send I2C Address
I2C_Status I2C1_SendAddr(uint8_t addr, uint8_t rw) {
	uint32_t timeout = I2C_TIMEOUT;

	/* Clear pending ACK failure flag */
	I2C1->SR1 &= ~I2C_SR1_AF;

	I2C1->DR = (uint8_t) ((addr << 1U) | (rw & 0x01U));

	/* Wait for ADDR */
	while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
		if (I2C1->SR1 & I2C_SR1_AF) {
			I2C1->CR1 |= I2C_CR1_STOP; // Release Bus
			return I2C_ERR_NACK;
		}
		if (--timeout == 0U) {
			I2C1->CR1 |= I2C_CR1_STOP; // Release Bus
			return I2C_ERR_TIMEOUT;
		}
	}

	/* Clear ADDR: read SR1 then SR2 */
	(void) I2C1->SR1;
	(void) I2C1->SR2;

	return I2C_OK;
}

// Write Single Byte
I2C_Status I2C1_WriteByte(uint8_t data) {
	uint32_t timeout = I2C_TIMEOUT;

	while (!(I2C1->SR1 & I2C_SR1_TXE)) {
		if (I2C1->SR1 & I2C_SR1_AF) {
			I2C1->CR1 |= I2C_CR1_STOP;
			return I2C_ERR_NACK;
		}
		if (--timeout == 0U) {
			I2C1->CR1 |= I2C_CR1_STOP;
			return I2C_ERR_TIMEOUT;
		}
	}

	I2C1->DR = data;
	return I2C_OK;
}

// Write Byte and Wait
I2C_Status I2C1_WriteByteWaitComplete(uint8_t data) {
	I2C_Status status = I2C1_WriteByte(data);
	if (status != I2C_OK) {
		return status;
	}

	uint32_t timeout = I2C_TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_BTF)) {
		if (I2C1->SR1 & I2C_SR1_AF) {
			I2C1->CR1 |= I2C_CR1_STOP;
			return I2C_ERR_NACK;
		}
		if (--timeout == 0U) {
			I2C1->CR1 |= I2C_CR1_STOP;
			return I2C_ERR_TIMEOUT;
		}
	}
	return I2C_OK;
}

// Brust Read
I2C_Status I2C1_ReadBurst(uint8_t devAddr, uint8_t regAddr, uint8_t *buf,
		uint8_t len) {
	I2C_Status status;
	uint32_t timeout;

	if (len == 0U || buf == NULL) {
		return I2C_ERR_TIMEOUT;
	}

	/* Write register pointer */
	status = I2C1_Start();
	if (status != I2C_OK)
		return status;

	status = I2C1_SendAddr(devAddr, 0U);
	if (status != I2C_OK)
		return status;

	status = I2C1_WriteByteWaitComplete(regAddr);
	if (status != I2C_OK)
		return status;

	/* Repeated START + Read */
	status = I2C1_Start();
	if (status != I2C_OK)
		return status;

	status = I2C1_SendAddr(devAddr, 1U);
	if (status != I2C_OK)
		return status;

	/* Receive bytes */
	if (len == 1U) {
		I2C1->CR1 &= ~I2C_CR1_ACK;
		I2C1->CR1 |= I2C_CR1_STOP;

		timeout = I2C_TIMEOUT;
		while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
			if (--timeout == 0U)
				return I2C_ERR_TIMEOUT;
		}
		buf[0] = (uint8_t) I2C1->DR;

	} else if (len == 2U) {
		I2C1->CR1 |= I2C_CR1_POS;
		I2C1->CR1 &= ~I2C_CR1_ACK;

		timeout = I2C_TIMEOUT;
		while (!(I2C1->SR1 & I2C_SR1_BTF)) {
			if (--timeout == 0U)
				return I2C_ERR_TIMEOUT;
		}

		I2C1->CR1 |= I2C_CR1_STOP;
		buf[0] = (uint8_t) I2C1->DR;
		buf[1] = (uint8_t) I2C1->DR;

		I2C1->CR1 &= ~I2C_CR1_POS;
	} else {
		uint8_t i;
		for (i = 0U; i < len - 3U; i++) {
			timeout = I2C_TIMEOUT;
			while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
				if (--timeout == 0U)
					return I2C_ERR_TIMEOUT;
			}
			buf[i] = (uint8_t) I2C1->DR;
		}

		/* Wait for BTF: byte N-2 in DR, byte N-1 in shift register */
		timeout = I2C_TIMEOUT;
		while (!(I2C1->SR1 & I2C_SR1_BTF)) {
			if (--timeout == 0U)
				return I2C_ERR_TIMEOUT;
		}

		/* NACK the last byte, read byte N-2 */
		I2C1->CR1 &= ~I2C_CR1_ACK;
		buf[len - 3U] = (uint8_t) I2C1->DR;

		/* Wait for RXNE: byte N-1 ready */
		timeout = I2C_TIMEOUT;
		while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
			if (--timeout == 0U)
				return I2C_ERR_TIMEOUT;
		}
		buf[len - 2U] = (uint8_t) I2C1->DR;

		/* STOP before reading last byte (byte N) */
		I2C1->CR1 |= I2C_CR1_STOP;
		buf[len - 1U] = (uint8_t) I2C1->DR;
	}

	return I2C_OK;
}
