/*
 * dht11.c
 *
 *  Created on: Aug 25, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "dwt.h"
#include "timer2.h"

// Pin definitions
#define DHT11_GPIO      GPIOB
#define DHT11_PIN       0

// Pin operations
#define DHT11_HIGH()    (DHT11_GPIO->BSRR = GPIO_BSRR_BS_0)
#define DHT11_LOW()     (DHT11_GPIO->BSRR = GPIO_BSRR_BR_0)
#define DHT11_READ()    ((DHT11_GPIO->IDR & GPIO_IDR_ID0) ? 1 : 0)

void DHT11_Init(void) {
	// Enable GPIOB clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	// Configure PB0 as output push-pull, high speed, no pull
	GPIOB->MODER &= ~(3UL << (DHT11_PIN * 2));
	GPIOB->MODER |= (1UL << (DHT11_PIN * 2));  	// Output mode (01)

	GPIOB->OTYPER &= ~(1UL << DHT11_PIN);      	// Push-pull (0)
	GPIOB->OSPEEDR |= (3UL << (DHT11_PIN * 2)); // High speed (11)
	GPIOB->PUPDR &= ~(3UL << (DHT11_PIN * 2)); 	// No pull-up/pull-down

	DHT11_HIGH();
}

void DHT11_Start(void) {
	// Set as output
	GPIOB->MODER &= ~(3UL << (DHT11_PIN * 2));
	GPIOB->MODER |= (1UL << (DHT11_PIN * 2));  // Output

	// Pull LOW for 18 ms
	DHT11_LOW();
	TIMER2_Delay_ms(18);

	// Pull HIGH for 20 µs
	DHT11_HIGH();
	DWT_Delay_us(20);

	// Set as input floating
	GPIOB->MODER &= ~(3UL << (DHT11_PIN * 2)); 		// Input mode (00)
	// GPIOB->PUPDR |= (1UL << (DHT11_PIN * 2)); 	// Pull-up
}

int DHT11_Check_Response(void) {
	DWT_Delay_us(40);

	// Check if sensor pulled LOW
	if (!DHT11_READ()) {
		DWT_Delay_us(80);  // Wait 80 µs (LOW pulse)

		if (DHT11_READ())  // Should be HIGH now
				{
			// Wait for HIGH to end
			uint32_t timeout = 500;
			while (DHT11_READ()) {
				if (--timeout == 0)
					return 0;
			}
			return 1;  // Response OK
		}
	}
	return 0;  // No response
}

uint8_t DHT11_Read(void) {
	uint8_t data = 0;
	uint32_t timeout;

	for (int bit = 7; bit >= 0; bit--) {
		// Wait for pin to go HIGH (start of bit)
		timeout = 500;
		while (!DHT11_READ()) {
			if (--timeout == 0)
				return 0;
		}

		// Wait 40 µs into the HIGH pulse
		DWT_Delay_us(40);

		// Check if pin is still HIGH
		if (DHT11_READ()) {
			// Still HIGH after 40 µs = this is a 1 (70 µs pulse)
			data |= (1 << bit);

			// Wait for the rest of the HIGH pulse to end
			timeout = 500;
			while (DHT11_READ()) {
				if (--timeout == 0)
					return 0;
			}
		} else {
			// Pin went LOW already = this is a 0 (26 µs pulse)
			// No need to set bit, just wait for next bit
			// The pin is already LOW, so next bit will start soon
		}
	}

	return data;
}
