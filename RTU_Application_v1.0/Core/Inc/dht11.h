/*
 * dht11.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

void DHT11_Init(void);
int DHT11_Start(void);
int DHT11_Check_Response(void);
uint8_t DHT11_Read(void);

#endif /* INC_DHT11_H_ */
