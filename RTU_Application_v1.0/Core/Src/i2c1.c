/*
 * i2c1.c
 *
 *  Created on: Aug 16, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "i2c1.h"

void I2C1_Init(void) {
	// Enable Clocks
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	// Enable I2C1 peripheral
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// Configure GPIO for I2C1
	GPIOB->MODER &= ~((3UL << (6 * 2)) | (3UL << (7 * 2)));  // Clear mode bits
	GPIOB->MODER |= ((2UL << (6 * 2)) | (2UL << (7 * 2)));  // Set AF mode

	// Configure open drain
	GPIOB->OTYPER |= ((1UL << 6) | (1UL << 7));

	// Set I2C speed
	GPIOB->OSPEEDR |= ((3UL << (6 * 2)) | (3UL << (7 * 2)));

	// Do not use internal pullup
	GPIOB->PUPDR &= ~((3UL << (6 * 2)) | (3UL << (7 * 2)));  // 00 = floating

	// Alternate function
	GPIOB->AFR[0] &= ~((0xFUL << (6 * 4)) | (0xFUL << (7 * 4)));  // Clear
	GPIOB->AFR[0] |= ((4UL << (6 * 4)) | (4UL << (7 * 4)));       // AF4 = I2C1

	// I2C1 Configuration
	I2C1->CR1 &= ~I2C_CR1_PE;	// Disable I2C1 Peripheral

	// Peripheral clock frequency (APB1) in MHz
	I2C1->CR2 = 50;

	// Clock control for 100 kHz Standard Mode
	I2C1->CCR = 250;

	// Maximum rise time
	I2C1->TRISE = 51;

	// Enable ACK
	I2C1->CR1 = I2C_CR1_ACK;

	// Enable I2C1 peripheral
	I2C1->CR1 |= I2C_CR1_PE;
}

void I2C1_Start(void) {
	uint32_t timeout = 10000;

	// Wait for bus to be free
	while (I2C1->SR2 & I2C_SR2_BUSY) {
		if (--timeout == 0)
			break;
	}

	// Generate start condition
	I2C1->CR1 |= I2C_CR1_START;

	// Wait for start condition generated (SB bit)
	timeout = 10000;
	while (!(I2C1->SR1 & I2C_SR1_SB)) {
		if (--timeout == 0)
			break;
	}
}

void I2C1_Stop(void) {
	// Generate stop condition
	I2C1->CR1 |= I2C_CR1_STOP;

	// Small delay to let STOP complete
	for (volatile int i = 0; i < 100; i++)
		;
}

uint8_t I2C1_SendAddr(uint8_t addr, uint8_t rw) {
	uint32_t timeout = 10000;

	// Clear any pending acknowledge failure
	I2C1->SR1 &= ~I2C_SR1_AF;

	// Send address (shifted left 1 bit + R/W bit)
	I2C1->DR = (addr << 1) | rw;

	// Wait for ADDR flag (address sent and ACK received)
	while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
		// Check for acknowledge failure
		if (I2C1->SR1 & I2C_SR1_AF) {
			return I2C_ERROR;  // No ACK from device
		}

		if (--timeout == 0) {
			return I2C_TIMEOUT;
		}
	}

	// Clear ADDR flag by reading SR2 register
	(void) I2C1->SR2;

	return I2C_OK;
}

uint8_t I2C1_WriteByte(uint8_t data) {
	uint32_t timeout = 10000;

	// Wait for TXE flag (data register empty)
	while (!(I2C1->SR1 & I2C_SR1_TXE)) {
		// Check for acknowledge failure
		if (I2C1->SR1 & I2C_SR1_AF) {
			return I2C_ERROR;
		}

		if (--timeout == 0) {
			return I2C_TIMEOUT;
		}
	}

	// Send data
	I2C1->DR = data;

	// Wait for byte to be transmitted (TXE set again)
	timeout = 10000;
	while (!(I2C1->SR1 & I2C_SR1_TXE)) {
		// Check for acknowledge failure
		if (I2C1->SR1 & I2C_SR1_AF) {
			return I2C_ERROR;
		}

		if (--timeout == 0) {
			return I2C_TIMEOUT;
		}
	}

	return I2C_OK;
}

uint8_t I2C1_ReadByte(uint8_t ack) {
	uint32_t timeout = 10000;

	// Configure ACK/NACK for this byte
	if (ack) {
		I2C1->CR1 |= I2C_CR1_ACK;   // Send ACK after receiving
	} else {
		I2C1->CR1 &= ~I2C_CR1_ACK;  // Send NACK after receiving
	}

	// Wait for RXNE flag (data received)
	while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
		if (--timeout == 0) {
			return 0;
		}
	}

	// Read received data
	return I2C1->DR;
}
