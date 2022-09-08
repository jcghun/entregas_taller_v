/*
 * Ejercicio_3_Main.c
 *
 *  Created on: 7/09/2022
 *      Author: camilo
 */


#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

void delayMS(int delay);

unsigned int i = 0;


/* Función principal del programa. Es acá donde se ejecuta todo */
int main(void){

	//*********
	// Definimos el handler para el PIN que deseamos configurar
	GPIO_Handler_t handlerUserLedPin = {0};
	GPIO_Handler_t handlerUserLed2Pin = {0};
//	GPIO_Handler_t handlerUserInterr = {0};

	// Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	// Deseamos trabajar con el puerto GPIOB
	handlerUserLed2Pin.pGPIOx = GPIOB;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					// Ninguna función

	// Deseamos trabajar con el puerto GPIOC
//	handlerUserLedPin.pGPIOx = GPIOC;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLDOWN;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	//Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
	GPIO_Config(&handlerUserLed2Pin);


	RCC->AHB1ENR  |= (1<<2); //Activar el reloj del puerto C


	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);
	//GPIO_WritePin(&handlerUserLed2Pin, SET);

	// Este es el ciclo principal, donde se ejecuta to do el programa

	while(1){

		if ((GPIOC -> IDR & (1<<13)) || (GPIOC -> IDR & (1<<6))){


			GPIOA ->ODR |= (1<<5);

			GPIOB -> ODR |= (1<<9);
		}
		else {
			GPIOA -> ODR |= (1<<5);
			GPIOB ->ODR |= (1<<9);
			delayMS(100);
			GPIOB -> ODR &= ~(1<<9);
			delayMS(100);
		}

	}
}

void delayMS(int delay)
{
		for( ; delay > 0; delay--)
			for(i = 0; i <= 3195; i++){
		}
}
