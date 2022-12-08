/**
 ******************************************************************************
 * @file           : SolucionTarea6Main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Main program body
 ******************************************************************************
 */

#include <stdint.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
#include "PwmDriver.h"
#include "CaptureFrecDriver.h"

/* Definicion de variables */
GPIO_Handler_t handlerStateLed = {0};
GPIO_Handler_t handlerPinTx = {0};
GPIO_Handler_t handlerPinRx = {0};

BasicTimer_Handler_t handlerStateTimer	= {0};

GPIO_Handler_t			handlerPinPwmChannel = {0};
PWM_Handler_t			handlerSignalPWM	= {0};

GPIO_Handler_t handlerPinCaptureFrec	= {0};
uint32_t rawPeriod = 0;

USART_Handler_t handlerUsart2 = {0};
uint8_t rxData = 0;
char bufferData[64]= "esto es una prueba";

Capture_Handler_t handlerCapturaFrec	= {0};

uint16_t duttyValue = 5000;

/* Prototipos de funciones */
void initSystem(void);
void cmdCapture (void);

int main(void)
{
	initSystem();

	while(1){
	}
}

void initSystem(void){
	//Configuring State LED
	handlerStateLed.pGPIOx 									= GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerStateLed);
	GPIO_WritePin(&handlerStateLed, SET);

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

		//Configuring Timer2 to work with the State LED
		handlerStateTimer.ptrTIMx 								= TIM2;
		handlerStateTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
		handlerStateTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_100us;
		handlerStateTimer.TIMx_Config.TIMx_period 				= 2500;
		handlerStateTimer.TIMx_Config.TIMx_interruptEnable 		= 1;

		BasicTimer_Config(&handlerStateTimer);
		startTimer(&handlerStateTimer);


		// Configuracion del PWM
		handlerPinPwmChannel.pGPIOx								= GPIOC;
		handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
		handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
		handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
		handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
		handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
		handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;

		GPIO_Config(&handlerPinPwmChannel);

		// configuracion del PWM  canal  3
			handlerSignalPWM.ptrTIMx			= TIM3;
			handlerSignalPWM.config.channel		= PWM_CHANNEL_2;
			handlerSignalPWM.config.duttyCicle	= duttyValue;
			handlerSignalPWM.config.prescaler	= 16;
			handlerSignalPWM.config.periodo		= 20000;

			pwm_Config(&handlerSignalPWM);
			enableOutput(&handlerSignalPWM);
			startPwmSignal(&handlerSignalPWM);

			//TIMER 4.
			/*configuracion del pin para la captura de la señel
			 * que será en el pin PB8, canal CH3 del Timer 4 */

			handlerPinCaptureFrec.pGPIOx								= GPIOB;
			handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
			handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
			handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
			handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
			handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
			handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;

			GPIO_Config(&handlerPinCaptureFrec);

			// TIMER 4  CANAL 3
			handlerCapturaFrec.ptrTIMx                  = TIM4;
			handlerCapturaFrec.config.channel           = CAPTURE_CHANNEL_1;
			handlerCapturaFrec.config.edgeSignal        = CAPTURE_FALLING_EDGE;
			handlerCapturaFrec.config.prescalerCapture  = CAPTURE_PREESCALER_1_1;
			handlerCapturaFrec.config.timerSpeed        = CAPTURE_TIMER_SPEED_100us;

			capture_Config(&handlerCapturaFrec);
}

void BasicTimer2_Callback (void){
	GPIOxTooglePin(&handlerStateLed);
}

void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

//Callback de la interrupcion generada por el flanco de bajada
void CaptureFrec4_Callback(void){
	cmdCapture();
}

//Funcion que permite utilizar el serial para controlar el dutty pwm y para capturar frecuencia
void cmdCapture (void){
	if(rxData == 'd'){
		duttyValue -= 100;
		//Lanzamos un nuevo ciclo de adquisiciones
		updateDuttyCycle(&handlerSignalPWM, duttyValue);
		sprintf(bufferData, "dutty = %u \n", (unsigned int) duttyValue);
		writeMsg(&handlerUsart2, bufferData);
		rxData = '\0';
		}
	else if(rxData == 'u'){
		duttyValue = duttyValue + 100;
		//lanzamos un nuevo ciclo de adquisiciones
		updateDuttyCycle(&handlerSignalPWM, duttyValue);
		sprintf(bufferData, "dutty = %u \n",(unsigned int) duttyValue);	// %u es el especificador de formato <- se puede buscar en google
		writeMsg(&handlerUsart2, bufferData);		// /n sirve para bajar de renglón
		rxData = '\0';
	}
	else if(rxData =='c'){

		rawPeriod =getPeriodFrec(&handlerCapturaFrec);
		sprintf(bufferData, "rawPeriod = %u \n",(unsigned int) rawPeriod);
		writeMsg(&handlerUsart2, bufferData);
		rxData = '\0';

	}
}

/*
 * Hola Camilo, es muy particular como decides controlar el fluijo del programa, dependiendo de la interrupción generada por la captura.
 * En general uno no deberia tener un cosas sucediendo permanentemente, sino que se deberia poder seleccionar y lanzar/detener lo
 * que uno necesita.
 *
 * Calificación = 4.5
 * */
