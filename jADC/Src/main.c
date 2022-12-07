/**
 **************************************************************************************************
 * @file           : MainTaller7Parte2.c
 * @authors        : Juan Camilo Gomez
 * @brief          : Main para probar integracion de joystick con lcd
 *
 **************************************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "I2CDriver.h"
#include "LcdDriver.h"
#include "ExtiDriver.h"
#include "SH1106.h"
#include "Fonts.h"
#include "DBWordle.h"


// Definicion de los handlers necesarios
GPIO_Handler_t handlerStateLed = {0};
GPIO_Handler_t handlerPinTx		= {0};
GPIO_Handler_t handlerPinRx		= {0};

// Timer encargado del Blinky
BasicTimer_Handler_t handlerStateTimer 	= {0};

// Utiliza la conexion USB
USART_Handler_t handlerUsart2		= {0};

// Configuración ADC
ADC_Config_t 		adcConfigX 		= {0};
ADC_Config_t 		adcConfigY 		= {0};
GPIO_Handler_t handlerJoystickButton			= {0};
EXTI_Config_t handlerExtiJoystickButton	 	= {0};
BasicTimer_Handler_t	handlerAdcTimer		= {0};

//LCD
GPIO_Handler_t  	handlerI2Cclk = {0};
GPIO_Handler_t  	handlerI2Cdata = {0};

I2C_Handler_t		handlerI2Clcd	={0};
I2C_Handler_t		handlerI2COled	={0};

// Variables USART
uint8_t rxData = 0;
char bufferData[64];
uint8_t counter = 0;

char chars[] = {'A', 'B', 'C', 'D', 'E', 'F', 0x7F, 'G', 'H', 'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

char keyboardPosition[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x28, 0x29, 0x2A,
		0x2B, 0x2C, 0x2D, 0x2E, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x3C, 0x3D,
		0x3E, 0x3F, 0x40, 0x41};

// Variables ADC

int adcValue[2];
//uint16_t adcValueY;
uint8_t adcIsComplete = RESET;

/* Definición de prototipos de funciones */
void InitSystem(void);
uint8_t keyboardPosition2Char(void);

/**
 * Funcion principal del programa.
 * Esta funcion es el corazón del programa!!
 *
 * */
int main(void) {

	// Inicializamos todos los elementos del sistema
	InitSystem();

	uint8_t i;
	uint8_t j;
	uint8_t c;
	for(i=0; i<4; i++){

		for(j=0; j<7; j++){
			if(i==3 && j==6){break;}
			LCD_setCursor(&handlerI2Clcd, i, j);
			LCD_sendata(&handlerI2Clcd, chars[c]);
			delay(1);
			c++;
		}
	}

	LCD_sendCMD(&handlerI2Clcd, 0x02);
	delay(5);
	LCD_sendCMD(&handlerI2Clcd, 0x0F);


	/* Loop forever */
	while (1) {

		// El sistema siempre está verificando si el valor de rxData ha cambiado
        // (lo cual sucede en la ISR de la recepcion (RX).
        // Si este valor deja de ser '\0' significa que se recibio un caracter
        // por lo tanto entra en el bloque if para analizar que se recibio
//		if(rxData != '\0'){
//			// Imprimimos el caracter recibido
//            writeChar(&handlerUsart2, rxData);
//
//			// Iniciamos muestreo del ADC
//			if(rxData == 'c'){
//				// Activamos el TIM4
////				startContinousADC();
//				startTimer(&handlerAdcTimer);
//			}
//			// Paramos muestreo del ADC
//			else if(rxData == 'p'){
//				// Desactivamos el TIM4
//
//				stopTimer(&handlerAdcTimer);
//			}
//			rxData = '\0';
//		}

//		// Mandamos los valores de la conversion ADC
//		if(adcIsComplete == SET){
//			// Seccionamos el valor en un arreglo
//
//			// ****** // ESCRIBA AQUI SU CODIGO // ****** //
//
//			sprintf(bufferData, "%u \t", adcValue[0]);
//
//
//			// Enviamos el dato del ADC resultante
//
//			// ****** // ESCRIBA AQUI SU CODIGO // ****** //
//			writeMsg(&handlerUsart2, bufferData);
//
//			sprintf(bufferData, "%u \n", adcValue[1]);
//			writeMsg(&handlerUsart2, bufferData);
//
//			// Bajamos la bandera
//
//			adcIsComplete = RESET;
//		}

		if(adcValue[0] > 4000){

			cursorShiftR(&handlerI2Clcd);
//			writeChar(&handlerUsart2, LCD_GetX());
//			writeChar(&handlerUsart2, LCD_GetY());
			delay(400);
		}

		else if(adcValue[0] < 50){
			cursorShiftL(&handlerI2Clcd);
//			writeChar(&handlerUsart2, LCD_GetX());
//			writeChar(&handlerUsart2, LCD_GetY());
			delay(400);
		}

		if(adcValue[1] > 4000){
			cursorShiftD(&handlerI2Clcd);
//			writeChar(&handlerUsart2, LCD_GetX());
//			writeChar(&handlerUsart2, LCD_GetY());
			delay(400);
		}

		if(adcValue[1] < 50){
			cursorShiftU(&handlerI2Clcd);
//			writeChar(&handlerUsart2, LCD_GetX());
//			writeChar(&handlerUsart2, LCD_GetY());
			delay(400);
		}
	}

	return 0;
}

