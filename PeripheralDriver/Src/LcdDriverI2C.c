/*
 * LcdDriverI2C.c
 *
 *  Created on: 2/11/2022
 *      Author: Ejguzmanc
 */

#include <stdint.h>
#include "I2CDriver.h"
#include "LcdDriver.h"
#include "GPIOxDriver.h"

static LCD_t LCD;

//Protocol de escritura en la LCD
void i2c_writeByteLcd(I2C_Handler_t *ptrHandlerI2C,  uint8_t newValue){

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();

	//1. Generamos la condicion de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);



	//4. Enviamos el valor que deseamos escribir en el registro seleccionado
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	//5. Generamos la condicion de stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

	//activamos las interrupciones
	__enable_irq();
}

//Enviando comandos
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd){
	//ej. CMD = 0b00110000
	char up;
	char low;
	uint8_t T[4];
	up=(cmd & 0xf0); //00110000
	//corridos hacia la izquierda y aplicamos mascara para obtener parte baja
	low=((cmd<<4) & 0xf0); //00000000
	T[0] = up|0x0C;  //0x0C = 1100 => (P3) 1 = backlight ; (P2) 1 = enable strobe (ayuda a entender a la lcd
					//enviar un dato o comando); (P1) 0 = Wirte ; (P0)  0 = enviando comando
					//=>ej. 00110000 or 00001100 =00111100
	i2c_writeByteLcd(ptrHandlerI2C, T[0]);
	T[1] = up|0x08; //0x08 = 1000 => (P3) 1 = backlight ; (P2) 0 = enable strobe (ayuda a entender a la lcd
	//enviar un dato o comando); (P1) 0 = Wirte ; (P0)  0 = enviando COMANDO
	//=>ej. 00110000 or 00001100 =00111100

	i2c_writeByteLcd(ptrHandlerI2C, T[1]);
	T[2] = low|0x0C;
	i2c_writeByteLcd(ptrHandlerI2C, T[2]);
	T[3] = low|0x08;
	i2c_writeByteLcd(ptrHandlerI2C, T[3]);

}

//Enviando DATA

void LCD_sendata (I2C_Handler_t *ptrHandlerI2C, char data){

	char up;
	char low;
	uint8_t T[4];
	up=(data & 0xf0);
	low=((data<<4) & 0xf0);
	T[0] = up|0x0D; //0x0D = 1101 => (P3) 1 = backlight ; (P2) 1 = enable strobe (ayuda a entender a la lcd
	//enviar un dato o comando); (P1) 0 = Wirte ; (P0)  1 = enviando DATOS

	i2c_writeByteLcd(ptrHandlerI2C, T[0]);
	T[1] = up|0x09;
	i2c_writeByteLcd(ptrHandlerI2C, T[1]);
	T[2] = low|0x0D;
	i2c_writeByteLcd(ptrHandlerI2C, T[2]);
	T[3] = low|0x09;
	i2c_writeByteLcd(ptrHandlerI2C, T[3]);

}


void LCD_Clear (I2C_Handler_t *ptrHandlerI2C) {

	LCD_sendata (ptrHandlerI2C, 0x00);
	for (int i=0; i<100; i++) {
		LCD_sendata (ptrHandlerI2C,' ');
	}
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

	LCD.CurrentX = 0;
	LCD.CurrentY = 0;
}

void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str) {
	while (*str) LCD_sendata (ptrHandlerI2C, *str++);

}

