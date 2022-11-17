/*
 * PllDriver.h
 *
 *  Created on: 15/11/2022
 *      Author: Juan Camilo Gomez Hidalgo
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"

typedef struct
{
	RCC_TypeDef			*ptrRCC;
	uint8_t				PLL_M;
	uint16_t			PLL_N;
	uint8_t				PLL_P;
}RCC_Handler_t;

void pll_config(RCC_Handler_t ptrHandlerRCC);


#endif /* PLLDRIVER_H_ */
