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
	GPIO_Handler_t handlerUserLed2Pin = {0};
	GPIO_Handler_t handlerUserLed3Pin = {0};
	GPIO_Handler_t handlerUserLed4Pin = {0};
	GPIO_Handler_t handlerUserInterr = {0};
	GPIO_Handler_t handlerUserInterExt = {0};

	// Deseamos trabajar con el puerto GPIOA Pin 5 (USER LED)
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función


	// Deseamos trabajar con el puerto GPIOC Pin 6 (Interruptor Externo)
	handlerUserInterExt.pGPIOx = GPIOC;
	handlerUserInterExt.GPIO_PinConfig.GPIO_PinNumber		= PIN_6;
	handlerUserInterExt.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserInterExt.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUserInterExt.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLDOWN;  //COnfigurado el modo PULL-DOWN en PC6
	handlerUserInterExt.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;
	handlerUserInterExt.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					// Ninguna función

	//Pin 13 (USER BUTTON)
	handlerUserInterr.pGPIOx = GPIOC;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserInterr.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	//Pin 10 (Led 2)
	handlerUserLed2Pin.pGPIOx = GPIOC;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLed2Pin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					// Ninguna función

	//Pin 11 (LED 3)
	handlerUserLed3Pin.pGPIOx = GPIOC;
	handlerUserLed3Pin.GPIO_PinConfig.GPIO_PinNumber		= PIN_11;
	handlerUserLed3Pin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLed3Pin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUserLed3Pin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLed3Pin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLed3Pin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					// Ninguna función

	//Pin 12 (LED 4)
	handlerUserLed4Pin.pGPIOx = GPIOC;
	handlerUserLed4Pin.GPIO_PinConfig.GPIO_PinNumber		= PIN_12;
	handlerUserLed4Pin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLed4Pin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUserLed4Pin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLed4Pin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLed4Pin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					// Ninguna función

	//Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
	GPIO_Config(&handlerUserInterr);
	GPIO_Config(&handlerUserInterExt);
	GPIO_Config(&handlerUserLed2Pin);
	GPIO_Config(&handlerUserLed3Pin);
	GPIO_Config(&handlerUserLed4Pin);


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
		//Si el pin del interruptor no esta presionado (Entra 1 en PC13, vease Diagrama de microcontrolador), no hacer nada
		if (GPIO_ReadPin(&handlerUserInterr)){
		}
		//En caso contrario cambiar el estado del pin del LED verde con GPIOxTooglePin
		else{
			GPIOxTooglePin(&handlerUserLedPin);
		}
	}


	/**
	 * Punto 4. (Por favor eliminar de comentarios para verificar y comentar el punto 3) Uso de un boton externo para controlar
	 * 3 LED externos conectados en los pines PC10, PC11 y PC12.
	*/

	//Antes de la hora de entrega el codigo que sigue estaba comentado, solammente se quitó de comentarios

	//Funcion utilizada para crear un lapso de tiempo
	void delayMS(int delay)
	{
			for( ; delay > 0; delay--)
				for(i = 0; i <= 3195; i++){
			}
	}

	while(1){
		//Si el boton es pulsado encender los LED y apagarlos en un orden especifico con tiempos diferentes
		if(GPIOC->IDR & (1<<6)){
			GPIOC->ODR |= (1<<10); 	//Encender LED 2
			GPIOC->ODR |= (1<<11); 	//Encender LED 3
			GPIOC->ODR |= (1<<12); 	//Encender LED 4
			delayMS(1500);			//Delay de ~5s
			GPIOC->ODR &= ~(1<<12);	//Apagar LED 4
			delayMS(600);			//Delay de ~2s
			GPIOC->ODR &= ~(1<<11); //Apagar LED 3
			delayMS(300);			//Delay de ~1s
			GPIOC->ODR &= ~(1<<10);	//Apagar LED 2
		}
		//Si el boton no es pulsado mantener los LED apagados
		else{
			GPIOC->ODR &= ~(1<<12);
			GPIOC->ODR &= ~(1<<11);
			GPIOC->ODR &= ~(1<<10);
		}


	}

}



