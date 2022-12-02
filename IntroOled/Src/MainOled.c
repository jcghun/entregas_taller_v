/**
 ******************************************************************************
 * @file           : MainOled.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Main program body
 ******************************************************************************
 */

#include <BasicTimer.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "SH1106.h"
#include "Fonts.h"

BasicTimer_Handler_t handlerStateTimer = {0};
GPIO_Handler_t  	handlerStateLed = {0};

GPIO_Handler_t  	handlerI2Cclk = {0};
GPIO_Handler_t  	handlerI2Cdata = {0};

I2C_Handler_t		handlerI2Clcd	={0};

void InitSystem(void);
void initOled(void);

int main(void)
{
	InitSystem();
//	uint8_t i=0;
//	for(i=0; i<131;i++){
//		SH1106_DrawPixel(i,22);
		//SH1106_DrawPixel(0,i);
//	}

	SH1106_DrawPixel(0,0, 1);
//	SH1106_DrawPixel(2,0, 1);
//	SH1106_DrawPixel(6,0, 1);
//	SH1106_DrawPixel(6,16, 1);

	SH1106_GotoXY(2, 0);
	SH1106_Putc('A', &Font_7x10);

//	SH1106_GotoXY(2, 0);
//	SH1106_Puts("PELAR", &Font_7x10);
//	SH1106_GotoXY(2, 10);
//	SH1106_Puts("PELAR", &Font_7x10);
//	SH1106_GotoXY(2, 20);
//	SH1106_Puts("PELAR", &Font_7x10);
//	SH1106_GotoXY(2, 30);
//	SH1106_Puts("PELAR", &Font_7x10);
//	SH1106_GotoXY(2, 40);
//	SH1106_Puts("PELAR", &Font_7x10);
//	SH1106_GotoXY(2, 50);
//	SH1106_Puts("PELAR", &Font_7x10);


	SH1106_UpdateScreen(&handlerI2Clcd);
	//delay(1000);

	while(1){

		//i2c_writeSingleRegister(&handlerI2COled,0x40,0xAE);


	}
	return 0;
}


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
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cclk);

	//Señal data PB7  - I2C1
	handlerI2Cdata.pGPIOx								= GPIOB;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2Cdata);

	//handler I2C
	handlerI2Clcd.ptrI2Cx							= I2C1;
	handlerI2Clcd.slaveAddress						= 0x3C; 		//Cargando dirección del esclavo
	handlerI2Clcd.modeI2C							= I2C_MODE_FM;

	i2c_config(&handlerI2Clcd);

	sh1106_Init(&handlerI2Clcd);

	//lcd_clear(&handlerI2COled);
}

void BasicTimer2_Callback (void){

	GPIOxTooglePin(&handlerStateLed);

}
