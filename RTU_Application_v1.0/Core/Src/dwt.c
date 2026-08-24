/*
 * dwt.c
 *
 *  Created on: Aug 24, 2026
 *      Author: Rubin Khadka
 */

#include "dwt.h"
#include "main.h"

// Initialize DWT cycle counter
void DWT_Init(void) {
	// Enable trace and debug
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	// Reset cycle counter
	DWT->CYCCNT = 0;

	// Enable cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// Microsecond delay using DWT
void DWT_Delay_us(uint32_t us) {
    uint32_t cycles = (uint32_t)(((uint64_t)SystemCoreClock * us) / 1000000);
    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cycles);
}
