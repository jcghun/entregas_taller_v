/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"

GPIO_Handler_t handlerUserLed		= {0};
GPIO_Handler_t handlerUserButton	= {0};
EXTI_Config_t handlerExtiUsarButton	 = {0};

int main(void)
{
    //Configurando led
	handlerUserLed.pGPIOx			= GPIOA;
	handlerUserLed.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerUserLed.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerUserLed.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerUserLed.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;
	GPIO_Config(&handlerUserLed);
	GPIO_WritePin(&handlerUserLed, SET);

	//Config button
	handlerUserButton.pGPIOx		= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber		= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	//Config exti
	handlerExtiUsarButton.pGPIOHandler	= &handlerUserButton;
	handlerExtiUsarButton.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerExtiUsarButton);

	while(1){
		__NOP();
	}
}

void callback_extInt13(void){
	GPIOxTooglePin(&handlerUserLed);
}