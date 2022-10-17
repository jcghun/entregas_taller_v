/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Solución Tarea 4
 ******************************************************************************
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "ExtiDriver.h"

BasicTimer_Handler_t handlerStateTimer	= {0};
BasicTimer_Handler_t handlerDisplayTimer	= {0};
GPIO_Handler_t handlerStateLed		= {0};
GPIO_Handler_t handlerEncoderCLK			= {0};
GPIO_Handler_t handlerEncoderDT			= {0};
USART_Handler_t handlerUSART2		= {0};

//Handlers LED 7 segmentos
GPIO_Handler_t handlerDisplayRight	= {0};
GPIO_Handler_t handlerDisplayLeft	= {0};

GPIO_Handler_t handlerDisplayA		= {0};
GPIO_Handler_t handlerDisplayB		= {0};
GPIO_Handler_t handlerDisplayC		= {0};
GPIO_Handler_t handlerDisplayD		= {0};
GPIO_Handler_t handlerDisplayE		= {0};
GPIO_Handler_t handlerDisplayF		= {0};
GPIO_Handler_t handlerDisplayG		= {0};

GPIO_Handler_t handlerLED = {0};
GPIO_Handler_t handlerUserButton	= {0};
EXTI_Config_t handlerExtiUsarButton	 = {0};

EXTI_Config_t handlerExtiEncoderCLK		= {0};
EXTI_Config_t handlerExtiEncoderB		= {0};

/* Definición de prototipos de funciones */
void startTimer(BasicTimer_Handler_t *ptrBTimerHandler);
void defineDirection (void);
void setDisplay0 (void);
void setDisplay1 (void);
void setDisplay2 (void);
void setDisplay3 (void);
void setDisplay4 (void);
void setDisplay5 (void);
void setDisplay6 (void);
void setDisplay7 (void);
void setDisplay8 (void);
void setDisplay9 (void);

void counter2Display (uint8_t digit);

//Variables
uint8_t counter = 0;
uint8_t dirCW = 0;
uint8_t dirCCW = 0;

