/*
 * timer2.h
 *
 *  Created on: Aug 24, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_TIMER2_H_
#define INC_TIMER2_H_

#include <stdint.h>

void TIMER2_Init(void);
uint32_t TIMER2_GetMillis(void);
void TIMER2_Delay_ms(uint32_t ms);

#endif /* INC_TIMER2_H_ */