void LCD_setCursor(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y) {



	uint8_t cursor;
	switch (x) {
	case 0 :                           /* caso para la primera fila del LCD */
		switch (y) {
		/* casos para las casillas de la primera fila */
		case 0 : cursor = 0x00; break;
		case 1 : cursor = 0x01; break;
		case 2 : cursor = 0x02; break;
		case 3 : cursor = 0x03; break;
		case 4 : cursor = 0x04; break;
		case 5 : cursor = 0x05; break;
		case 6 : cursor = 0x06; break;
		case 7 : cursor = 0x07; break;
		case 8 : cursor = 0x08; break;
		case 9 : cursor = 0x09; break;
		case 10 : cursor = 0x0A; break;
		case 11 : cursor = 0x0B; break;
		case 12 : cursor = 0x0C; break;
		case 13 : cursor = 0x0D; break;
		case 14 : cursor = 0x0E; break;
		case 15 : cursor = 0x0F; break;
		case 16 : cursor = 0x10; break;
		case 17 : cursor = 0x11; break;
		case 18 : cursor = 0x12; break;
		case 19 : cursor = 0x13; break;
		}
		break;

		case 1 :                               /* caso para la segunda fila del LCD */
			switch (y) {
			/* casos para las casillas de la segunda fila */
			case 0 : cursor = 0x40; break;
			case 1 : cursor = 0x41; break;
			case 2 : cursor = 0x42; break;
			case 3 : cursor = 0x43; break;
			case 4 : cursor = 0x44; break;
			case 5 : cursor = 0x45; break;
			case 6 : cursor = 0x46; break;
			case 7 : cursor = 0x47; break;
			case 8 : cursor = 0x48; break;
			case 9 : cursor = 0x49; break;
			case 10 : cursor = 0x4A; break;
			case 11 : cursor = 0x4B; break;
			case 12 : cursor = 0x4C; break;
			case 13 : cursor = 0x4D; break;
			case 14 : cursor = 0x4E; break;
			case 15 : cursor = 0x4F; break;
			case 16 : cursor = 0x50; break;
			case 17 : cursor = 0x51; break;
			case 18 : cursor = 0x52; break;
			case 19 : cursor = 0x53; break;
			}
			break;

			case 2 :                           /* caso para la tercera fila del LCD */
				switch (y) {
				/* casos para las casillas de la tercera fila */
				case 0 : cursor = 0x14; break;
				case 1 : cursor = 0x15; break;
				case 2 : cursor = 0x16; break;
				case 3 : cursor = 0x17; break;
				case 4 : cursor = 0x18; break;
				case 5 : cursor = 0x19; break;
				case 6 : cursor = 0x1A; break;
				case 7 : cursor = 0x1B; break;
				case 8 : cursor = 0x1C; break;
				case 9 : cursor = 0x1D; break;
				case 10 : cursor = 0x1E; break;
				case 11 : cursor = 0x1F; break;
				case 12 : cursor = 0x20; break;
				case 13 : cursor = 0x21; break;
				case 14 : cursor = 0x22; break;
				case 15 : cursor = 0x23; break;
				case 16 : cursor = 0x24; break;
				case 17 : cursor = 0x25; break;
				case 18 : cursor = 0x26; break;
				case 19 : cursor = 0x27; break;
				}
				break;

				case 3 :                           /* caso para la cuarta fila del LCD */
					switch (y) {
					/* casos para las casillas de la primera fila */
					case 0 : cursor = 0x54; break;
					case 1 : cursor = 0x55; break;
					case 2 : cursor = 0x56; break;
					case 3 : cursor = 0x57; break;
					case 4 : cursor = 0x58; break;
					case 5 : cursor = 0x59; break;
					case 6 : cursor = 0x5A; break;
					case 7 : cursor = 0x5B; break;
					case 8 : cursor = 0x5C; break;
					case 9 : cursor = 0x5D; break;
					case 10 : cursor = 0x5E; break;
					case 11 : cursor = 0x5F; break;
					case 12 : cursor = 0x60; break;
					case 13 : cursor = 0x61; break;
					case 14 : cursor = 0x62; break;
					case 15 : cursor = 0x63; break;
					case 16 : cursor = 0x64; break;
					case 17 : cursor = 0x65; break;
					case 18 : cursor = 0x66; break;
					case 19 : cursor = 0x67; break;
					}
					break;
	}
	LCD_sendCMD(ptrHandlerI2C, 0x80|cursor);
}

/*
 * Funcion para obtener dinamicamente la posiion del cursor
 */
uint8_t LCD_GetX (void){
	return LCD.CurrentX;
}
uint8_t LCD_GetY (void){
	return LCD.CurrentY;
}

/*
 * Permite mover el cursor a la derecha solo si este no se encuentra en el extremo derecho
 */
void cursorShiftR(I2C_Handler_t *ptrHandlerI2C){

	//Aumenta en 1 la posicion del cursor si no esta en el extremo derecho
	if(LCD.CurrentX != 0x06 && LCD.CurrentX != 0x2E && LCD.CurrentX != 0x1A && LCD.CurrentX != 0x41){
		LCD_sendCMD(ptrHandlerI2C, 0x14);
		LCD.CurrentX+=1;
		delay(5);
	}
}

/*
 * Permite mover el cursor a la izquierda solo si este no se encuentra en el extremo izquierdo
 */
void cursorShiftL(I2C_Handler_t *ptrHandlerI2C){

	//Disminuye en 1 la posicion del cursor si no esta en el extremo izquierdo
	if(LCD.CurrentX != 0x00 && LCD.CurrentX != 0x28 && LCD.CurrentX != 0x14 && LCD.CurrentX != 0x3C){
		LCD_sendCMD(ptrHandlerI2C, 0x10);
		LCD.CurrentX-=1;
		delay(5);
	}
}

