/*
 * SH1106.c
 *
 *  Created on: 11/11/2022
 *      Author: Juan Camilo Gomez
 */

#include "stm32f4xx.h"
#include "SH1106.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"

extern I2C_Handler_t ptrHandlerI2C;

//#define SH1106_WRITECOMMAND(command)      sh1106_I2C_Write(ptrHandlerI2C, SH1106_I2C_ADDR, 0x00, (command))
#define SH1106_WRITEDATA(data)            sh1106_I2C_Write(SSD1306_I2C_ADDR, 0x40, (data))

SH1106_t SH1106 = {0};

void sh1106_Init (I2C_Handler_t *ptrHandlerI2C){

	delay(250);

	oled_sendCMD(ptrHandlerI2C, 0xAE); // Display off
	oled_sendCMD(ptrHandlerI2C, 0xD5); // Divide ratio/osc mode SET
	oled_sendCMD(ptrHandlerI2C, 0x80);
	oled_sendCMD(ptrHandlerI2C, 0xA8); // SET multiplex radio
	oled_sendCMD(ptrHandlerI2C, 0x1F);
	oled_sendCMD(ptrHandlerI2C, 0xD3); //Display offset mode SET
	oled_sendCMD(ptrHandlerI2C, 0x00);
	oled_sendCMD(ptrHandlerI2C, 0x8D);
	oled_sendCMD(ptrHandlerI2C, 0x10);
	oled_sendCMD(ptrHandlerI2C, 0xA1); // Set segment remap
	oled_sendCMD(ptrHandlerI2C, 0xC8); // Set com output scan direct
	oled_sendCMD(ptrHandlerI2C, 0xDA); // Set com pins hardware config
	oled_sendCMD(ptrHandlerI2C, 0x00);
	oled_sendCMD(ptrHandlerI2C, 0x81); //Set contrast color
	oled_sendCMD(ptrHandlerI2C, 0x4F);
	oled_sendCMD(ptrHandlerI2C, 0xD9); // Precharge period mode SET
	oled_sendCMD(ptrHandlerI2C, 0x1F);
	oled_sendCMD(ptrHandlerI2C, 0xDB); //VCOMH deselect level
	oled_sendCMD(ptrHandlerI2C, 0x40);
	oled_sendCMD(ptrHandlerI2C, 0xA4); // Set entire display ON

	SH1106.CurrentX = 0;
	SH1106.CurrentY = 0;
	SH1106.Initialized = 1;
}




void sh1106_I2C_Write(I2C_Handler_t *ptrHandlerI2C, uint8_t reg, uint8_t data) {
	i2c_writeSingleRegister(ptrHandlerI2C, reg, data);
}

void oled_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd){
	sh1106_I2C_Write(ptrHandlerI2C, 0x00, cmd);
}

void oled_sendata (I2C_Handler_t *ptrHandlerI2C, char data){
	sh1106_I2C_Write(ptrHandlerI2C, 0x40, data);
}
