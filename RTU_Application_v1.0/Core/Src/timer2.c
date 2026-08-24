/*
 * timer2.c
 *
 *  Created on: Aug 24, 2026
 *      Author: Rubin Khadka
 */

#include "main.h"
#include "timer2.h"

static volatile uint32_t system_millis = 0;

void TIMER2_Init(void) {
	// Enable TIM2 clock (APB1)
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Small delay for clock to stabilize
	for (volatile int i = 0; i < 10; i++)
		;

	// Configure for 1 µs resolution at 100 MHz
	// Prescaler = 100 - 1 = 99 (100 MHz / 100 = 1 MHz)
	TIM2->PSC = 99;

	// Auto-reload for 1 ms (1000 ticks at 1 MHz)
	TIM2->ARR = 999;

	// Clear counter
	TIM2->CNT = 0;

	// Clear update flag
	TIM2->SR &= ~TIM_SR_UIF;

	// Enable update interrupt
	TIM2->DIER |= TIM_DIER_UIE;

	// Enable TIM2 interrupt in NVIC
	NVIC_SetPriority(TIM2_IRQn, 1);
	NVIC_EnableIRQ(TIM2_IRQn);

	// Start timer
	TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {
		TIM2->SR &= ~TIM_SR_UIF;
		system_millis++;
	}
}

uint32_t TIMER2_GetMillis(void) {
	uint32_t ms;
	__disable_irq();
	ms = system_millis;
	__enable_irq();
	return ms;
}

void TIMER2_Delay_ms(uint32_t ms) {
	uint32_t start = TIMER2_GetMillis();
	while ((TIMER2_GetMillis() - start) < ms) {
		// wait
	}
}
