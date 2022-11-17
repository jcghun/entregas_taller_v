/*
 * PllDriver.c
 *
 *  Created on: 15/11/2022
 *      Author: Juan Camilo Gomez Hidalgo
 */

#include <stdint.h>
#include "PllDriver.h"

void pll_config(RCC_Handler_t ptrHandlerRCC){

	if(ptrHandlerRCC.ptrRCC == RCC){

		//Configurar los WS para que la flash pueda responder a la frecuencia que se desea trabajar
		FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

		//Encendemos el PLL
		ptrHandlerRCC.ptrRCC->CR |= RCC_CR_PLLON;

		while(!(ptrHandlerRCC.ptrRCC->CR & RCC_CR_PLLRDY)){
			__NOP();
		}

		//Configurar la fuente de PLL para que sea HSI
		ptrHandlerRCC.ptrRCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;

		//Configurar el valor prescaler M  2 ≤ M ≤ 63.
		ptrHandlerRCC.ptrRCC->PLLCFGR |= ptrHandlerRCC.PLL_M;

		//Configurar el valor N 192 ≤ N ≤ 432.
		ptrHandlerRCC.ptrRCC->PLLCFGR |= (ptrHandlerRCC.PLL_N << RCC_PLLCFGR_PLLN_Pos);

		//Configurar el valor P (2, 4, 6, 8)
		ptrHandlerRCC.ptrRCC->PLLCFGR |= (ptrHandlerRCC.PLL_P << RCC_PLLCFGR_PLLP_Pos);



		//Elegimos al PLL como SYSCLK y chequeamos que cargo su configuracion
		ptrHandlerRCC.ptrRCC->CFGR |= RCC_CFGR_SW_PLL;

		while(!(ptrHandlerRCC.ptrRCC->CFGR & RCC_CFGR_SWS_PLL)){
			__NOP();
		}

		//Prescaler AHB prescaler
		ptrHandlerRCC.ptrRCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		//Prescaler PPRE1, APB Low speed prescaler
		ptrHandlerRCC.ptrRCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

		//Prescaler PPRE2, APB high-speed prescaler
		ptrHandlerRCC.ptrRCC->CFGR |= RCC_CFGR_PPRE2_DIV1;


		//Configurando el prescaler del MCO1 prescaler (/5)
		ptrHandlerRCC.ptrRCC->CFGR |= RCC_CFGR_MCO1PRE;
		//Configurando el MCO1 para que seleccione PLL
		ptrHandlerRCC.ptrRCC->CFGR |= RCC_CFGR_MCO1;

	}
}