uint8_t clkValue;
uint32_t dtValue;
//uint8_t rxData = 0;


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

		//Configurando el pin de entrada encoder CLK para la EXTI
		handlerEncoderCLK.pGPIOx = GPIOA;
		handlerEncoderCLK.GPIO_PinConfig.GPIO_PinNumber 		= PIN_8;
		handlerEncoderCLK.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
		handlerEncoderCLK.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
		handlerEncoderCLK.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;

		GPIO_Config(&handlerEncoderCLK);

		//Configurando la EXTI de encoder CLK
		handlerExtiEncoderCLK.pGPIOHandler					= &handlerEncoderCLK;
		handlerExtiEncoderCLK.edgeType						= EXTERNAL_INTERRUPT_RISING_EDGE;

		extInt_Config(&handlerExtiEncoderCLK);

		//Configurando el pin de entrada encoder DT para la EXTI
		handlerEncoderDT.pGPIOx = GPIOB;
		handlerEncoderDT.GPIO_PinConfig.GPIO_PinNumber 		= PIN_10;
		handlerEncoderDT.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
		handlerEncoderDT.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
		handlerEncoderDT.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;

		GPIO_Config(&handlerEncoderDT);

		handlerStateTimer.ptrTIMx = TIM2;
		handlerStateTimer.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
		handlerStateTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
		handlerStateTimer.TIMx_Config.TIMx_period 			= 250;
		handlerStateTimer.TIMx_Config.TIMx_interruptEnable 	= 1;

		BasicTimer_Config(&handlerStateTimer);
		startTimer(&handlerStateTimer);


		//Timer displays
		handlerDisplayTimer.ptrTIMx = TIM3;
		handlerDisplayTimer.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
		handlerDisplayTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
		handlerDisplayTimer.TIMx_Config.TIMx_period 			= 11;
		handlerDisplayTimer.TIMx_Config.TIMx_interruptEnable 	= 1;

		BasicTimer_Config(&handlerDisplayTimer);
		startTimer(&handlerDisplayTimer);

		handlerUSART2.ptrUSARTx = USART2;
		handlerUSART2.USART_Config.USART_mode 			= USART_MODE_RXTX;
		handlerUSART2.USART_Config.USART_baudrate 		= USART_BAUDRATE_115200;
		handlerUSART2.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
		handlerUSART2.USART_Config.USART_parity 		= USART_PARITY_NONE;
		handlerUSART2.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
		//handlerUSART2.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

		USART_Config(&handlerUSART2);



		//Pinout leds A-G Dsiplay

		handlerDisplayLeft.pGPIOx		= GPIOB;
		handlerDisplayLeft.GPIO_PinConfig.GPIO_PinNumber		= PIN_4;
		handlerDisplayLeft.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayLeft.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayLeft.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayLeft.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayLeft);
		GPIO_WritePin(&handlerDisplayLeft, RESET);

		handlerDisplayRight.pGPIOx		= GPIOB;
		handlerDisplayRight.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
		handlerDisplayRight.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayRight.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayRight.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayRight.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayRight);
		GPIO_WritePin(&handlerDisplayRight, SET);

		handlerDisplayA.pGPIOx			= GPIOA;
		handlerDisplayA.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
		handlerDisplayA.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayA.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayA.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayA);
		GPIO_WritePin(&handlerDisplayA, RESET);

		handlerDisplayB.pGPIOx			= GPIOH;
		handlerDisplayB.GPIO_PinConfig.GPIO_PinNumber		= PIN_1;
		handlerDisplayB.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayB.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayB.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayB);
		GPIO_WritePin(&handlerDisplayB, RESET);

		handlerDisplayC.pGPIOx			= GPIOC;
		handlerDisplayC.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
		handlerDisplayC.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayC.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayC.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayC);
		GPIO_WritePin(&handlerDisplayC, RESET);

		handlerDisplayD.pGPIOx			= GPIOC;
		handlerDisplayD.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
		handlerDisplayD.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayD.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayD.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayD);
		GPIO_WritePin(&handlerDisplayD, RESET);

		handlerDisplayE.pGPIOx			= GPIOC;
		handlerDisplayE.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
		handlerDisplayE.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayE.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayE.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayE.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayE);
		GPIO_WritePin(&handlerDisplayE, RESET);

		handlerDisplayF.pGPIOx			= GPIOC;
		handlerDisplayF.GPIO_PinConfig.GPIO_PinNumber		= PIN_1;
		handlerDisplayF.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayF.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayF.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayF.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayF);
		GPIO_WritePin(&handlerDisplayF, RESET);

		handlerDisplayG.pGPIOx			= GPIOB;
		handlerDisplayG.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
		handlerDisplayG.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerDisplayG.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerDisplayG.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerDisplayG.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerDisplayG);
		GPIO_WritePin(&handlerDisplayG, SET);







		//Config button
//		handlerUserButton.pGPIOx		= GPIOC;
//		handlerUserButton.GPIO_PinConfig.GPIO_PinNumber		= PIN_13;
//		handlerUserButton.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
//		handlerUserButton.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
//		handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
//		GPIO_Config(&handlerUserButton);
//
//		//Config exti
//		handlerExtiUsarButton.pGPIOHandler	= &handlerUserButton;
//		handlerExtiUsarButton.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
//		extInt_Config(&handlerExtiUsarButton);





		//Revisar continuamente en que sentido esta avanzando el encoder


    /* Loop forever */
	while(1){


		if(GPIO_ReadPin(&handlerDisplayLeft) == 1){
			counter2Display(counter/10);
		}
		else if(GPIO_ReadPin(&handlerDisplayRight) == 1){
			counter2Display(counter%10);
		}



	}
	return 0;
}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerStateLed);
}

