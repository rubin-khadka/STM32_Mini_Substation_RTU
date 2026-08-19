/*
 * usart1.h
 *
 *  Created on: Aug 19, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_USART1_H_
#define INC_USART1_H_

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

/* Buffer structure */
typedef struct {
	volatile uint8_t *buffer;
	volatile uint16_t size;
	volatile uint16_t head;
	volatile uint16_t tail;
	volatile uint16_t count;
} USART1_Buffer_t;

/* Global buffer instances */
extern volatile USART1_Buffer_t usart1_rx_buf;
extern volatile USART1_Buffer_t usart1_tx_buf;

/* Function prototypes */
void USART1_Init(void);
void UART1_BufferInit(volatile USART1_Buffer_t *buff, uint8_t *storage,
		uint16_t size);
bool USART1_BufferEmpty(volatile USART1_Buffer_t *buff);
bool USART1_BufferFull(volatile USART1_Buffer_t *buff);
bool USART1_BufferWrite(volatile USART1_Buffer_t *buff, uint8_t data);
uint8_t USART1_BufferRead(volatile USART1_Buffer_t *buff);
bool USART1_DataAvailable(void);
void USART1_SendChar(char c);
void USART1_SendString(const char *str);
uint8_t USART1_GetChar(void);
void USART1_SendNumber(uint32_t num);
void USART1_SendHex(uint8_t value);

#endif /* INC_USART1_H_ */
