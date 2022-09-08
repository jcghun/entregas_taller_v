/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @email		   : jucgomezhi@unal.edu.co
 * @brief          : Main program body
 ******************************************************************************
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
//	GPIO_Handler_t handlerUserLed2Pin = {0};
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
	handlerUserInterr.pGPIOx = GPIOC;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	//Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
	GPIO_Config(&handlerUserInterr);
//	GPIO_Config(&handlerUserLed2Pin);


	//Hacemos que el PIN_A5 quede encendido
//	GPIO_WritePin(&handlerUserLedPin, RESET);

	void delayMS(int delay)
	{
			for( ; delay > 0; delay--)
				for(i = 0; i <= 3195; i++){
			}
	}


	/**
	 * Punto 2. Función para cambiar los estados
	 */
	void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){
		//Función creada para cambiar el estado de un pin a un handler debidamente configurado previamente

		//Aquí se hace shift al bit 1 un numero de pin veces, cualquier caso que sea el registro ODR del pin, la operación XOR cambia
		//su estado e invierte ese bit, para posteriormente asignarse al mismo registro.

		pPinHandler->pGPIOx->ODR ^= (1 << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
	}

	/**
	 * Punto 3. Uso de la funcion GPIOxTooglePin con user button.
	 */
	while(1){
		//Si el pin del interruptor no esta presionado, no hacer nada
		if (GPIO_ReadPin(&handlerUserInterr)){
		}
		//En caso contrario cambiar el estado del pin del LED verde
		else{
			GPIOxTooglePin(&handlerUserLedPin);
		}
	}
}



