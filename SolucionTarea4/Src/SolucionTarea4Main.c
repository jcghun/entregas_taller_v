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

//Timers Definition
BasicTimer_Handler_t handlerStateTimer		= {0};
BasicTimer_Handler_t handlerDisplayTimer	= {0};

//GPIO handlers definition
GPIO_Handler_t handlerStateLed				= {0};
GPIO_Handler_t handlerEncoderCLK			= {0};
GPIO_Handler_t handlerEncoderDT				= {0};
GPIO_Handler_t handlerPinTx					= {0};
GPIO_Handler_t handlerPinRx					= {0};
GPIO_Handler_t handlerJoystickButton			= {0};

//7 segment handlers LEDs
GPIO_Handler_t handlerDisplayRight			= {0};
GPIO_Handler_t handlerDisplayLeft			= {0};

GPIO_Handler_t handlerDisplayA				= {0};
GPIO_Handler_t handlerDisplayB				= {0};
GPIO_Handler_t handlerDisplayC				= {0};
GPIO_Handler_t handlerDisplayD				= {0};
GPIO_Handler_t handlerDisplayE				= {0};
GPIO_Handler_t handlerDisplayF				= {0};
GPIO_Handler_t handlerDisplayG				= {0};

//USART connection handler
USART_Handler_t handlerUSART2				= {0};

//Necessary EXTI handlers
EXTI_Config_t handlerExtiJoystickButton	 	= {0};
EXTI_Config_t handlerExtiEncoderCLK			= {0};

/* Definition of function prototypes */
void configSystem(void);
void startTimer(BasicTimer_Handler_t *ptrBTimerHandler);
void defineDisplayAndUsart (void);
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
uint8_t clkValue = 0;
uint8_t dtValue = 0;

char bufferCounter[64];


int main(void){
	//Setting up the initial configuration for GPIOs, Timers, USART and EXTIs
	configSystem();

    // Loop forever
	while(1){

		//Constantly ask which one of the digits in display is on, to decide the number to show
		//E.g. if counter is 27 display left shows 2, right shows 7 ===> [2|7]
		if(GPIO_ReadPin(&handlerDisplayLeft) == 1){
			counter2Display(counter%10);

		}
		else if(GPIO_ReadPin(&handlerDisplayRight) == 1){
			counter2Display(counter/10);

		}
	}
	return 0;
}

//Function in charge of the initialization of the whole system
void configSystem(void){

	//Configuring State LED
	handlerStateLed.pGPIOx 									= GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerStateLed);
	GPIO_WritePin(&handlerStateLed, SET);

	//Configuring the encoder's CLK input pin for the EXTI
	handlerEncoderCLK.pGPIOx 								= GPIOA;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinNumber 		= PIN_8;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_IN;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLUP;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerEncoderCLK);

	//Setting up the encoder's CLK EXTI
	handlerExtiEncoderCLK.pGPIOHandler						= &handlerEncoderCLK;
	handlerExtiEncoderCLK.edgeType							= EXTERNAL_INTERRUPT_FALLING_EDGE;

	extInt_Config(&handlerExtiEncoderCLK);

	//Config the pin for the encoder's button
	handlerJoystickButton.pGPIOx		= GPIOA;
	handlerJoystickButton.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerJoystickButton.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerJoystickButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerJoystickButton);

	//Setting up the encoder's button EXTI
	handlerExtiJoystickButton.pGPIOHandler					= &handlerJoystickButton;
	handlerExtiJoystickButton.edgeType						= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerExtiJoystickButton);

	//Configuring the encode's DT input pin
	handlerEncoderDT.pGPIOx 								= GPIOB;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
