/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Solución Tarea 3
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"

BasicTimer_Handler_t handlerTimer2	= {0};
GPIO_Handler_t handlerStateLed		= {0};
GPIO_Handler_t handlerEncoderA			= {0};
USART_Handler_t handlerUSART1		= {0};
uint8_t blinky = 0;

int main(void){
		handlerStateLed.pGPIOx = GPIOA;
		handlerStateLed.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
		handlerStateLed.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerStateLed.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerStateLed);
		GPIO_WritePin(&handlerStateLed, SET);


		handlerEncoderA.pGPIOx = GPIOA;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_HIGH;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;

		GPIO_Config(&handlerEncoderA);


		handlerTimer2.ptrTIMx = TIM2;
		handlerTimer2.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
		handlerTimer2.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_100us;
		handlerTimer2.TIMx_Config.TIMx_period 			= 2500;
		handlerTimer2.TIMx_Config.TIMx_interruptEnable 	= 1;

		BasicTimer_Config(&handlerTimer2);

		handlerUSART1.ptrUSARTx = USART1;
		handlerUSART1.USART_Config.USART_mode 			= USART_MODE_TX;
		handlerUSART1.USART_Config.USART_baudrate 		= USART_BAUDRATE_9600;
		handlerUSART1.USART_Config.USART_datasize 		= USART_DATASIZE_9BIT;
		handlerUSART1.USART_Config.USART_parity 		= USART_PARITY_EVEN;
		handlerUSART1.USART_Config.USART_stopbits 		= USART_STOPBIT_1;

		USART_Config(&handlerUSART1);
    /* Loop forever */
	while(1){

	}
	return 0;
}


void BasicTimer2_Callback(void){
	blinky = !blinky;

	if(blinky){
		GPIO_WritePin(&handlerStateLed, SET);
	}else{
		GPIO_WritePin(&handlerStateLed, RESET);
	}
}
