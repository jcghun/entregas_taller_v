/*
 * LcdDriver.h
 *
 *  Created on: 6/11/2022
 *      Author: Juan Camilo Gomez Hidalgo
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"


/* Prototipos de las funciones */



void i2c_writeByteLcd(I2C_Handler_t *ptrHandlerI2C,  uint8_t newValue); //protocolo sin sendMemoryAddr
void LCD_Init (I2C_Handler_t *ptrHandlerI2C);
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd);
void LCD_sendata (I2C_Handler_t *ptrHandlerI2C, char data);
void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str);
void set_cursor(I2C_Handler_t *ptrHandlerI2C, uint8_t row, uint8_t col);
void lcd_clear(I2C_Handler_t *ptrHandlerI2C);

#endif /* INC_LCD_H_ */
