/*
 * AdcDriver.c
 *
 *  Created on: 12/10/2022
 *      Author: mmayag
 */

#include "AdcDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerAdcPin = {0};
uint16_t adcRawData = 0;

void adc_Config(ADC_Config_t *adcConfig){
	/*1.Configuramos el PinX para que cumpla la funcion de canal analogo deseado */
	configAnalogPin(adcConfig->channel);

	/*2.Activamos la señal del reloj para el periferico ADC1 (bus APB2)*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	//Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/*Comensamos la configuracion del ADC1*/
	/*3.Resolucion del ADC */
	switch(adcConfig->resolution){
	case ADC_RESOLUTION_12_BIT:
	{
		ADC1->CR1 &= ~ADC_CR1_RES_0;
		ADC1->CR1 &= ~ADC_CR1_RES_1;
		break;
	}
	case ADC_RESOLUTION_10_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 &= ~ADC_CR1_RES_1;
		break;
	}
	case ADC_RESOLUTION_8_BIT:
	{
		ADC1->CR1 &= ~ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}
	case ADC_RESOLUTION_6_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}
	default:
	{
		break;
	}
	}

	//Configuramos el modo scan como activado para varias conversiones ADC
	ADC1->CR1 |= ADC_CR1_SCAN;

	//Configuramos que se genere la interrupción después de cada conversión
	ADC1->CR2 |= ADC_CR2_EOCS;

	/*5.Configuramos la alineacion de los adatos (derecha o izquierda)*/
	if(adcConfig->dataAlignment == ADC_ALIGNMENT_RIGHT){
		//Alineamos a la derecha (esta es la forma "natural")
		ADC1->CR2 &= ~ADC_CR2_ALIGN;
	}
	else{

		//Alineacion a la izquierda (para alginos calculos matematicos)
		ADC1->CR2 |= ADC_CR2_ALIGN;

	}

	/* 6.Desactivamos e "continuos mode"*/

	ADC1->CR2 &= ~ADC_CR2_CONT;

	/*7.Aca se deberia configurar el samplin...*/
	if(adcConfig->channel < ADC_CHANNEL_9){

		ADC1->SMPR2 |= (adcConfig->samplingPeriod << (3*(adcConfig->channel)));

	}
	else{
		ADC1->SMPR1 |= (adcConfig->samplingPeriod << (3*(adcConfig->channel)));

	}

	// configuramos la cantidad de conversiones ADC que se harán

	// Se configuran para dos conersiones ADC
	ADC1->SQR1 |= ADC_SQR1_L_0;

	if(adcConfig->channel == 0){
		//Asignamos el canal de la conversion a la primera posicion en la secuencia
		ADC1->SQR3 |= (adcConfig->channel << ADC_SQR3_SQ1_Pos);
	}
	if(adcConfig->channel == 1){
		//Asignamos el canal de la conversion a la primera posicion en la secuencia
		ADC1->SQR3 |= (adcConfig->channel << ADC_SQR3_SQ2_Pos);
	}

	/*9. Configuramos el preescaler del ADC EN 2:1( EL MAS RAPIDO QUE SE PUEDE TENER */
	ADC ->CCR = 0;

	/*10.Desactivamos as interrupciones globales */
	__disable_irq();

	/*11.Activamos la interupcion devida a la finalizacion de una conversion E0C */
	ADC1->CR1 |= ADC_CR1_EOCIE;

	/*11a. Matriculamos la interrupcion en el NVIC*/
	__NVIC_EnableIRQ(ADC_IRQn);

	/*12.Activamos el modulo ADC */
	ADC1->CR2 |= ADC_CR2_ADON;

	/*13.Activamos las interupciones globales */
	__enable_irq();
}

/*
 * Esta función habilita la conversion ADC de forma continua.
 * Una vez ejecutada esta función, el sistema lanza una nueva conversion ADC cada vez que
 * termina, sin necesidad de utilizar para cada conversion el bit SWSTART del registro CR2.
 * Solo es necesario activar una sola vez dicho bit y las conversiones posteriores se lanzan
 * automaticamente.
 * */