void BasicTimer3_Callback(void){
	GPIOxTooglePin(&handlerDisplayLeft);
	GPIOxTooglePin(&handlerDisplayRight);
}

void callback_extInt8(void){
	defineDirection();
}

void defineDirection(void){
	if (GPIO_ReadPin(&handlerEncoderDT) == RESET) {
	counter += 1;
	} else {
	counter -= 1;
	}

}

//void callback_extInt13(void){
//	counter += 1;
//
//
//
//
//	counter2Display(counterD);
//}

void setDisplay0 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, RESET);
	GPIO_WritePin(&handlerDisplayE, RESET);
	GPIO_WritePin(&handlerDisplayF, RESET);
	GPIO_WritePin(&handlerDisplayG, SET);
}

void setDisplay1 (void){
	GPIO_WritePin(&handlerDisplayA, SET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, SET);
	GPIO_WritePin(&handlerDisplayE, SET);
	GPIO_WritePin(&handlerDisplayF, SET);
	GPIO_WritePin(&handlerDisplayG, SET);
}

void setDisplay2 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, SET);
	GPIO_WritePin(&handlerDisplayD, RESET);
	GPIO_WritePin(&handlerDisplayE, RESET);
	GPIO_WritePin(&handlerDisplayF, SET);
	GPIO_WritePin(&handlerDisplayG, RESET);
}

void setDisplay3 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, RESET);
	GPIO_WritePin(&handlerDisplayE, SET);
	GPIO_WritePin(&handlerDisplayF, SET);
	GPIO_WritePin(&handlerDisplayG, RESET);
}

void setDisplay4 (void){
	GPIO_WritePin(&handlerDisplayA, SET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, SET);
	GPIO_WritePin(&handlerDisplayE, SET);
	GPIO_WritePin(&handlerDisplayF, RESET);
	GPIO_WritePin(&handlerDisplayG, RESET);
}

void setDisplay5 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, SET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, RESET);
	GPIO_WritePin(&handlerDisplayE, SET);
	GPIO_WritePin(&handlerDisplayF, RESET);
	GPIO_WritePin(&handlerDisplayG, RESET);
}

void setDisplay6 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, SET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, RESET);
	GPIO_WritePin(&handlerDisplayE, RESET);
	GPIO_WritePin(&handlerDisplayF, RESET);
	GPIO_WritePin(&handlerDisplayG, RESET);
}

void setDisplay7 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, SET);
	GPIO_WritePin(&handlerDisplayE, SET);
	GPIO_WritePin(&handlerDisplayF, SET);
	GPIO_WritePin(&handlerDisplayG, SET);
}

void setDisplay8 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, RESET);
	GPIO_WritePin(&handlerDisplayE, RESET);
	GPIO_WritePin(&handlerDisplayF, RESET);
	GPIO_WritePin(&handlerDisplayG, RESET);
}

void setDisplay9 (void){
	GPIO_WritePin(&handlerDisplayA, RESET);
	GPIO_WritePin(&handlerDisplayB, RESET);
	GPIO_WritePin(&handlerDisplayC, RESET);
	GPIO_WritePin(&handlerDisplayD, SET);
	GPIO_WritePin(&handlerDisplayE, SET);
	GPIO_WritePin(&handlerDisplayF, RESET);
	GPIO_WritePin(&handlerDisplayG, RESET);
}

void counter2Display(uint8_t digit){
	switch (digit) {
	case 0: {
		setDisplay0();
		break;
	}
	case 1:{
		setDisplay1();
		break;
	}
	case 2:{
		setDisplay2();
		break;
	}
	case 3:{
		setDisplay3();
		break;
	}
	case 4:{
		setDisplay4();
		break;
	}
	case 5:{
		setDisplay5();
		break;
	}
	case 6:{
		setDisplay6();
		break;
	}
	case 7:{
		setDisplay7();
		break;
	}
	case 8:{
		setDisplay8();
		break;
	}
	case 9:{
		setDisplay9();
		break;
	}
	default:{
		setDisplay0();
		break;
	}
	}
}
