/*
 * gpio.c
 *
 *  Created on: Aug 24, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "gpio.h"

void GPIO_Init(void) {
	// Enable GPIOA clock (already enabled, but ensure)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Configure PA5 as push-pull output
	GPIOA->MODER &= ~(3UL << (5U * 2U));   // Clear mode bits
	GPIOA->MODER |= (1UL << (5U * 2U));    // Output mode (01)

	GPIOA->OTYPER &= ~(1UL << 5U);         // Push-pull
	GPIOA->OSPEEDR |= (3UL << (5U * 2U));  // High speed
}