//	handlerEncoderDT.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerEncoderDT);

	//Configuring Timer2 to work with the State LED
	handlerStateTimer.ptrTIMx 								= TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period 				= 250;
	handlerStateTimer.TIMx_Config.TIMx_interruptEnable 		= 1;

	BasicTimer_Config(&handlerStateTimer);
	startTimer(&handlerStateTimer);


	//Configuring Timer3 to work with the 2 digits display
	handlerDisplayTimer.ptrTIMx 							= TIM3;
	handlerDisplayTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerDisplayTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerDisplayTimer.TIMx_Config.TIMx_period 			= 11;
	handlerDisplayTimer.TIMx_Config.TIMx_interruptEnable 	= 1;

	BasicTimer_Config(&handlerDisplayTimer);
	startTimer(&handlerDisplayTimer);


	// Configuring the pins for the USART2 (TX)
	handlerPinTx.pGPIOx 									= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTx);

	// Configuring the pins for the USART2 (RX)
	handlerPinRx.pGPIOx 									= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRx);

	//Setting up serial communication
	handlerUSART2.ptrUSARTx 								= USART2;
	handlerUSART2.USART_Config.USART_mode 					= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate 				= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 				= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 				= USART_PARITY_NONE;
	handlerUSART2.USART_Config.USART_stopbits 				= USART_STOPBIT_1;

	USART_Config(&handlerUSART2);



	//Configuring the pins to energize anodes and decide which one of the digit
	//to show (via transistor)
	handlerDisplayLeft.pGPIOx								= GPIOA;
	handlerDisplayLeft.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerDisplayLeft.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayLeft.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerDisplayLeft.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerDisplayLeft.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayLeft);
	GPIO_WritePin(&handlerDisplayLeft, RESET);

	handlerDisplayRight.pGPIOx								= GPIOC;
	handlerDisplayRight.GPIO_PinConfig.GPIO_PinNumber		= PIN_4;
	handlerDisplayRight.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handlerDisplayRight.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerDisplayRight.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerDisplayRight.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayRight);
	GPIO_WritePin(&handlerDisplayRight, SET);

	//Pinout leds A-G Dsiplay
	handlerDisplayA.pGPIOx									= GPIOB;
	handlerDisplayA.GPIO_PinConfig.GPIO_PinNumber			= PIN_15;
	handlerDisplayA.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayA.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerDisplayA.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerDisplayA.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayA);
	GPIO_WritePin(&handlerDisplayA, RESET);

	handlerDisplayB.pGPIOx									= GPIOB;
	handlerDisplayB.GPIO_PinConfig.GPIO_PinNumber			= PIN_1;
	handlerDisplayB.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayB.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerDisplayB.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerDisplayB.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayB);
	GPIO_WritePin(&handlerDisplayB, RESET);

	handlerDisplayC.pGPIOx									= GPIOB;
	handlerDisplayC.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerDisplayC.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayC.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerDisplayC.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerDisplayC.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayC);
	GPIO_WritePin(&handlerDisplayC, RESET);

	handlerDisplayD.pGPIOx									= GPIOB;
	handlerDisplayD.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerDisplayD.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayD.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerDisplayD.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerDisplayD.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayD);
	GPIO_WritePin(&handlerDisplayD, RESET);

	handlerDisplayE.pGPIOx									= GPIOB;
	handlerDisplayE.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerDisplayE.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayE.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerDisplayE.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerDisplayE.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayE);
	GPIO_WritePin(&handlerDisplayE, RESET);

	handlerDisplayF.pGPIOx									= GPIOB;
	handlerDisplayF.GPIO_PinConfig.GPIO_PinNumber			= PIN_14;
	handlerDisplayF.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayF.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerDisplayF.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerDisplayF.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayF);
	GPIO_WritePin(&handlerDisplayF, RESET);

	handlerDisplayG.pGPIOx									= GPIOB;
	handlerDisplayG.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerDisplayG.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerDisplayG.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerDisplayG.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerDisplayG.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDisplayG);
	GPIO_WritePin(&handlerDisplayG, SET);
}

//Callback used for blink the StateLed
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerStateLed);
}

//Callback used for ultra-fast blink both digits in display
void BasicTimer3_Callback(void){
	GPIOxTooglePin(&handlerDisplayLeft);
	GPIOxTooglePin(&handlerDisplayRight);
	// La actualización desordenada de los displays hace que se presente este efecto del "fantasma"
}

//Callback used to attend the interruption of encoder's CLK
void callback_extInt8(void){
	defineDisplayAndUsart();

}

//Callbck used to attend the interruption of encoder's button
void callback_extInt7(void){
	writeMsg(&handlerUSART2, "\n\"software gets slower faster than hardware gets faster\"\n");
	// Esto no se debe hacer aca, ya que se demora mucho.
}

//Function used to define the range of the counter, if it increases or decreases
//according to the value of the DT pin of the encoder and to send via serial communication
//if the steps are CW or CCW.
void defineDisplayAndUsart(void){

	clkValue= GPIO_ReadPin(&handlerEncoderCLK);
	dtValue = GPIO_ReadPin(&handlerEncoderDT);

	if (!clkValue && dtValue){
		if (counter < 50){
			counter += 1;
			writeMsg(&handlerUSART2, "\nencoder CW, pasos = ");
			sprintf(bufferCounter, "%u\n", counter);
			writeMsg(&handlerUSART2, bufferCounter);
		}}
	else if (!clkValue && !dtValue){
		if(counter > 0){
			counter -= 1;
			writeMsg(&handlerUSART2, "\nencoder CCW, pasos = ");
			sprintf(bufferCounter, "%u\n", counter);
			writeMsg(&handlerUSART2, bufferCounter);
	}}

}

//Functions in charge of configuring the cathode pins for each of the seven-segment
//display LEDs [0-9]
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

//Function used to select one of the previous settings based on the digit provided
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
