/*
 * dwt.c
 *
 *  Created on: Aug 24, 2026
 *      Author: Rubin Khadka
 */

#include "dwt.h"
#include "main.h"

// DWT cycle counter
#define DWT_CYCCNT_R  (DWT->CYCCNT)

static volatile uint32_t tick_counter = 0;

// Initialize DWT cycle counter
void DWT_Init(void) {
	// Enable trace and debug
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	// Reset cycle counter
	DWT->CYCCNT = 0;

	// Enable cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	tick_counter = 0; // Initialize Counter
}

// Get current tick count (milliseconds)
uint32_t DWT_GetTick(void) {
	return tick_counter + (DWT_CYCCNT_R / (SystemCoreClock / 1000));
}

// Delay for microseconds using DWT
void DWT_Delay_us(uint32_t us)
{
  uint32_t cycles = (uint32_t)(((uint64_t)SystemCoreClock * us) / 1000000);
  uint32_t start = DWT->CYCCNT;
  while ((DWT->CYCCNT - start) < cycles);
}

// Delay for milliseconds using DWT
void DWT_Delay_ms(uint32_t ms) {
	while (ms--) {
		DWT_Delay_us(1000);
		tick_counter++;
	}
}
