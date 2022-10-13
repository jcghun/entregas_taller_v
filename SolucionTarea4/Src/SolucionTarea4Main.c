/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Solución Tarea 4
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "ExtiDriver.h"

BasicTimer_Handler_t handlerStateTimer	= {0};
GPIO_Handler_t handlerStateLed		= {0};
GPIO_Handler_t handlerEncoderA			= {0};
GPIO_Handler_t handlerEncoderB			= {0};
USART_Handler_t handlerUSART1		= {0};

EXTI_Config_t handlerExtiEncoderA		= {0};
EXTI_Config_t handlerExtiEncoderB		= {0};

/* Definición de prototipos de funciones */
void startTimer(BasicTimer_Handler_t *ptrBTimerHandler);


int main(void){

		//Configurando el State LED
		handlerStateLed.pGPIOx = GPIOA;
		handlerStateLed.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
		handlerStateLed.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerStateLed.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerStateLed);
		GPIO_WritePin(&handlerStateLed, SET);

		//Configurando el pin de entrada encoder A para la EXTI
		handlerEncoderA.pGPIOx = GPIOC;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinNumber 		= PIN_0;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
		handlerEncoderA.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerEncoderA);

		//Configurando la EXTI de encoder A
		handlerExtiEncoderA.pGPIOHandler					= &handlerEncoderA;
		handlerExtiEncoderA.edgeType						EXTERNAL_INTERRUPT_FALLING_EDGE;

		extInt_Config(&handlerExtiEncoderA);

		//Configurando el pin de entrada encoder B para la EXTI
		handlerEncoderB.pGPIOx = GPIOC;
		handlerEncoderB.GPIO_PinConfig.GPIO_PinNumber 		= PIN_1;
		handlerEncoderB.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
		handlerEncoderB.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerEncoderB);

		//Configurando la EXTI de encoder B
		handlerExtiEncoderB.pGPIOHandler					= &handlerEncoderB;
		handlerExtiEncoderB.edgeType						EXTERNAL_INTERRUPT_FALLING_EDGE;

		extInt_Config(&handlerExtiEncoderB);

		handlerStateTimer.ptrTIMx = TIM2;
		handlerStateTimer.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
		handlerStateTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
		handlerStateTimer.TIMx_Config.TIMx_period 			= 250;
		handlerStateTimer.TIMx_Config.TIMx_interruptEnable 	= 1;

		BasicTimer_Config(&handlerStateTimer);
		startTimer(&handlerStateTimer);

//		handlerUSART1.ptrUSARTx = USART1;
//		handlerUSART1.USART_Config.USART_mode 			= USART_MODE_TX;
//		handlerUSART1.USART_Config.USART_baudrate 		= USART_BAUDRATE_9600;
//		handlerUSART1.USART_Config.USART_datasize 		= USART_DATASIZE_9BIT;
//		handlerUSART1.USART_Config.USART_parity 		= USART_PARITY_EVEN;
//		handlerUSART1.USART_Config.USART_stopbits 		= USART_STOPBIT_1;

//		USART_Config(&handlerUSART1);
    /* Loop forever */
	while(1){

	}
	return 0;
}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerStateLed);
}

void callback_extInt0(void){

}
