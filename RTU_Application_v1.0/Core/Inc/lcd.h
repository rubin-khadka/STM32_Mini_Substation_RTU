/*
 * lcd.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdint.h>

// I2C address of PCF8574 backpack
#define LCD_ADDR 0x27

void LCD_Init(void);
void LCD_SendCmd(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);

void LCD_DisplayReading_Temp(uint8_t temp_int, uint8_t temp_dec, uint8_t hum_int, uint8_t hum_dec);
void LCD_DisplayFloat(float value, uint8_t decimal_places);
void LCD_DisplayAccelScaled(float ax, float ay, float az);
void LCD_DisplayGyroScaled(float gx, float gy, float gz);

#endif /* INC_LCD_H_ */
