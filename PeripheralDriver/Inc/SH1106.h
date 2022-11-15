/*
 * SH1106.h
 *
 *  Created on: 11/11/2022
 *      Author: camil
 */

#ifndef SH1106_H_
#define SH1106_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"

#define SH1106_I2C_ADDR			0x78
#define SH1106_HEIGHT			64
#define SH1106_WIDTH			128

//Estructura para transformaciones(?)
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SH1106_t;


void i2c_writeByteOled(I2C_Handler_t *ptrHandlerI2C,  uint8_t newValue); //protocolo sin sendMemoryAddr
void sh1106_I2C_Write(I2C_Handler_t *ptrHandlerI2C, uint8_t address, uint8_t reg, uint8_t data);
void sh1106_Init (I2C_Handler_t *ptrHandlerI2C);
void oled_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd);
void oled_sendata (I2C_Handler_t *ptrHandlerI2C, char data);
void oled_clear(I2C_Handler_t *ptrHandlerI2C);

#endif /* SH1106_H_ */
