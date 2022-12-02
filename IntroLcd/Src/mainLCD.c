/**
 ******************************************************************************
 * @file           : main.c
 * @author         : ejguzmanc
 * @brief          : Configuracion basica del proyecto
 ******************************************************************************
 *
 *
 ******************************************************************************
 */

#include <BasicTimer.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "LcdDriver.h"
#include "USARTxDriver.h"


BasicTimer_Handler_t handlerStateTimer = {0};
GPIO_Handler_t  	handlerStateLed = {0};

GPIO_Handler_t  	handlerI2Cclk = {0};
GPIO_Handler_t  	handlerI2Cdata = {0};

I2C_Handler_t		handlerI2Clcd	={0};

GPIO_Handler_t handlerPinTx		= {0};
GPIO_Handler_t handlerPinRx		= {0};
USART_Handler_t handlerUsart2 = {0};



uint8_t rxData = 0;

char chars[] = {'A', 'B', 'C', 'D', 'E', 'F', '<', 'G', 'H', 'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

/*CABECERAS DE FUNCIONES CREADAS*/

void InitSystem(void);

int main(void)
{
	InitSystem();



	uint8_t i;
	uint8_t j;
	uint8_t c;
writeChar(&handlerUsart2, LCD_GetX());
	for(i=0; i<4; i++){

		for(j=0; j<7; j++){
			if(i==3 && j==6){break;}
			LCD_setCursor(&handlerI2Clcd, i, j);
			LCD_sendata(&handlerI2Clcd, chars[c]);
			delay(1);
			c++;
		}
	}
writeChar(&handlerUsart2, LCD_GetX());

	LCD_sendCMD(&handlerI2Clcd, 0x02);
	delay(5);
	LCD_sendCMD(&handlerI2Clcd, 0x0F);

writeChar(&handlerUsart2, LCD_GetX());
//	set_cursor(&handlerI2Clcd,1,1);
//	LCD_sendata(&handlerI2Clcd, 'A');
//
//	set_cursor(&handlerI2Clcd,2,0);
//	LCD_sendSTR(&handlerI2Clcd, "Tres");
//
//	set_cursor(&handlerI2Clcd,3,0);
//	LCD_sendSTR(&handlerI2Clcd, "cuatro");




    /* Loop forever */
	while(1){
		if (rxData != '\0') {
//			writeChar(&handlerUsart2, rxData);
			if(rxData == 0x1D){
				cursorShiftR(&handlerI2Clcd);
				writeChar(&handlerUsart2, LCD_GetX());
			}

			if(rxData == 0x20){
				LCD_sendCMD(&handlerI2Clcd, 0x14);
				writeChar(&handlerUsart2, LCD_GetX());

				LCD_sendCMD(&handlerI2Clcd, 0x0E);
				LCD_setCursor(&handlerI2Clcd, 0, 12);
				LCD_sendSTR(&handlerI2Clcd, "GANASTE");
			}
		}


		rxData = '\0';
}
return 0;
}

void InitSystem(void){

	//Configuring State LED
	handlerStateLed.pGPIOx 									= GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerStateLed);
	GPIO_WritePin(&handlerStateLed, SET);


	//Configuring Timer2 to work with the State LED
	handlerStateTimer.ptrTIMx 								= TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period 				= 250;
	handlerStateTimer.TIMx_Config.TIMx_interruptEnable 		= 1;

	BasicTimer_Config(&handlerStateTimer);
	startTimer(&handlerStateTimer);

	//Señal clock PB6  - I2C1
	handlerI2Cclk.pGPIOx								= GPIOB;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cclk);

	//Señal data PB7  - I2C1
	handlerI2Cdata.pGPIOx								= GPIOB;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cdata);

	//handler I2C
	handlerI2Clcd.ptrI2Cx							= I2C1;
	handlerI2Clcd.slaveAddress						= 0x27; 		//Cargando dirección del esclavo
	handlerI2Clcd.modeI2C							= I2C_MODE_SM;

	/* Configurando los pines sobre los que funciona el USART2 (TX) */
	handlerPinTx.pGPIOx 							= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinTx);

	/* Configurando los pines sobre los que funciona el USART2 (RX) */
	handlerPinRx.pGPIOx 							= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinRx);

	// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart2.ptrUSARTx 						= USART2;
	handlerUsart2.USART_Config.USART_baudrate		= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

	// Cargamos la configuración del USART
	USART_Config(&handlerUsart2);

	i2c_config(&handlerI2Clcd);

	LCD_Init(&handlerI2Clcd);

//	lcd_clear(&handlerI2Clcd);
}


void BasicTimer2_Callback (void){
	GPIOxTooglePin(&handlerStateLed);
}

void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}
