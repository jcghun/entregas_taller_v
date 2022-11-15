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

#define SH1106_WRITECOMMAND(command)      sh1106_I2C_Write(ptrHandlerI2C, SH1106_I2C_ADDR, 0x00, (command))
#define SH1106_WRITEDATA(data)            sh1106_I2C_Write(SSD1306_I2C_ADDR, 0x40, (data))

SH1106_t SH1106 = {0};

//Protocol de escritura en oled
void i2c_writeByteOled(I2C_Handler_t *ptrHandlerI2C,  uint8_t newValue){
	//ptrHandlerI2C->slaveAddress = 0x4E;

	//1. Generamos la condicion de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//4. Enviamos el valor que deseamos escribir en el registro seleccionado
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	//5. Generamos la condicion de stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);
	//ptrHandlerI2C->slaveAddress = 0x27;
}

void sh1106_Init (I2C_Handler_t *ptrHandlerI2C){

	delay(250);

	SH1106_WRITECOMMAND(0xAE); // Display off
	SH1106_WRITECOMMAND(0xD5); // Divide ratio/osc mode SET
	SH1106_WRITECOMMAND(0x80);
	SH1106_WRITECOMMAND(0xA8); // SET multiplex radio
	SH1106_WRITECOMMAND(0x1F);
	SH1106_WRITECOMMAND(0xD3); //Display offset mode SET
	SH1106_WRITECOMMAND(0x00);
	SH1106_WRITECOMMAND(0x00); // Set display start line
	SH1106_WRITECOMMAND(0x8D);
	SH1106_WRITECOMMAND(0x10);
	SH1106_WRITECOMMAND(0xA1); // Set segment remap
	SH1106_WRITECOMMAND(0xC8); // Set com output scan direct
	SH1106_WRITECOMMAND(0xDA); // Set com pins hardware config
	SH1106_WRITECOMMAND(0x00);
	SH1106_WRITECOMMAND(0x81); //Set contrast color
	SH1106_WRITECOMMAND(0x4F);
	SH1106_WRITECOMMAND(0xD9); // Precharge period mode SET
	SH1106_WRITECOMMAND(0x1F);
	SH1106_WRITECOMMAND(0xDB); //VCOMH deselect level
	SH1106_WRITECOMMAND(0x40);
	SH1106_WRITECOMMAND(0xA4); // Set entire display ON

	SH1106.CurrentX = 0;
	SH1106.CurrentY = 0;
	SH1106.Initialized = 1;
}




void sh1106_I2C_Write(I2C_Handler_t *ptrHandlerI2C, uint8_t address, uint8_t reg, uint8_t data) {
	uint8_t dt[2];
	dt[0] = reg;
	i2c_writeByteOled(ptrHandlerI2C, dt[0]);
	dt[1] = data;
	i2c_writeByteOled(ptrHandlerI2C, dt[1]);
}
