/**
 ******************************************************************************
 * @file           : MainIntroI2C.c
 * @author         : Juan Camilo Gomez
 * @brief          : Ejemplo de uso del I2C
 ******************************************************************************
 */

#include <stm32f4xx.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
#include "I2CDriver.h"

/* Definición de variables */
GPIO_Handler_t handlerLedOK = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};

BasicTimer_Handler_t handlerStateOKTimer = {0};


USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;
char bufferData[64] = "esto es una pequeña prueba";

/* Configuración para el I2C */
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

#define ACCEL_ADDRESS	0b1101001; // 0xD2 -> Dirección del Accel con Logic_1
#define ACCEL_XOUT_H	59	// 0x3B
#define ACCEL_XOUT_L	60	// 0x3C
#define ACCEL_YOUT_H	61	// 0x3D
#define ACCEL_YOUT_L	62	// 0x3E
#define ACCEL_ZOUT_H	63	// 0x3F
#define ACCEL_ZOUT_L	64	// 0x40

#define PWR_MGMT_1		107
#define WHO_AM_I		175


/* Definición de prototipos de funciones */
void initSystem(void);

/* Función principal del programa */

int main(void)
{
    //LLamamos a la función que nos inicializa el hardware del sistema
	initSystem();

	/* Main Loop */
	while(1){

		//Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			writeChar(&handlerCommTerminal, rxData);

			if(rxData == 'd'){
				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'p'){
				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
				rxData = '\0';
			}

			else if(rxData == 'r'){
				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
				rxData = '\0';
			}

			else if(rxData == 'x'){
				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if(rxData == 'y'){
				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if(rxData == 'z'){
				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
		}
	}
}



void initSystem(void){
	//Organizamos la configuración deseada del pin que controla el LED de la board
	handlerLedOK.pGPIOx								= GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	//Llamamos a la función config, para que se encarge de cargar la configuracion

	GPIO_Config(&handlerLedOK);
	GPIO_WritePin(&handlerLedOK, SET);

	/*Configuramos los pines sobre los que funciona el USART */
	handlerPinTX.pGPIOx								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;

	GPIO_Config(&handlerPinTX);

	/* COnfiguramos los pines sobre los que funciona el USART */
	handlerPinRX.pGPIOx								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;

	GPIO_Config(&handlerPinRX);

	//Configuramos la comunicación serial
	handlerCommTerminal.ptrUSARTx						= USART2;
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity		= USART_PARITY_ODD;
	handlerCommTerminal.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_baudrate		= USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

	USART_Config(&handlerCommTerminal);

	//Configuramos el timer
	handlerStateOKTimer.ptrTIMx					= TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed	= BTIMER_SPEED_100us;
	handlerStateOKTimer.TIMx_Config.TIMx_period	= 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable	= 1;

	BasicTimer_Config(&handlerStateOKTimer);
	startTimer(&handlerStateOKTimer);

	//Configuramos los pines sobre los que funciona el I2C
	handlerI2cSCL.pGPIOx								= GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2cSCL);

	handlerI2cSDA.pGPIOx								= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx			= I2C1;
	handlerAccelerometer.modeI2C			= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress		= ACCEL_ADDRESS;

	i2c_config(&handlerAccelerometer);
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLedOK);
}
