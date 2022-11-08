/*
 * LcdDriverI2C.c
 *
 *  Created on: 2/11/2022
 *      Author: Juan Camilo Gomez Hidalgo
 */

#include <stdint.h>
#include "I2CDriver.h"
#include "LcdDriver.h"
#include "GPIOxDriver.h"

//Protocol de escritura en la LCD
void i2c_writeByteLcd(I2C_Handler_t *ptrHandlerI2C,  uint8_t newValue){
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

//Enviando comandos
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd){
	//ej. CMD = 0b00110000
	char _U;
	char _L;
	uint8_t _T[4];
	_U=(cmd & 0xf0); //00110000
	//corridos hacia la izquierda y aplicamos mascara para obtener parte baja
	_L=((cmd<<4) & 0xf0); //00000000
	_T[0] = _U|0x0C;  //0x0C = 1100 => (P3) 1 = backlight ; (P2) 1 = enable strobe (ayuda a entender a la lcd
					//enviar un dato o comando); (P1) 0 = Wirte ; (P0)  0 = enviando comado
					//=>ej. 00110000 or 00001100 =00111100
	i2c_writeByteLcd(ptrHandlerI2C, _T[0]);
	_T[1] = _U|0x08; //0x08 = 1000 => (P3) 1 = backlight ; (P2) 0 = enable strobe (ayuda a entender a la lcd
	//enviar un dato o comando); (P1) 0 = Wirte ; (P0)  0 = enviando COMANDO
	//=>ej. 00110000 or 00001100 =00111100

	i2c_writeByteLcd(ptrHandlerI2C, _T[1]);
	_T[2] = _L|0x0C;
	i2c_writeByteLcd(ptrHandlerI2C, _T[2]);
	_T[3] = _L|0x08;
	i2c_writeByteLcd(ptrHandlerI2C, _T[3]);

}

//Enviando DATA

void LCD_sendata (I2C_Handler_t *ptrHandlerI2C, char data){

	char _U;
	char _L;
	uint8_t _T[4];
	_U=(data & 0xf0);
	_L=((data<<4) & 0xf0);
	_T[0] = _U|0x0D; //0x0D = 1101 => (P3) 1 = backlight ; (P2) 1 = enable strobe (ayuda a entender a la lcd
	//enviar un dato o comando); (P1) 0 = Wirte ; (P0)  1 = enviando DATOS

	i2c_writeByteLcd(ptrHandlerI2C, _T[0]);
	_T[1] = _U|0x09;
	i2c_writeByteLcd(ptrHandlerI2C, _T[1]);
	_T[2] = _L|0x0D;
	i2c_writeByteLcd(ptrHandlerI2C, _T[2]);
	_T[3] = _L|0x09;
	i2c_writeByteLcd(ptrHandlerI2C, _T[3]);

}

//void LCD_Clear (I2C_Handler_t *ptrHandlerI2C, uint8_t adress) {
//
//	LCD_sendata (ptrHandlerI2C, 0x00);
//	for (int i=0; i<100; i++) {
//		LCD_sendata (ptrHandlerI2C,' ');
//	}
//}

void lcd_clear(I2C_Handler_t *ptrHandlerI2C){

	//Enviamos comando para limpiar display
	LCD_sendCMD(ptrHandlerI2C,0x01);
	// según el datasheet se espera en el proceso minimo 1.5 ms
	delay(5);
}

void LCD_Init (I2C_Handler_t *ptrHandlerI2C) {

	//seún datasheet esperar más de 15 ms
	delay(45);
	LCD_sendCMD (ptrHandlerI2C, 0x30);
	//más de 4.1 ms
	delay(5);
	LCD_sendCMD (ptrHandlerI2C, 0x30);
	delay(1);
	LCD_sendCMD (ptrHandlerI2C, 0x30);
	delay(10);
	// comando 0x20 modo 4 bits
	LCD_sendCMD (ptrHandlerI2C, 0x20);
	delay(10);
	//comando 0x28 funtion set DL = 0 (4 bits), linea del display y display font size
	LCD_sendCMD (ptrHandlerI2C, 0x28);
	delay(1);
	//Apagamos el display (Display ON/OFF control ) D=0; C=0; B=0 (todos se apagan) B(blinky del cursor)
	LCD_sendCMD (ptrHandlerI2C, 0x08);
	delay(1);
	//Limpiar el Display
	LCD_sendCMD (ptrHandlerI2C, 0x01);
	delay(2);
	// Entry mode Set I/D = 1; S = 0; S(Blinky del display cuadro)
	LCD_sendCMD (ptrHandlerI2C, 0x06);
	delay(1);
	//Prendemos el display (Display ON/OFF control ) D=1; C=0; B=0.  B(blinky del cursor)
	LCD_sendCMD (ptrHandlerI2C, 0x0C);
}

void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str) {
	while (*str) LCD_sendata (ptrHandlerI2C, *str++);

}

void set_cursor(I2C_Handler_t *ptrHandlerI2C, uint8_t row, uint8_t col){

	switch(row){

	case 0:
		col |= 0x80;
		break;
	case 1:
		col |= 0xC0;
		break;
	case 2:
		col |= 0x94;
		break;
	case 3:
		col |= 0xD4;
		break;
	default:
		col |= 0x80;
		break;

	}
	LCD_sendCMD(ptrHandlerI2C, col);
}