void startContinousADC(void){

	/* Activamos el modo continuo de ADC */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* Iniciamos un ciclo de conversión ADC */
	ADC1->CR2 |= ADC_CR2_SWSTART;

}

/*
 * Esta función habilita la conversion ADC de forma single.
 */

void startSingleADC(void){
	/* Desactivamos el modo continuo de ADC */
	ADC1->CR2 &= ~ADC_CR2_CONT;

	/* Limpiamos el bit del overrun (CR1) */
	ADC1->CR1 &= ~ADC_CR1_OVRIE;

	/*Iniciamos un ciclo de conversion ADC*/
	ADC1->CR2 |= ADC_CR2_SWSTART;

}

/*Funcion que me retorna el ultimo dato adquirido por la ADC*/
uint16_t getADC(void){

	return adcRawData;
}

void adcConfigExternal(void){
	//Activamos el evento externo, flanco de bajada
	ADC1->CR2 |= ADC_CR2_EXTEN_1;

	//Evento externo seleccionado para lanzar la conversión ADC (evento en el pin 11)
	ADC1->CR2 |= (0xF << ADC_CR2_EXTSEL_Pos);
}

void adcConfigTimer(void){
	//Activamos el evento externo, flanco de bajada
	ADC1->CR2 |= ADC_CR2_EXTEN_0;

	//Evento TIM5 CC3-event seleccionado para lanzar la conversión ADC
	ADC1->CR2 |= (0x7U << ADC_CR2_EXTSEL_Pos);
}
/*Esta es la ISR de la interrupcion por conversion ADC*/
void ADC_IRQHandler(void)
{
	//Evaluamos que se dio la interrupcion por conversion ADC
	if(ADC1->SR & ADC_SR_EOC){
		//Leemos el resultado de la conversion ADC y lo cargamos en un valor axiliar
		adcRawData = ADC1->DR;

		// Hacemos el llamado a la funcion que se ejecutara en el main
		adcComplete_Callback();
	}
}

/*Funcion debil , que debe ser sobreescrita em el main */

__attribute__ ((weak)) void adcComplete_Callback(void){
	__NOP();
}

/*con esta funcion configuramos que pin deseamos que funcione como ADC*/

void configAnalogPin(uint8_t adcChannel){
	switch (adcChannel){

	case ADC_CHANNEL_0:
	{
		// Es el pin PA0
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	};
	case ADC_CHANNEL_1:
	{
		// Es el pin PA1
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;

	}
	case ADC_CHANNEL_2:
	{
		// Es el pin PA2
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}

	case ADC_CHANNEL_3:
	{
		// Es el pin PA3
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;

	}
	case ADC_CHANNEL_4:
	{
		// Es el pin PA4
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;

	}
	case ADC_CHANNEL_5:
	{
		// Es el pin PA5
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}
	case ADC_CHANNEL_6:
	{
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_6;

		break;
	}
	case ADC_CHANNEL_7:
	{
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_7;

		break;
	}
	case ADC_CHANNEL_8:
	{
		//Es el pin PB0
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
	case ADC_CHANNEL_9: {
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;

		break;
	}
	case ADC_CHANNEL_10: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;

		break;
	}
	case ADC_CHANNEL_11: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;

		break;
	}
	case ADC_CHANNEL_12: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;

		break;
	}
	case ADC_CHANNEL_13: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;

		break;
	}
	case ADC_CHANNEL_14: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}
	case ADC_CHANNEL_15: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;

		break;
	}

	default:
	{
		break;
	}
	}
	// Despues de configurar el canal adecuadamente, se define este pin como Analogo y se
	// carga la configuración con el driver del GPIOx
	handlerAdcPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	GPIO_Config(&handlerAdcPin);
}

