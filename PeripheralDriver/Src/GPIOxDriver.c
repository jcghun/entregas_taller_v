/*
 * GPIOxDriver.c
 *
 *  Created on: 29/08/2022
 *      Author: camilo
 *
 *  Este archivo es la parte del programa donde escribimos adecuadamente el control,
 *  para que sea lo mas genérico posible, de forma que independiente del puerto GPIO y
 *  el PIN seleccionados, el programa se ejecute y configure todo correctamente.
 *
 */

#include "GPIOxDriver.h"

/**
 * Para cualquier periférico, hay varios pasos que siempre se deben seguir en un
 * orden estricto para poder que el sistema permita configurar el periférico X.
 * Lo primero y mas importante es activar la señal del reloj principal hacia ese
 * elemento especifico (relacionado con el periférico RCC), a esto llamaremos
 * simplemente "activar el periférico o activar la señal de reloj del periférico)
 */
void GPIO_Config (GPIO_Handler_t *pGPIOHandler){ //Pasando un puntero a la función

	//Variable para hacer todo paso a paso
	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	// 1) Activar el periférico
	// Verificamos para GPIOA
	if(pGPIOHandler->pGPIOx == GPIOA){
		// Escribimos 1 (SET) en la posición correspondiente al GPIOA
		RCC ->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOAEN_Pos);
	}
	//Verificamos para GPIOB
	else if(pGPIOHandler->pGPIOx == GPIOB){
		//Escribimos 1 (SET) en la posción correspondiente al GPIOB
		RCC ->AHB1ENR |=(SET <<RCC_AHB1ENR_GPIOBEN_Pos);
	}
	//Verificamos para GPIOC
	else if(pGPIOHandler->pGPIOx == GPIOC){
		//Escribimos 1 (SET) en la posción correspondiente al GPIOC
		RCC ->AHB1ENR |=(SET <<RCC_AHB1ENR_GPIOCEN_Pos);
	}
	//Verificamos para GPIOD
	else if(pGPIOHandler->pGPIOx == GPIOD){
		//Escribimos 1 (SET) en la posción correspondiente al GPIOD
		RCC ->AHB1ENR |=(SET <<RCC_AHB1ENR_GPIODEN_Pos);
	}
	//Verificamos para GPIOE
	else if(pGPIOHandler->pGPIOx == GPIOE){
		//Escribimos 1 (SET) en la posción correspondiente al GPIOE
		RCC ->AHB1ENR |=(SET <<RCC_AHB1ENR_GPIOEEN_Pos);
	}
	//Verificamos para GPIOH
	else if(pGPIOHandler->pGPIOx == GPIOH){
		//Escribimos 1 (SET) en la posción correspondiente al GPIOH
		RCC ->AHB1ENR |=(SET <<RCC_AHB1ENR_GPIOHEN_Pos);
	}

	//Despues de activado, podemos comenzar a configurar.

	// 2) Configurando el registro GPIOx_MODER
	// Acá estamos leyendo la config, moviendo "PinNumber" veces hacia la izquierda ese valor (shift left)
	// y todo eso lo cargamos en la variable auxConfig
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	// Antes de cargar el nuevo valor, limpiamos los bits especificos de ese registro (debemos escribir 0b00)
	// para lo cual aplicamos una máscara y una operación bitwise AND
	pGPIOHandler->pGPIOx->MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	// Cargamos a auxConfig en el registro MODER
	pGPIOHandler->pGPIOx->MODER |= auxConfig;

	// 3) Configurando el registro GPIOx_DTYPER
	// De nuevo, leemos y movemos el valor un número "PinNumber" de veces
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	//Limpiamos antes de cargar
	pGPIOHandler->pGPIOx->OTYPER &= ~(SET << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	// cargamos el resultado sobre el registro adecuado
	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;

	// 4) Configurando ahora la velocidad
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	// Limpiando la posición antes de cargar la nueva configuración
	pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	// cargamos el resultado sobre el registro adecuado
	pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;

	// 5) Configurando si se desea pull-up, pull-down o flotante.
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	// Limpiando la posición antes de cargar la nueva configuración
	pGPIOHandler ->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	// cargamos el resultado sobre el registro adecuado
	pGPIOHandler->pGPIOx->PUPDR |= auxConfig;

	// Esta es la parte para la configuración de las funciones alternativas... se verá luego
	if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		// Seleccionamos primero si se debe utilizar el registro bajo (AFRL) o el alto (AFRH)
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){
			// Estamos en el registro AFRL, que controla los pines del PIN_0 al PIN_7
			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;

			//Limpiamos primero la posición del registro que deseamos escribir a continuación
			pGPIOHandler->pGPIOx->AFR[0] &= ~(0b1111 << auxPosition);

			// Y escribimos el valor configurado en la posición seleccionada
			pGPIOHandler->pGPIOx->AFR[0] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else{
			// Estamos en el registro AFRH, que controla los pines del PIN_8 al PIN_15
			auxPosition = 4 *(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber -8);

			//Limpiamos primero la posición del registro que deseamos escribir a continuación
			pGPIOHandler->pGPIOx->AFR[1] &= ~(0b1111 << auxPosition);

			// Y escribimos el valor configurado en la posición seleccionada
			pGPIOHandler->pGPIOx->AFR[1] |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
	}
} //Fin del GPIO_config



/**
 *  Función utilizada para cambiar de estado el pin entregado en el handler, asignando
 *  el valor entregado en la variable newState
 */
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState){
	//Limpiamos la posición que deseamos
	//pPinHandler->pGPIOx->ODR &= ~ (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
	if(newState == SET){
		// Trabajando con la parte baja del registro
		pPinHandler->pGPIOx->BSRR |= (SET <<pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
	}
	else{
		// Trabajando con la parte alta del registro
		pPinHandler->pGPIOx->BSRR |= (SET <<(pPinHandler->GPIO_PinConfig.GPIO_PinNumber + 16));
	}
}

/**
 *  Función para leer el estado de un pin especifico
 */
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler){
	// Creamos una variable auxiliar la cual luego retomaremos
	uint32_t pinValue = 0;

	// Cargamos el valor del registro IDR, desplazando a derecha tantas veces como la ubicación
	// del pin especifico
	pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	pinValue &= ~(0xFFFE); //Limpiar todas excepto la primera, mascara de 16 bits

	return pinValue;
}

void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){
	//Función creada para cambiar el estado de un pin a un handler debidamente configurado previamente

	//Aquí se hace shift al bit 1 un numero de pin veces, cualquier caso que sea el registro ODR del pin, la operación XOR cambia
	//su estado e invierte ese bit, para posteriormente asignarse al mismo registro.

	pPinHandler->pGPIOx->ODR ^= (1 << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
}
