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
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"

BasicTimer_Handler_t handlerTimer2	= {0};
GPIO_Handler_t handlerBlinkyLed		= {0};
uint8_t blinky = 0;

int main(void){
		handlerBlinkyLed.pGPIOx = GPIOA;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerBlinkyLed);
		GPIO_WritePin(&handlerBlinkyLed, SET);

		handlerTimer2.ptrTIMx = TIM2;
		handlerTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
		handlerTimer2.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
		handlerTimer2.TIMx_Config.TIMx_period = 1500;
		handlerTimer2.TIMx_Config.TIMx_interruptEnable = 1;

		BasicTimer_Config(&handlerTimer2);

    /* Loop forever */
	while(1){

	}
	return 0;
}


void BasicTimerX_Callback(void){
	blinky = !blinky;

	if(blinky){
		GPIO_WritePin(&handlerBlinkyLed, SET);
	}else{
		GPIO_WritePin(&handlerBlinkyLed, RESET);
	}
}
