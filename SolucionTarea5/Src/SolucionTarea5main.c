/**
 ******************************************************************************
 * @file           : SolucionTarea5main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Main program body
 ******************************************************************************
 */

#include <BasicTimer.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "PllDriver.h"
#include "USARTxDriver.h"

RCC_Handler_t		handlerRCC_PLL;
GPIO_Handler_t		handlerMCO1;

BasicTimer_Handler_t handlerStateTimer = {0};
GPIO_Handler_t  	handlerStateLed = {0};

GPIO_Handler_t handlerPinTx		= {0};
GPIO_Handler_t handlerPinRx		= {0};

// Utiliza la conexion USB
USART_Handler_t handlerUsart2 = {0};

uint8_t rxData = 0;
uint16_t counter = 0;
char bufferCounter[64];

void initSystem(void);

int main(void)
{
	initSystem();

	while(1){
		writeMsg(&handlerUsart2, "\nCounter = ");
		sprintf(bufferCounter, "%u\n", counter);
		writeMsg(&handlerUsart2, bufferCounter);
		delay(3000);
		counter++;
	}
	return 0;
}


void initSystem(void){

	//Se escoge los prescaler, M=8, N=100, P=2
	handlerRCC_PLL.ptrRCC		= RCC;
	handlerRCC_PLL.PLL_M		= 0b00001000;
	handlerRCC_PLL.PLL_N		= 0b1100100;
	handlerRCC_PLL.PLL_P		= 0b00;

	pll_config(handlerRCC_PLL);

	handlerMCO1.pGPIOx			= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_HIGH;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;

	GPIO_Config(&handlerMCO1);



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
	handlerStateTimer.TIMx_Config.TIMx_speed 				= 50000;
	handlerStateTimer.TIMx_Config.TIMx_period 				= 500;
	handlerStateTimer.TIMx_Config.TIMx_interruptEnable 		= 1;

	BasicTimer_Config(&handlerStateTimer);
	startTimer(&handlerStateTimer);

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
		handlerUsart2.USART_Config.USART_baudrate	= USART_BAUDRATE_CUSTOM;
		handlerUsart2.USART_Config.USART_datasize	= USART_DATASIZE_8BIT;
		handlerUsart2.USART_Config.USART_parity		= USART_PARITY_NONE;
		handlerUsart2.USART_Config.USART_stopbits	= USART_STOPBIT_1;
		handlerUsart2.USART_Config.USART_mode		= USART_MODE_RXTX;
		handlerUsart2.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

		// Cargamos la configuración del USART
		USART_Config(&handlerUsart2);

}

void BasicTimer2_Callback (void){
	GPIOxTooglePin(&handlerStateLed);
}

void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}
