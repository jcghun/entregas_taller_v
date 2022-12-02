/*
 * SH1106.c
 *
 *  Created on: 11/11/2022
 *      Author: Juan Camilo Gomez Hidalgo
 */

#include "stm32f4xx.h"
#include "SH1106.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "string.h"

extern I2C_Handler_t ptrHandlerI2C;

static SH1106_t SH1106;



//BUFFER CON DATASIZE = 1024 ===> DEFINIENDO EL TAMAÑO DE UNA PAGINA
static uint8_t SH1106_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

void sh1106_Init (I2C_Handler_t *ptrHandlerI2C){

	//delay(1000);

	oled_sendCMD(ptrHandlerI2C, 0xAE);
	oled_sendCMD(ptrHandlerI2C, 0xD5);
	oled_sendCMD(ptrHandlerI2C, 0x80);
	oled_sendCMD(ptrHandlerI2C, 0xA8);
	oled_sendCMD(ptrHandlerI2C, 0x3F);
	oled_sendCMD(ptrHandlerI2C, 0xD3);
	oled_sendCMD(ptrHandlerI2C, 0x00);
	oled_sendCMD(ptrHandlerI2C, 0x40);
	oled_sendCMD(ptrHandlerI2C, 0xAD);
	//oled_sendCMD(ptrHandlerI2C, 0x8A);
	oled_sendCMD(ptrHandlerI2C, 0x8D);
	oled_sendCMD(ptrHandlerI2C, 0xA1);
	oled_sendCMD(ptrHandlerI2C, 0xC8);
	oled_sendCMD(ptrHandlerI2C, 0xDA);
	oled_sendCMD(ptrHandlerI2C, 0x12);
	oled_sendCMD(ptrHandlerI2C, 0x81);
	//oled_sendCMD(ptrHandlerI2C, 0xFF);
	oled_sendCMD(ptrHandlerI2C, 0xCF);
	oled_sendCMD(ptrHandlerI2C, 0xD9);
	//oled_sendCMD(ptrHandlerI2C, 0x1F);
	oled_sendCMD(ptrHandlerI2C, 0x22);
	oled_sendCMD(ptrHandlerI2C, 0xDB);
	oled_sendCMD(ptrHandlerI2C, 0x40);
	oled_sendCMD(ptrHandlerI2C, 0x40);	//REPITE
	oled_sendCMD(ptrHandlerI2C, 0x32);
	//oled_sendCMD(ptrHandlerI2C, 0xA6);
	oled_sendCMD(ptrHandlerI2C, 0xA4);
	oled_sendCMD(ptrHandlerI2C, 0xA6);
	oled_sendCMD(ptrHandlerI2C, 0xAF);

//	oled_sendCMD(ptrHandlerI2C, 0xAE);
//	oled_sendCMD(ptrHandlerI2C, 0x02);
//	oled_sendCMD(ptrHandlerI2C, 0x10);
//	oled_sendCMD(ptrHandlerI2C, 0x40);
//	oled_sendCMD(ptrHandlerI2C, 0xB0);
//	oled_sendCMD(ptrHandlerI2C, 0x81);
//	oled_sendCMD(ptrHandlerI2C, 0xBF);
//	oled_sendCMD(ptrHandlerI2C, 0xA1);
//	oled_sendCMD(ptrHandlerI2C, 0xA6);
//	oled_sendCMD(ptrHandlerI2C, 0xA8 );
//	oled_sendCMD(ptrHandlerI2C, 0x3F);
//	oled_sendCMD(ptrHandlerI2C, 0xAD);
//	oled_sendCMD(ptrHandlerI2C, 0x8A);
//	oled_sendCMD(ptrHandlerI2C, 0x32);
//	oled_sendCMD(ptrHandlerI2C, 0xC8);
//	oled_sendCMD(ptrHandlerI2C, 0xD3);
//	oled_sendCMD(ptrHandlerI2C, 0x00);
//	oled_sendCMD(ptrHandlerI2C, 0xD5);
//	oled_sendCMD(ptrHandlerI2C, 0x80);
//	oled_sendCMD(ptrHandlerI2C, 0xD9);
//	oled_sendCMD(ptrHandlerI2C, 0x22);
//	oled_sendCMD(ptrHandlerI2C, 0xDA);
//	oled_sendCMD(ptrHandlerI2C, 0x12);
//	oled_sendCMD(ptrHandlerI2C, 0xDB);
//	oled_sendCMD(ptrHandlerI2C, 0x40);
//	oled_sendCMD(ptrHandlerI2C, 0xAF);


//	oled_sendCMD(ptrHandlerI2C, 0xAE); // Display off
//	oled_sendCMD(ptrHandlerI2C, 0xA6); // Divide ratio/osc mode SET
//	oled_sendCMD(ptrHandlerI2C, 0xD5);
//	oled_sendCMD(ptrHandlerI2C, 0x80); // SET multiplex radio
//	oled_sendCMD(ptrHandlerI2C, 0xA8);
//	oled_sendCMD(ptrHandlerI2C, 0x3F); //Display offset mode SET
//	oled_sendCMD(ptrHandlerI2C, 0xBF);
//	oled_sendCMD(ptrHandlerI2C, 0xD3);
//	oled_sendCMD(ptrHandlerI2C, 0x00);
//	oled_sendCMD(ptrHandlerI2C, (0x40 | 0x00) ); // Set segment remap
//	oled_sendCMD(ptrHandlerI2C, 0x8E); // Set com output scan direct
//	oled_sendCMD(ptrHandlerI2C, 0x14); // Set com pins hardware config
//	oled_sendCMD(ptrHandlerI2C, 0x20);
//	oled_sendCMD(ptrHandlerI2C, 0x00); //Set contrast color
//	oled_sendCMD(ptrHandlerI2C, 0xA1);
//	oled_sendCMD(ptrHandlerI2C, 0xC8); // Precharge period mode SET
//	oled_sendCMD(ptrHandlerI2C, 0xDA);
//	oled_sendCMD(ptrHandlerI2C, 0x12); //VCOMH deselect level
//	oled_sendCMD(ptrHandlerI2C, 0x81);
//	oled_sendCMD(ptrHandlerI2C, 0xCF); // Set entire display ON
//	oled_sendCMD(ptrHandlerI2C, 0xD9);
//	oled_sendCMD(ptrHandlerI2C, 0xF1);
//	oled_sendCMD(ptrHandlerI2C, 0xDB);
//	oled_sendCMD(ptrHandlerI2C, 0x40);
//	oled_sendCMD(ptrHandlerI2C, 0xA4);
//	oled_sendCMD(ptrHandlerI2C, 0xA6);
//	oled_sendCMD(ptrHandlerI2C, 0x2E);
//	oled_sendCMD(ptrHandlerI2C, 0xAF);

//	SH1106_Fill();

	SH1106_Clear();

	SH1106_UpdateScreen(ptrHandlerI2C);

}



