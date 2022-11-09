/**
 ******************************************************************************
 * @file           : solucionExamenMain.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Main program body
 ******************************************************************************

 */


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "USARTxDriver.h"
#include "LcdDriver.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "BasicTimer.h"

uint8_t counterReception = 0;
uint8_t rxData = 0;
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
char bufferReception[64] = {0};
char bufferData[64];
char userMsg[] = {0};
char cmd[] = {0};
_Bool stringComplete = false;

unsigned int numeroGrande = 192873;

GPIO_Handler_t  	handlerStateLed = {0};
GPIO_Handler_t      handlerI2Cclk = {0};
GPIO_Handler_t		handlerI2Cdata = {0};
GPIO_Handler_t 		handlerPinTx		= {0};
GPIO_Handler_t 		handlerPinRx		= {0};

BasicTimer_Handler_t handlerStateTimer = {0};

USART_Handler_t handlerUsart1 = {0};

I2C_Handler_t		handlerI2Clcd	={0};

void parseCommands(char *ptrBufferReception);
void initSystem(void);

int main(void)
{
	initSystem();

	while(1){


			//Creamos una cadena de caracteres con los datos que llegan por el serial
			//El caracter '@' nos indica  que es el final de la cadena
			if (rxData != '\0'){
				bufferReception[counterReception] = rxData;
				counterReception++;

				//Si el caracter que llega es @, alza bandera
				//Para que en el main loop se haga algo
				if(rxData == '@'){
					stringComplete = true;

					//Agrego esta linea para crear el string con el null al final
					bufferReception[counterReception] = '\0';

					counterReception = 0;


				}

				//Para que no vuelva a entrar. Solo cambia debido a la interrupcion
				rxData = '\0';
			}

			//Hacemos un analisis de la cadena de datos obtenida
			if (stringComplete){
				parseCommands(bufferReception);
				stringComplete = false;
			}

	}
	return 0;
}

void parseCommands(char *ptrBufferReception){

	//Esta funcion de C lee la cadena de caracteres a la que apunta el ptr y la divide
	//y almacena en tres elementos diferenctes: un str llamado "cmd" y dos numeros
	//integer llamados "firstParameter" y "SecondParameter".
	//de esta forma, podemos introducir informacion al micro desde el puerto serial
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	//Este comando imprime una lista con los otros comandos que tiene el equipo
	if(strcmp(cmd, "help") == 0){
		writeMsg(&handlerUsart1, "Help Menu CMD:\n");
		writeMsg(&handlerUsart1, "1) help				-- Print this menu\n");
		writeMsg(&handlerUsart1, "2) dummy #A #B		-- dummy cmd, #A and #B are uint32_t\n");
		writeMsg(&handlerUsart1, "3) usermsg # # msg 	-- msg is a string comming from outside\n");
		writeMsg(&handlerUsart1, "4) initLcd			-- steps to initialize the LCD\n");
		writeMsg(&handlerUsart1, "5) testLcd  		-- simple Test for the LCD\n");
		writeMsg(&handlerUsart1, "6) setPeriod #      -- change the StateLed period\n");
		writeMsg(&handlerUsart1, "7) resetCursor      -- Set the lcd cursor to (0,0");
	}

	//El comandfo dummy sirve para entender como funciona la recepcion de numeros enviados
	//desde la consola
	else if (strcmp(cmd, "dummy") == 0){
		writeMsg(&handlerUsart1, "CMD: dummy\n");
		//Cambiando el formato para presentar el numero por el puerto serial
		sprintf(bufferData, "number A = %u \n", firstParameter);
		writeMsg(&handlerUsart1, bufferData);

		//Cambiando el formato para presentar el numero por el puerto serial
		sprintf(bufferData, "number B = %u \n", secondParameter);
		writeMsg(&handlerUsart1, bufferData);
	}

	//El comando usermsg sirve para entender como funciona la recepcion de strings enviados
	else if(strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerUsart1, "CMD: usermsg\n");
		writeMsg(&handlerUsart1, userMsg);
		writeMsg(&handlerUsart1, "\n");
	}

	else if(strcmp(cmd, "initLcd") == 0){
		writeMsg(&handlerUsart1, "CMD: initLcd\n");


		LCD_Init(&handlerI2Clcd);
	}

	else if(strcmp(cmd, "testLcd") == 0){
		writeMsg(&handlerUsart1, "CMD: testLcd\n");
		LCD_sendSTR(&handlerI2Clcd, "Test LCD");
	}

	else if (strcmp(cmd, "clearLcd") == 0){
		writeMsg(&handlerUsart1, "CMD: clearLcd\n");
		lcd_clear(&handlerI2Clcd);
	}
	else if (strcmp(cmd, "resetCursor") == 0){
		writeMsg(&handlerUsart1, "CMD: resetCursor\n");
		set_cursor(&handlerI2Clcd, 0, 0);
	}

	else if(strcmp(cmd, "setPeriod") == 0){
		writeMsg(&handlerUsart1, "CMD: setPeriod\n");
		if(firstParameter < 100){
			firstParameter = 100;
		}
		if(firstParameter > 10000){
			firstParameter = 10000;
		}
		handlerStateTimer.TIMx_Config.TIMx_period = firstParameter;
		BasicTimer_Config(&handlerStateTimer);
	}

	else{
		//Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados.
		writeMsg(&handlerUsart1, "WRONG CMD \n");
	}
}

void initSystem(void){
	/*
	 *                        Configurando State LED
	 */
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

	/*
	 * 						Configurando la comunicacion USART
	 */

	/* Configurando los pines sobre los que funciona el USART1 (TX) */
	handlerPinTx.pGPIOx 							= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinTx);

	/* Configurando los pines sobre los que funciona el USART1 (RX) */
	handlerPinRx.pGPIOx 							= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinRx);

	// Configurando la comunicación serial
	handlerUsart1.ptrUSARTx 						= USART1;
	handlerUsart1.USART_Config.USART_baudrate		= USART_BAUDRATE_9600;
	handlerUsart1.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerUsart1.USART_Config.USART_parity			= USART_PARITY_ODD;
	handlerUsart1.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	handlerUsart1.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerUsart1.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

	USART_Config(&handlerUsart1);

	/*
	 * 						Configurando I2C LCD
	 */
	//Señal clock PA8  - I2C3
	handlerI2Cclk.pGPIOx								= GPIOA;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cclk);

	//Señal data PC9  - I2C3
	handlerI2Cdata.pGPIOx								= GPIOC;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cdata);

	//handler I2C
	handlerI2Clcd.ptrI2Cx							= I2C3;
	handlerI2Clcd.slaveAddress						= 0x27; 		//Cargando dirección del esclavo
	handlerI2Clcd.modeI2C							= I2C_MODE_SM;
	i2c_config(&handlerI2Clcd);
}



void BasicTimer2_Callback (void){
	GPIOxTooglePin(&handlerStateLed);
}

void usart1Rx_Callback(void){
	rxData = getRxData();
}
