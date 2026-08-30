/*
 * spi1.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "spi1.h"

// SPI Initialization
void SPI1_Init(void) {
	// Enable Clocks
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;       	// GPIOA clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;         // SPI1 clock

	// Configure GPIO for SPI
	GPIOA->MODER &= ~((3UL << (5U * 2U)) | (3UL << (6U * 2U))
			| (3UL << (7U * 2U)));
	GPIOA->MODER |= ((2UL << (5U * 2U)) | (2UL << (6U * 2U))
			| (2UL << (7U * 2U)));

	// Push-Pull
	GPIOA->OTYPER &= ~((1UL << 5U) | (1UL << 6U) | (1UL << 7U));

	// High Speed
	GPIOA->OSPEEDR &= ~((3UL << (5U * 2U)) | (3UL << (6U * 2U))
			| (3UL << (7U * 2U)));
	GPIOA->OSPEEDR |= ((3UL << (5U * 2U)) | (3UL << (6U * 2U))
			| (3UL << (7U * 2U)));

	// No Pull-up / Pull-down
	GPIOA->PUPDR &= ~((3UL << (5U * 2U)) | (3UL << (6U * 2U))
			| (3UL << (7U * 2U)));

	// Alternate Function 5 (AF5) for SPI1
	GPIOA->AFR[0] &= ~((0xFUL << (5U * 4U)) | (0xFUL << (6U * 4U))
			| (0xFUL << (7U * 4U)));
	GPIOA->AFR[0] |= ((5UL << (5U * 4U)) | (5UL << (6U * 4U))
			| (5UL << (7U * 4U)));

	// Configure CS pin (PA4) as GPIO Output
	GPIOA->MODER &= ~(3UL << (SPI_CS_PIN * 2U));
	GPIOA->MODER |= (1UL << (SPI_CS_PIN * 2U));    // General purpose output

	GPIOA->OTYPER &= ~(1UL << SPI_CS_PIN);           // Push-pull

	GPIOA->OSPEEDR &= ~(3UL << (SPI_CS_PIN * 2U));
	GPIOA->OSPEEDR |= (3UL << (SPI_CS_PIN * 2U));   // High speed

	// CS starts HIGH (deselected)
	GPIOA->BSRR = (1UL << SPI_CS_PIN);

	// Disable SPI before configuration
	SPI1->CR1 &= ~SPI_CR1_SPE;

	// Configure SPI1 Control Register 1 (CR1)
	SPI1->CR1 = 0U;    // Clear everything first

	// Master mode
	SPI1->CR1 |= SPI_CR1_MSTR;

	// PCLK2 = 100 MHz , prescaler 32 -> SPI clock = 3.125 MHz
	SPI1->CR1 |= (4UL << SPI_CR1_BR_Pos);

	// CPOL = 0 CPHA = 0
	SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

	// MSB first
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

	// 8-bit data frame
	SPI1->CR1 &= ~SPI_CR1_DFF;

	// Software slave management
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;    // Internal slave select high (inactive)

	// Configure SPI1 Control Register 2 (CR2)
	SPI1->CR2 = 0U;

	// Enable SPI peripheral
	SPI1->CR1 |= SPI_CR1_SPE;
}

// Select the SPI slave (pull CS low)
void SPI1_CS_Select(void) {
	GPIOA->BSRR = (1UL << (SPI_CS_PIN + 16U));   // Reset = low
}

// Deselect the SPI slave (pull CS high)
void SPI1_CS_Deselect(void) {
	GPIOA->BSRR = (1UL << SPI_CS_PIN);           // Set = high
}

// Transfer a single byte (full-duplex)
SPI_Status SPI1_TransferByte(uint8_t txData, uint8_t *rxData) {
	uint32_t timeout = SPI_TIMEOUT;

	/* Wait until TX buffer is empty */
	while (!(SPI1->SR & SPI_SR_TXE)) {
		if (--timeout == 0U) {
			return SPI_ERR_TIMEOUT;
		}
	}

	/* Write data to data register */
	*(volatile uint8_t*) &SPI1->DR = txData;

	/* Wait until RX buffer is not empty */
	timeout = SPI_TIMEOUT;
	while (!(SPI1->SR & SPI_SR_RXNE)) {
		if (--timeout == 0U) {
			return SPI_ERR_TIMEOUT;
		}
	}

	/* Read received data */
	uint8_t received = (uint8_t) (*(volatile uint8_t*) &SPI1->DR);
	if (rxData != NULL) {
		*rxData = received;
	}

	return SPI_OK;
}

// Transmit multiple bytes (ignore received data)
SPI_Status SPI1_Transmit(uint8_t *data, uint32_t length) {
	if (data == NULL || length == 0U) {
		return SPI_ERR_INVALID_PARAM;
	}

	for (uint32_t i = 0U; i < length; i++) {
		SPI_Status status = SPI1_TransferByte(data[i], NULL);
		if (status != SPI_OK) {
			return status;
		}
	}
	return SPI_OK;
}

// Receive multiple bytes (send dummy byte 0xFF)
SPI_Status SPI1_Receive(uint8_t *data, uint32_t length) {
	if (data == NULL || length == 0U) {
		return SPI_ERR_INVALID_PARAM;
	}

	for (uint32_t i = 0U; i < length; i++) {
		SPI_Status status = SPI1_TransferByte(0xFFU, &data[i]);
		if (status != SPI_OK) {
			return status;
		}
	}
	return SPI_OK;
}

// Full-duplex transfer (transmit and receive simultaneously)
SPI_Status SPI1_TransmitReceive(uint8_t *txData, uint8_t *rxData,
		uint32_t length) {
	if ((txData == NULL && rxData == NULL) || length == 0U) {
		return SPI_ERR_INVALID_PARAM;
	}

	for (uint32_t i = 0U; i < length; i++) {
		uint8_t tx_byte = (txData != NULL) ? txData[i] : 0xFFU;
		uint8_t rx_byte = 0U;

		SPI_Status status = SPI1_TransferByte(tx_byte, &rx_byte);
		if (status != SPI_OK) {
			return status;
		}

		if (rxData != NULL) {
			rxData[i] = rx_byte;
		}
	}
	return SPI_OK;
}
