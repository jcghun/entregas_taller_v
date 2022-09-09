/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Taller 4
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "GPIOxDriver.h"

#define SUMA			'+' //macros (asignación constante al programa)
#define RESTA			'-'
#define MULTIPLICACION	'*'
#define DIVISION		'/'

//Función Ejemplo del Switch Case
uint16_t resultadoOpercaion(uint8_t tipoOperacion, uint8_t numeroA, uint8_t numeroB);
void delay(uint64_t time);


int main(void){
	uint16_t operacionFinal = 0;

	operacionFinal = resultadoOpercaion(SUMA, 12, 11);

	// Ejemplo básico de while
	uint8_t contador = 0;

	while(contador < 10){
		contador += 3;
	}

	/* Actividad #1:
	 * Crear una función llamada delay, que tome un parametro y que no retorne nada
	 * El parámetro que debe recibir tiene que mantener ocupado al MCU haciendo nada NOP()
	*/

	//Con figuracion del handler
	GPIO_Handler_t handlerStateLED = {0};

	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_HIGH;

	GPIO_Config(&handlerStateLED);

	while(1){
		GPIO_WritePin(&handlerStateLED, SET); 	//Enciendo el LED
		delay(150000);							//Delay
		GPIO_WritePin(&handlerStateLED, RESET);	//Apago el LED
		delay(70000);							//Delay
	}
}

//Soluión a la actividad 1

void delay(uint64_t time){
	uint64_t count = 0;

	//No hacer nada mientras el contador no supere (time * 9999)
	while(count <= time){
		__asm("NOP"); //En lugar de NOP();
		count++;
	}
}

uint16_t resultadoOpercaion(uint8_t tipoOperacion, uint8_t numeroA, uint8_t numeroB){
	uint16_t resultado = 0;

	// Estructura Switch case
	switch(tipoOperacion){
	case SUMA:{
		resultado = numeroA + numeroB;
		break;
	}

	case RESTA:{
		resultado = numeroA - numeroB;
		break;
	}

	case MULTIPLICACION:{
		resultado = numeroA * numeroB;
		break;
	}

	case DIVISION:{
		resultado = numeroA / numeroB;
		break;
	}

	default:{
		resultado = 0;
		return resultado;
		break;
	}

	}
}


