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

typedef struct {
    uint8_t CurrentX;
    uint8_t CurrentY;
} LCD_t;


/* Prototipos de las funciones */



void i2c_writeByteLcd(I2C_Handler_t *ptrHandlerI2C,  uint8_t newValue); //protocolo sin sendMemoryAddr
void LCD_Init (I2C_Handler_t *ptrHandlerI2C);
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd);
void LCD_sendata (I2C_Handler_t *ptrHandlerI2C, char data);
void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str);
void set_cursor(I2C_Handler_t *ptrHandlerI2C, uint8_t row, uint8_t col);
void LCD_setCursor(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);

void LCD_Clear (I2C_Handler_t *ptrHandlerI2C);

uint8_t LCD_GetX (void);
uint8_t LCD_GetY (void);

void cursorShiftL(I2C_Handler_t *ptrHandlerI2C);
void cursorShiftR(I2C_Handler_t *ptrHandlerI2C);
void cursorShiftD(I2C_Handler_t *ptrHandlerI2C);
void cursorShiftU(I2C_Handler_t *ptrHandlerI2C);
void cursorHome(I2C_Handler_t *ptrHandlerI2C);
void cursorBS(I2C_Handler_t *ptrHandlerI2C);

void cursorRestartShiftR(I2C_Handler_t *ptrHandlerI2C);
void cursorRestartShiftL(I2C_Handler_t *ptrHandlerI2C);
void cursorInRestart(I2C_Handler_t *ptrHandlerI2C);
#endif /* INC_LCD_H_ */
