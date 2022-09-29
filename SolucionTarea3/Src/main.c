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
GPIO_Handler_t handlerBlinkyLed		= {0};
GPIO_Handler_t handlerTxPin			= {0};
USART_Handler_t handlerUSART1		= {0};
uint8_t blinky = 0;

int main(void){
		handlerBlinkyLed.pGPIOx = GPIOA;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerBlinkyLed);
		GPIO_WritePin(&handlerBlinkyLed, SET);


		handlerTxPin.pGPIOx = GPIOA;
		handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
		handlerTxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
		handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_HIGH;
		handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;

		GPIO_Config(&handlerTxPin);


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


void BasicTimerX_Callback(void){
	blinky = !blinky;

	if(blinky){
		GPIO_WritePin(&handlerBlinkyLed, SET);
		writeChar(&handlerUSART1, "H");
	}else{
		GPIO_WritePin(&handlerBlinkyLed, RESET);
		writeChar(&handlerUSART1, "H");
	}
}