void SH1106_GotoXY(uint16_t x, uint16_t y){
	SH1106.CurrentX = x;
	SH1106.CurrentY = y;
}

uint16_t SH1106_GetX (void){
	return SH1106.CurrentX;
}

uint16_t SH1106_GetY (void){
	return SH1106.CurrentY;
}




void SH1106_DrawPixel(uint16_t x, uint16_t y, uint8_t drawOrClear) {
	if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) {
		/* Error */
		return ;
	}

	if(drawOrClear){
		SH1106_Buffer[x + (y / 8)* SH1106_WIDTH] |= 1 << (y % 8);
	}
	else {
		SH1106_Buffer[x + (y / 8)* SH1106_WIDTH] &= ~(1 << (y % 8));
	}
}

void SH1106_UpdateScreen(I2C_Handler_t *ptrHandlerI2C) {
	uint8_t page;

	for (page = 0; page < 8; page++) {
		oled_sendCMD(ptrHandlerI2C, 0xB0 + page);
		//seteamos lower column que define la columna en la pagina (page)
		oled_sendCMD(ptrHandlerI2C, 0x00);
		//seteamos higher column que define la fila en la columna (page)
		oled_sendCMD(ptrHandlerI2C, 0x10);

		/* Write multi data */
		SH1106_I2C_WriteMulti2(ptrHandlerI2C, 0x40, &SH1106_Buffer[SH1106_WIDTH * page], SH1106_WIDTH);
	}
}

//void SH1106_I2C_WriteMulti(I2C_Handler_t *ptrHandlerI2C, uint8_t reg, char* data, uint16_t count) {
//uint8_t dt[256];
//uint8_t i;
//for(i = 0; i < count; i++){
//	dt[i] = data[i];
//	i2c_WriteMulti(ptrHandlerI2C,reg, data, count);
//	}
//}

void SH1106_I2C_WriteMulti2(I2C_Handler_t *ptrHandlerI2C, uint8_t reg, uint8_t* data, uint16_t count){
	uint8_t i;

	i2c_startTransaction(ptrHandlerI2C);
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);
	sh1106_I2C_WriteData(ptrHandlerI2C, reg);
	for(i=0; i<count; i++){
		sh1106_I2C_WriteData(ptrHandlerI2C, data[i]);
	}

	/*	4.1 Esperamos hasta que el byte sea transmitido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
		__NOP();
	}
	//5. Generamos la condicion de stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);
}

//####################
void sh1106_I2C_WriteData(I2C_Handler_t *ptrHandlerI2C, uint8_t data) {

	while (!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)) {
		__NOP();
	}

	/* Send I2C data */
	ptrHandlerI2C->ptrI2Cx->DR = data;
}


//#########################3
void SH1106_Fill() {
	/* Set memory */
	memset(SH1106_Buffer, 0xFF , sizeof(SH1106_Buffer));
}

void SH1106_Clear() {
	/* Set memory */
	memset(SH1106_Buffer, 0x00 , sizeof(SH1106_Buffer));
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













char SH1106_Putc(char ch, FontDef_t* Font) {
	uint32_t i, b, j;

	/* Check available space in LCD */
	if (
		SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
		SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}

	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) { //Iterar (Altura de la fuente) veces
		b = Font->data[(ch - 32) * Font->FontHeight + i]; //Obtener la posicion en el arreglo data
		for (j = 0; j < Font->FontWidth; j++) { //Iterar (Ancho de la fuente) veces
			if ((b << j) & 0x8000) {
				SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), 1);
			} else {
				SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), 0);
			}
			//SH1106_UpdateScreen(ptrHandlerI2C);
		}
	}

	/* Increase pointer */
	SH1106.CurrentX += Font->FontWidth;

	/* Return character written */
	return ch;
}


char SH1106_PutNegch(char ch, FontDef_t* Font) {
	uint32_t i, b, j;

	/* Check available space in LCD */
	if (
		SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
		SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}

	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) { //Iterar (Altura de la fuente) veces
		b = Font->data[(ch - 32) * Font->FontHeight + i]; //Obtener la posicion en el arreglo data
		for (j = 0; j < Font->FontWidth; j++) { //Iterar (Ancho de la fuente) veces
			if ((b << j) & 0x8000) {
				SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), 0);
			} else {
				SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), 1);
			}
			//SH1106_UpdateScreen(ptrHandlerI2C);
		}
	}

	/* Increase pointer */
	SH1106.CurrentX += Font->FontWidth;

	/* Return character written */
	return ch;
}


char SH1106_Puts(char* str, FontDef_t* Font) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (SH1106_Putc(*str, Font) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
}
