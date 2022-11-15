/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
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
#include "RTC_Driver.h"

GPIO_Handler_t  	handlerStateLed = {0};
GPIO_Handler_t 		handlerPinTx		= {0};
GPIO_Handler_t 		handlerPinRx		= {0};
GPIO_Handler_t		handlerUserInterr	= {0};

BasicTimer_Handler_t handlerStateTimer = {0};

USART_Handler_t handlerUsart1 = {0};

RTC_Handler_t		handlerRTC	={0};

void initSystem(void);

int main(void)
{
	initSystem();



	while(1){
//		writeMsg(&handlerUsart1,(char*) read_time());
		writeChar(&handlerUsart1, (char) read_time()[0]);
		delay(1000);
		writeMsg(&handlerUsart1,(char*) read_date());
		delay(1000);
	}
}

void initSystem(void){

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

	//Pin 13 (USER BUTTON)
	handlerUserInterr.pGPIOx = GPIOC;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	GPIO_Config(&handlerUserInterr);


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

	// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart1.ptrUSARTx 						= USART1;
	handlerUsart1.USART_Config.USART_baudrate		= USART_BAUDRATE_115200;
	handlerUsart1.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerUsart1.USART_Config.USART_parity			= USART_PARITY_ODD;
	handlerUsart1.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	handlerUsart1.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerUsart1.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

	USART_Config(&handlerUsart1);

	//Configuranto el RTC
	handlerRTC.RTC_Hours				= 9;
	handlerRTC.RTC_Minutes				=30;
	handlerRTC.RTC_Seconds				=30;
	handlerRTC.RTC_Days					= 7;
	handlerRTC.RTC_Months				= 11;
	handlerRTC.RTC_Years				= 2022;
	handlerRTC.RTC_Wdu					= MONDAY;
	handlerRTC.RTC_AmPm					= RTC_AMPM;

	rtc_Config(&handlerRTC);
}

void BasicTimer2_Callback (void){
	GPIOxTooglePin(&handlerStateLed);
}
