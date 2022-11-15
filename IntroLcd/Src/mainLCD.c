/**
 ******************************************************************************
 * @file           : main.c
 * @author         : ejguzmanc
 * @brief          : Configuracion basica del proyecto
 ******************************************************************************
 *
 *
 ******************************************************************************
 */

#include <BasicTimer.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "LcdDriver.h"


BasicTimer_Handler_t handlerStateTimer = {0};
GPIO_Handler_t  	handlerStateLed = {0};

GPIO_Handler_t  	handlerI2Cclk = {0};
GPIO_Handler_t  	handlerI2Cdata = {0};

I2C_Handler_t		handlerI2Clcd	={0};


/*CABECERAS DE FUNCIONES CREADAS*/

void InitSystem(void);

int main(void)
{
	InitSystem();

	set_cursor(&handlerI2Clcd,0,0);
	LCD_sendSTR(&handlerI2Clcd, "C.................");

	set_cursor(&handlerI2Clcd,2,0);
	LCD_sendSTR(&handlerI2Clcd, "Tres");

	set_cursor(&handlerI2Clcd,3,0);
	LCD_sendSTR(&handlerI2Clcd, "cuatro");

	delay(2000);
	lcd_clear(&handlerI2Clcd);
	delay(500);

	set_cursor(&handlerI2Clcd,2,0);
	LCD_sendSTR(&handlerI2Clcd, "Tres");


    /* Loop forever */
	while(1){
//	set_cursor(&handlerI2Clcd,0,0);
//	LCD_sendSTR(&handlerI2Clcd, "Taller V AHHHH");
//
//	set_cursor(&handlerI2Clcd,2,0);
//	LCD_sendSTR(&handlerI2Clcd, "SGjkaGKJSg");
//	}
//
//	delay(1000);
//	lcd_clear(&handlerI2Clcd);
//	delay(500);

	//return 0;
}}

void InitSystem(void){

	//Configuring State LED
	handlerStateLed.pGPIOx 									= GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerStateLed);
	GPIO_WritePin(&handlerStateLed, SET);


	//Configuring Timer2 to work with the State LED
	handlerStateTimer.ptrTIMx 								= TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period 				= 250;
	handlerStateTimer.TIMx_Config.TIMx_interruptEnable 		= 1;

	BasicTimer_Config(&handlerStateTimer);
	startTimer(&handlerStateTimer);

	//Señal clock PB6  - I2C1
	handlerI2Cclk.pGPIOx								= GPIOB;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cclk);

	//Señal data PB7  - I2C1
	handlerI2Cdata.pGPIOx								= GPIOB;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cdata);

	//handler I2C
	handlerI2Clcd.ptrI2Cx							= I2C1;
	handlerI2Clcd.slaveAddress						= 0x27; 		//Cargando dirección del esclavo
	handlerI2Clcd.modeI2C							= I2C_MODE_SM;

	i2c_config(&handlerI2Clcd);

	LCD_Init(&handlerI2Clcd);

	lcd_clear(&handlerI2Clcd);
}


void BasicTimer2_Callback (void){
	GPIOxTooglePin(&handlerStateLed);
}
