/*
 * CaptureFrecDriver.h
 *
 *  Created on: 17/11/2022
 *      Author: if420_01
 */

#ifndef CAPTUREFRECDRIVER_H_
#define CAPTUREFRECDRIVER_H_

#include "stm32f4xx.h"

#define CAPTURE_CHANNEL_1	0
#define CAPTURE_CHANNEL_2	1
#define CAPTURE_CHANNEL_3	2
#define CAPTURE_CHANNEL_4	3

#define CAPTURE_RISING_EDGE		0
#define CAPTURE_FALLING_EDGE	1

#define CAPTURE_PREESCALER_1_1	0
#define CAPTURE_PREESCALER_2_1	1
#define CAPTURE_PREESCALER_4_1	2
#define CAPTURE_PREESCALER_8_1	3

#define CAPTURE_TIMER_SPEED_1us		16
#define CAPTURE_TIMER_SPEED_10us	160
#define CAPTURE_TIMER_SPEED_100us	1600
#define CAPTURE_TIMER_SPEED_1ms		16000

typedef struct
{
	uint8_t		channel;		//Canal relacionado con el timer
	uint8_t		edgeSignal;		//Seleccion del tipo de flanco que se desea capturar
	uint32_t	timerSpeed;		//A que velocidad se incrementa el timer
	uint32_t	prescalerCapture;	//A que velocidad incrementa timer
}Capture_Config_t;

typedef struct
{
	TIM_TypeDef			*ptrTIMx;	//Timer al que esta asociado la captura
	Capture_Config_t	config;		//Configuracion inicial de la captura
}Capture_Handler_t;

/* Prototipos de las funciones */
void capture_Config(Capture_Handler_t *ptrCaptureHandler);
uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler);
void IRQTimer(Capture_Handler_t *ptrCaptureHandler);

#endif /* CAPTUREFRECDRIVER_H_ */
