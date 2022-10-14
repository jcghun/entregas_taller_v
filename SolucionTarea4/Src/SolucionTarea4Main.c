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
GPIO_Handler_t handlerEncoderCLK			= {0};
GPIO_Handler_t handlerEncoderDT			= {0};
USART_Handler_t handlerUSART2		= {0};

GPIO_Handler_t handlerLED = {0};

EXTI_Config_t handlerExtiEncoderCLK		= {0};
EXTI_Config_t handlerExtiEncoderB		= {0};

/* Definición de prototipos de funciones */
void startTimer(BasicTimer_Handler_t *ptrBTimerHandler);
void defineDirection (void);

//Variables
uint8_t contador = 0;
uint8_t dirCW = 0;
uint8_t dirCCW = 0;

uint8_t clkValue;
uint8_t dtValue;


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

		//Configurando el State LED
		handlerLED.pGPIOx = GPIOA;
		handlerLED.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
		handlerLED.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerLED.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerLED.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerLED.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerLED);
		GPIO_WritePin(&handlerLED, SET);

		//Configurando el pin de entrada encoder A para la EXTI
		handlerEncoderCLK.pGPIOx = GPIOC;
		handlerEncoderCLK.GPIO_PinConfig.GPIO_PinNumber 		= PIN_0;
		handlerEncoderCLK.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
		handlerEncoderCLK.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerEncoderCLK);

		//Configurando la EXTI de encoder A
		handlerExtiEncoderCLK.pGPIOHandler					= &handlerEncoderCLK;
		handlerExtiEncoderCLK.edgeType						= EXTERNAL_INTERRUPT_FALLING_EDGE;

		extInt_Config(&handlerExtiEncoderCLK);

		//Configurando el pin de entrada encoder B para la EXTI
		handlerEncoderDT.pGPIOx = GPIOC;
		handlerEncoderDT.GPIO_PinConfig.GPIO_PinNumber 		= PIN_1;
		handlerEncoderDT.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
		handlerEncoderDT.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerEncoderDT);

		handlerStateTimer.ptrTIMx = TIM2;
		handlerStateTimer.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
		handlerStateTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
		handlerStateTimer.TIMx_Config.TIMx_period 			= 250;
		handlerStateTimer.TIMx_Config.TIMx_interruptEnable 	= 1;

		BasicTimer_Config(&handlerStateTimer);
		startTimer(&handlerStateTimer);

		handlerUSART2.ptrUSARTx = USART2;
		handlerUSART2.USART_Config.USART_mode 			= USART_MODE_RXTX;
		handlerUSART2.USART_Config.USART_baudrate 		= USART_BAUDRATE_115200;
		handlerUSART2.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
		handlerUSART2.USART_Config.USART_parity 		= USART_PARITY_NONE;
		handlerUSART2.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
		handlerUSART2.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

		USART_Config(&handlerUSART2);
    /* Loop forever */
	while(1){

		//Revisar continuamente en que sentido esta avanzando el encoder
		if(dirCW){
			writeMsg(&handlerUSART2, "CW");
		}
		else if(dirCCW){
			writeMsg(&handlerUSART2, "CCW");
		}
		else{

		}
	}
	return 0;
}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerStateLed);
}

void callback_extInt0(void){
	GPIOxTooglePin(&handlerLED);
	defineDirection();
}

void defineDirection(void){
	dtValue = GPIO_ReadPin(&handlerEncoderDT);

	if(dtValue){
		dirCW = 1;
	}
	else{
		dirCCW = 1;
	}
}
