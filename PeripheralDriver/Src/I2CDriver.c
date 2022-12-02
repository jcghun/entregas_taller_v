/*
 * I2CDriver.c
 *
 *  Created on: 20/10/2022
 *      Author: ejguzmanc
 */

#include <stdint.h>
#include "I2CDriver.h"
/* Recordar que se debe configurar los pines para el I2C (SDA y SCL),
 * para lo cual se necesita el modulo GPIO y los pines configurados
 * en el modo Alternate Function. * Ademas, estos pines deben ser configurados como salidas open-drain
 * y con la resistencias en modo pull-up. */

void i2c_config(I2C_Handler_t *ptrHandlerI2C){

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();
/* 1 Activamos la sepal de reloj para el modulo I2C seleccionado */
	if(ptrHandlerI2C->ptrI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	/*
	 * 2. Reinicimaos el periferico, de forma que inicia en estado conocido
	 */

	ptrHandlerI2C ->ptrI2Cx ->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C ->ptrI2Cx ->CR1 &= ~ I2C_CR1_SWRST;

	/*
	 * 3. Indicamos cual es la velocidad del reloj principal, que es la señal utilizda por el periferico
	 * para generar la señal de reloj para el bus I2C
	 */

	ptrHandlerI2C ->ptrI2Cx ->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos); //borramos la configuración previa
	ptrHandlerI2C ->ptrI2Cx ->CR2 |= (MAIN_CLOCK_16MHz_FOR_I2C << I2C_CR2_FREQ_Pos);

	/*
	 * 4. Configuramos el modo I2C en el que el sistema funciona
	 * En esta configuración se incluye también la velocidad del reloj
	 * Todo comienza con los dos registros en 0
	 */

	ptrHandlerI2C ->ptrI2Cx ->CCR = 0;
	ptrHandlerI2C ->ptrI2Cx ->TRISE = 0;

	if (ptrHandlerI2C ->modeI2C == I2C_MODE_SM){

		//Estamos en modo "standart" (SM Mode)
		//Seleccionamos el modo estandar
		ptrHandlerI2C ->ptrI2Cx ->CCR &= ~ I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la señal de reloj
		ptrHandlerI2C ->ptrI2Cx ->CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-Rise máximo
		ptrHandlerI2C ->ptrI2Cx ->TRISE |= I2C_MAX_RISE_TIME_SM;
	}
	else {
		//Estamos en modo "fast" (FM mode)
		//seleccionamos el modo fast
		ptrHandlerI2C->ptrI2Cx->CCR |= I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la señal de reloj
		ptrHandlerI2C ->ptrI2Cx ->CCR |= (I2C_MODE_FM_SPEED_400KHz << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-rise máximo
		ptrHandlerI2C ->ptrI2Cx ->TRISE |= I2C_MAX_RISE_TIME_FM;
	}

	/*
	 * 5. Activamos el modulo I2C
	 */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_PE;

	/* 6. Volvemos a activar las interrupciones del sistema */
	__enable_irq();
}

/*
 * 6. Generamos la condición de stop
 */
void i2c_stopTransaction (I2C_Handler_t *ptrHandlerI2C){
	/*
	 * 7. Generamos la condición de stop
	 */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;


}
/* 1. Verificamos que la línea no esta ocupada - bit "busy" en I2C_CR2 */
/* 2. Generamos la señal "start" */
/* 2a. Esperamos a que la bandera del evento "start" se levante */
/*Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1*/

void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C) {
	/* 1, Verificamos que la línea no esta ocupada - bit "busy" en I2C CR2 */
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}
	/* 2. Generamos la senal "start" */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;
	/* 2a. Esperamos a que la bandera del evento "start" se levante */
	/*Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1*/
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
	__NOP();
	}
}

void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* 2. Generamos la senal "start" */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;
	/* 2a. Esperamos a que la bandera del evento "start" se levante */
	/*Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1*/
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
	__NOP();
	}
}

/*Activamos la indicación para no-ACK (indicación para el slave terminar) */
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C){
	//Debemos escribir cero en la posicion ACK del registro de control 1
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

/*Activamos la indicación para ACK (indicación para el slave terminar) */
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C){
	//Debemos escribir uno en la posicion ACK del registro de control 1
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_ACK;
}

void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t PslaveAddress, uint8_t readOrWrite){
	/* 0. Definimos una variable auxiliar*/
	uint8_t auxByte = 0;
	(void) auxByte;

	/*
	 * 3.  Enviamos la dirección del slave y el bit que indica que deseamos escribir (0)
	 * en el siguiente paso se envia la direccion de memoria que se desea escribir
	 */
	ptrHandlerI2C ->ptrI2Cx ->DR = (ptrHandlerI2C->slaveAddress << 1) | readOrWrite;

	//ptrHandlerI2C ->ptrI2Cx ->DR = (slaveAddress);

	/*
	 * 3.1 Esperamos hasta la bandera del eveto "addr" se levante
	 * (esto nos indica que la dirección fue enviada saisfactoriamente)
	 */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/*
	 * 3.2 Debemos limpiar la bandera de la recepcion de ACK de la "addr", para lo cual debemos
	 * leer en secuencia primero el I2C_SR1 y luego I2C_SR2
	 */
	auxByte = ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte = ptrHandlerI2C->ptrI2Cx->SR2;
}

void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	/*	4. Enviamos la drección de memoria que deseamos leer */
	ptrHandlerI2C->ptrI2Cx->DR = memAddr;

	/*	4.1 Esperamos hasta que el byte sea transmitido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}
}

void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	/*	5. Cargamos el valor que desemos escribir */
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;

//	//agregado
//	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
//		__NOP();
//	}
	/*	4.1 Esperamos hasta que el byte sea transmitido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
		__NOP();
	}
}

uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C){
	//9. esperamos hasta que el byte entrante sea recibido
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE)){
		__NOP();
	}
	ptrHandlerI2C->dataI2C = ptrHandlerI2C->ptrI2Cx->DR;
	return ptrHandlerI2C->dataI2C;
}

uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead){

	//0. Creamos una variable auxiliar para recibir el dato que leemos
	uint8_t auxRead = 0;

	//1. Generamos la condicion de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//3. Enviamos la dirección de memoria que deseamos leer
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	//4. Creamos la condicion de reStart
	i2c_reStartTransaction(ptrHandlerI2C);

	//5. Enviamos la dirección del esclavo y la indicación de LEER
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	//6. Generamos la condicion de no-ACK, para el master no responda y el slave solo envie 1 byte
	i2c_sendNoAck(ptrHandlerI2C);

	//7. Generamos la condicion de stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

	//8. Leemos el dato que envia el esclavo
	auxRead = i2c_readDataByte(ptrHandlerI2C);

	return auxRead;

}

void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead,  uint8_t newValue){
	//1. Generamos la condicion de start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//3. Enviamos la dirección de memoria que deseamos leer
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	//4. Enviamos el valor que deseamos escribir en el registro seleccionado
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	//5. Generamos la condicion de stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

}

void i2c_WriteMulti(I2C_Handler_t *ptrHandlerI2C, uint8_t reg, char *buffer, uint8_t length){



	i2c_startTransaction(ptrHandlerI2C);

	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//MANDAR COMANDOS O DATOS
	i2c_sendMemoryAddress(ptrHandlerI2C, reg);

	//sending the data
	for (uint8_t i=0;i<length;i++)
	 {
	 I2C1->DR=buffer[i]; 						//filling buffer with command or data
		while (!(I2C1->SR1 & I2C_SR1_BTF));
	 }


	//5. Generamos la condicion de stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);


}