/*
 * Permite mover el cursor hacia abajo solo si este no se encuentra en el extremo inferior
 */
void cursorShiftD(I2C_Handler_t *ptrHandlerI2C){
	if(LCD.CurrentX != 60 && LCD.CurrentX != 61 && LCD.CurrentX != 62 && LCD.CurrentX != 63
			&& LCD.CurrentX != 64 && LCD.CurrentX != 65 && LCD.CurrentX != 26){

		if(LCD.CurrentX/10 == 0){
			uint8_t i;
			for(i=0; i<40; i++){
				LCD_sendCMD(ptrHandlerI2C, 0x14);
				delay(1);
			}
			LCD.CurrentX+=40;
			LCD.CurrentY+=1;
		}
		else if(LCD.CurrentX/10 == 4){
			uint8_t i;
			for(i=0; i<20; i++){
				LCD_sendCMD(ptrHandlerI2C, 0x10);
				delay(1);
			}
			LCD.CurrentX-=20;
			LCD.CurrentY+=1;
		}
		else if(LCD.CurrentX/10 == 2){
			uint8_t i;
			for(i=0; i<40; i++){
				LCD_sendCMD(ptrHandlerI2C, 0x14);
				delay(1);
			}
			LCD.CurrentX+=40;
			LCD.CurrentY+=1;
		}
	}
}

/*
 * Permite mover el cursor hacia arriba solo si este no se encuentra en el extremo superior
 */
void cursorShiftU(I2C_Handler_t *ptrHandlerI2C){
	if(LCD.CurrentX != 0 && LCD.CurrentX != 1 && LCD.CurrentX != 2 && LCD.CurrentX != 3
			&& LCD.CurrentX != 4 && LCD.CurrentX != 5 && LCD.CurrentX != 6){

		if(LCD.CurrentX/10 == 6){
			uint8_t i;
			for(i=0; i<40; i++){
				LCD_sendCMD(ptrHandlerI2C, 0x10);
				delay(1);
			}
			LCD.CurrentX-=40;
			LCD.CurrentY-=1;
		}
		else if(LCD.CurrentX/10 == 2){
			uint8_t i;
			for(i=0; i<20; i++){
				LCD_sendCMD(ptrHandlerI2C, 0x14);
				delay(1);
			}
			LCD.CurrentX+=20;
			LCD.CurrentY-=1;
		}
		else if(LCD.CurrentX/10 == 4){
			uint8_t i;
			for(i=0; i<40; i++){
				LCD_sendCMD(ptrHandlerI2C, 0x10);
				delay(1);
			}
			LCD.CurrentX-=40;
			LCD.CurrentY-=1;
		}
	}
}

/*
 * Posiciona el cursor de LCD en la primera fila primera columna
 */
void cursorHome(I2C_Handler_t *ptrHandlerI2C){
	LCD_sendCMD(ptrHandlerI2C,2);
	LCD.CurrentX = 0;
	LCD.CurrentY = 0;
}

/*
 * Posiciona el cursor de LCD en el "caracter" backspace a partir del cursor home
 */
void cursorBS(I2C_Handler_t *ptrHandlerI2C){
	uint8_t i;
	for(i=0;i<6;i++){
		LCD_sendCMD(ptrHandlerI2C, 0x14);
	}
	LCD.CurrentX = 6;
	LCD.CurrentY = 0;
}

/*
 * Posiciona el cursor en el valor 0x21 que corresponde a la opcion "S" en restart
 */
void cursorInRestart(I2C_Handler_t *ptrHandlerI2C){
	LCD_sendCMD(ptrHandlerI2C, 0x80|0x21);
	LCD.CurrentX = 35;
	LCD.CurrentY = 2;
}

/*
 * funcion que solamente puede ejecutar un shift a la derecha
 */
void cursorRestartShiftR(I2C_Handler_t *ptrHandlerI2C){

	//Aumenta en 1 la posicion del cursor si no esta en el extremo derecho
	if(LCD.CurrentX != 37){
		LCD_sendCMD(ptrHandlerI2C, 0x14);
		LCD_sendCMD(ptrHandlerI2C, 0x14);
		LCD.CurrentX+=2;
		delay(5);
	}
}

/*
 * funcion que solamente puede ejecutar un shift a la izquierda
 */
void cursorRestartShiftL(I2C_Handler_t *ptrHandlerI2C){

	//Disminuye en 1 la posicion del cursor si no esta en el extremo izquierdo
	if(LCD.CurrentX != 35){
		LCD_sendCMD(ptrHandlerI2C, 0x10);
		LCD_sendCMD(ptrHandlerI2C, 0x10);
		LCD.CurrentX-=2;
		delay(5);
	}
}
