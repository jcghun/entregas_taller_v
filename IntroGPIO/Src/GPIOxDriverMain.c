/*
 * GPIOxDriverMain.c
 *
 *  Created on: 29/08/2022
 *      Author: camilo
 *
 *
 *
 * Con este programa se desea mostrar el uso básico de los registros que controlan
 * al Micro (SFR) y la toma adecuada para utilizar los operadores &, |, ~ y =, para
 * cambiar la configuración de algún registro.
 * También es importante para entender la utilidad de los números BIN y HEX
 *
 * Es necesario tener a la mano tanto el manual de referencia del micro como la hoja de
 * datos del micro (ambos son documentos diferentes y complementarios el uno del otro)
 *
 * HAL -> Hardware Abstraction Layer.
 *
 * Este programa introduce el periférico mas simple que tiene el micro, que es el
 * encargado de manejar los pines de cada puerto del micro.
 *
 * Cada PINx de cada puerto GPIO puede desarrollar funciones básicas de tipo entrada
 * y salida de datos digitales (o sea 1's y 0's), además se les puede asignar funciones
 * especiales que generalmente están ligadas a otro periférico adicional (se vera mas
 * adelante).
 *
 * De igual forma, varias caracteristicas pueden ser configuradas para cada PINx
 * específico como son:
 *  - La velocidad a la que puede responder
 *  - Tipo de entrada (pull up, pull down, open drain (flotante))
 *  - Tipo de salida (push-pull, open drain...)
 *  - Entrada análoga
 *
 *  Debemos definir entonces todos los registros que manejan el periférico GPIOx y luego
 *  crear algunas funciones para utilizar adecuadamente el equipo-
 *
 ***********************************************************************
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

	// Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	//Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
	RCC->AHB1ENR  |= 0x00000001;
	GPIOA ->MODER |= 0x00000400;

	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);

	// Este es el ciclo principal, donde se ejecuta to do el programa

	while(1){
		GPIOA->ODR |= (1<<5);
		delayMS(600);
		GPIOA->ODR &= ~(1<<5);
		delayMS(600);
	}
}

void delayMS(int delay)
{
		for( ; delay > 0; delay--)
			for(i = 0; i <= 3195; i++){
		}

}
