/*
 * SH1106.h
 *
 *  Created on: 11/11/2022
 *      Author: Juan Camilo Gomez Hidalgo
 */

#ifndef SH1106_H_
#define SH1106_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"
#include "Fonts.h"

#define SH1106_I2C_ADDR			0x78
#define SH1106_HEIGHT			(uint8_t) 64
#define SH1106_WIDTH			(uint8_t) 132

//Estructura para transformaciones(?)
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SH1106_t;

typedef enum {
	SH1106_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SH1106_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SH1106_COLOR_t;

void SH1106_Fill();
void SH1106_GotoXY(uint16_t x, uint16_t y);
uint16_t SH1106_GetX (void);
uint16_t SH1106_GetY (void);
void i2c_writeByteOled(I2C_Handler_t *ptrHandlerI2C,  uint8_t newValue); //protocolo sin sendMemoryAddr
void sh1106_I2C_Write(I2C_Handler_t *ptrHandlerI2C, uint8_t reg, uint8_t data);
void sh1106_Init (I2C_Handler_t *ptrHandlerI2C);
void oled_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd);
void oled_sendata (I2C_Handler_t *ptrHandlerI2C, char data);
void oled_clear(I2C_Handler_t *ptrHandlerI2C);
void SH1106_DrawPixel(uint16_t x, uint16_t y, uint8_t drawOrClear);
void SH1106_UpdateScreen(I2C_Handler_t *ptrHandlerI2C);
void SH1106_I2C_WriteMulti(I2C_Handler_t *ptrHandlerI2C, uint8_t reg, char* data, uint16_t count);
void SH1106_I2C_WriteMulti2(I2C_Handler_t *ptrHandlerI2C, uint8_t reg, uint8_t* data, uint16_t count);

void SH1106_Clear();

void sh1106_I2C_WriteData(I2C_Handler_t *ptrHandlerI2C, uint8_t data);


char SH1106_Putc(char ch, FontDef_t* Font);
char SH1106_PutNegch(char ch, FontDef_t* Font);
char SH1106_Puts(char* str, FontDef_t* Font);

#endif /* SH1106_H_ */
