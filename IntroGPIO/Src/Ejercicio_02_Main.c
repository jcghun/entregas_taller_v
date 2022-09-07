/*
 * Ejercicio_02_Main.c
 *
 *  Created on: 6/09/2022
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
	GPIO_Handler_t handlerUserInterr = {0};

	// Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	// Deseamos trabajar con el puerto GPIOC
//	handlerUserLedPin.pGPIOx = GPIOC;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
//	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	//Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
//	GPIO_Config(&handlerUserInterr);
//	RCC->AHB1ENR  |= 0x00000001;
//	GPIOA ->MODER |= 0x00000400;

	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);
//	GPIO_ReadPin(&handlerUserInterr);

	// Este es el ciclo principal, donde se ejecuta to do el programa

	while(1){

		if (GPIOC -> IDR & (1<<13)){
			GPIOA->ODR |= (1<<5);
			delayMS(200);
			GPIOA->ODR &= ~(1<<5);
			delayMS(200);
		}
		else{
			GPIOA->ODR |= (1<<5);
			delayMS(500);
			GPIOA->ODR &= ~(1<<5);
			delayMS(500);
		}
	}
}

void delayMS(int delay)
{
		for( ; delay > 0; delay--)
			for(i = 0; i <= 3195; i++){
		}

}
