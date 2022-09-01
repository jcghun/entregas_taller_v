/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Configuración inicial
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
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

uint8_t dummy = 0;
uint8_t *prtEjemplo;
uint32_t valorPuntero = 0;

int main(void){

	//Cambiando el valor de una variable
	dummy = 30;

	//Haciendo que el puntero apunte a la variable dummy
	ptrEjemplo = &dummy;

	//Modificando de forma indirecta el valor alamacenado en la casilla a donde apunta el puntero
	*ptrEjemplo += 45;

	//Observando el valor de posición de memoria
	valorPuntero = (int)ptrEjemplo;

	ptrEjemplo++;
	*ptrEjemplo = 0xAC;

    /* Loop forever */
	while(1){

	}
	return 0;
}