/*
 * Funcion encargada de la inicializacion de los elementos del sistema
 */
void InitSystem(void){

	// Configurando el pin para el Led_Blinky
	handlerStateLed.pGPIOx 							= GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	// Cargamos la configuración del Pin del led de estado
	GPIO_Config(&handlerStateLed);

	// Llevamos el Led a un estado de encendido
	GPIO_WritePin(&handlerStateLed, RESET);

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
	handlerUsart2.ptrUSARTx 					= USART2;
	handlerUsart2.USART_Config.USART_baudrate	= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize	= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity		= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits	= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode		= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

	// Cargamos la configuración del USART
	USART_Config(&handlerUsart2);

	// Configurando el Timer2 para que funcione con el blinky
	handlerStateTimer.ptrTIMx 						= TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode			= BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period		= 250;

	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerStateTimer);

	// Activamos el TIM2
	startTimer(&handlerStateTimer);

	// Configuramos el timer del ADC
	handlerAdcTimer.ptrTIMx						= TIM4;
	handlerAdcTimer.TIMx_Config.TIMx_mode	= BTIMER_MODE_UP;
	handlerAdcTimer.TIMx_Config.TIMx_speed 		= BTIMER_SPEED_1ms;
	handlerAdcTimer.TIMx_Config.TIMx_period 	= 150;

	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerAdcTimer);
	startTimer(&handlerAdcTimer);

	//Se establecen las configuraciones del ADC
//	adcConfig.channel				= ADC_CHANNEL_1;
	adcConfigX.channel				= ADC_CHANNEL_0;
	adcConfigX.dataAlignment 		= ADC_ALIGNMENT_RIGHT;
	adcConfigX.resolution 			= ADC_RESOLUTION_12_BIT;
	adcConfigX.samplingPeriod		= ADC_SAMPLING_PERIOD_15_CYCLES;

	//Se carga la configuración del ADC
	adc_Config(&adcConfigX);


	adcConfigY.channel				= ADC_CHANNEL_1;
	adcConfigY.dataAlignment 		= ADC_ALIGNMENT_RIGHT;
	adcConfigY.resolution 			= ADC_RESOLUTION_12_BIT;
	adcConfigY.samplingPeriod		= ADC_SAMPLING_PERIOD_15_CYCLES;

	//Se carga la configuración del ADC
	adc_Config(&adcConfigY);
//	adcConfigTimer();

	//Config the pin for the encoder's button
	handlerJoystickButton.pGPIOx		= GPIOA;
	handlerJoystickButton.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerJoystickButton.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerJoystickButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerJoystickButton);

	//Setting up the encoder's button EXTI
	handlerExtiJoystickButton.pGPIOHandler					= &handlerJoystickButton;
	handlerExtiJoystickButton.edgeType						= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerExtiJoystickButton);

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

	i2c_config(&handlerI2Clcd);

	LCD_Init(&handlerI2Clcd);

}

// ADC (conversión) Callback
void adcComplete_Callback(void){
	// Leemos los valores de la conversión ADC
//	if(adcConfig.channel == ADC_CHANNEL_0){
//		adcValueX = getADC();
//	}
//	else if (adcConfig.channel == ADC_CHANNEL_1) {
//		adcValueY = getADC();
//	}

	if(counter == 0){
		adcValue[0] = getADC();
		counter++;
	}
	else if(counter == 1){
		adcValue[1] = getADC();
		counter = 0;
	}
	// Levantamos la bandera
	adcIsComplete = SET;

//	changeChannel();
}

/* Callback del Timer4 - Hacemos una conversión ADC */
void BasicTimer4_Callback(void){
	// Comienza una convesión ADC
	startSingleADC();
//	changeChannel();
}

/* Callback del Timer2 - Hacemos un blinky... */
void BasicTimer2_Callback(void){
	handlerStateLed.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

/* Callback relacionado con la recepción del USART2
 * El puerto es leido en la ISR (para bajar la bandera de la interrupción)
 * El caracter que se lee es devuelto por la función getRxData
 */
void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

void callback_extInt7(void){
	writeChar(&handlerUsart2, keyboardPosition2Char());
	// Esto no se debe hacer aca, ya que se demora mucho.
}


/*
 * Con esta funcion podemos recuperar un caracter a partir de la posición en el teclado
 */
uint8_t keyboardPosition2Char(void){
	uint8_t newChar;

	//Recuperamos el indice del caracter a partir de la posicion
	uint8_t i;
	for(i=0; i<27; i++){
		if(LCD_GetX() == keyboardPosition[i]){
			break;
		}
	}

	//Devolvemos el caracter a partir del abecedario
	newChar = chars[i];
	return newChar;
}
