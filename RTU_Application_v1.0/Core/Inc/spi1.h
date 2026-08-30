/*
 * spi1.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_SPI1_H_
#define INC_SPI1_H_

#include <stdint.h>

/* SPI Status */
typedef enum {
	SPI_OK = 0, SPI_ERR_TIMEOUT, SPI_ERR_INVALID_PARAM
} SPI_Status;

/* Timeout value for SPI operations */
#define SPI_TIMEOUT     100000U

/* Chip Select pin: PA4 */
#define SPI_CS_PIN      4U
#define SPI_CS_PORT     GPIOA

/* Public Functions */
void SPI1_Init(void);
void SPI1_CS_Select(void);
void SPI1_CS_Deselect(void);

SPI_Status SPI1_TransferByte(uint8_t txData, uint8_t *rxData);
SPI_Status SPI1_Transmit(uint8_t *data, uint32_t length);
SPI_Status SPI1_Receive(uint8_t *data, uint32_t length);
SPI_Status SPI1_TransmitReceive(uint8_t *txData, uint8_t *rxData,
		uint32_t length);

#endif /* INC_SPI1_H_ */
