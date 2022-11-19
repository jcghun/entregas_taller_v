/*
 * CaptureFrecDriver.c
 *
 *  Created on: 17/11/2022
 *      Author: if420_01
 */

#include "CaptureFrecDriver.h"

void capture_Config(Capture_Handler_t *ptrCaptureHandler){

	/* 1. Activar la señal de reloj del periferico requerido*/
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 2q. Estamos en UP mode, el limite se carga en ARR y se comienza en 0 */
	ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Ademas, en el mismo "case podemos configurar el modo del pwm
	 *
	 * 5.
	 */
	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC1S;
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC1S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC1F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC1PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC1PSC_Pos);

		// Configuranmos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			//Configuracion 00, ara rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}
		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;

		break;

	}

	case CAPTURE_CHANNEL_2:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC2S;
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC2S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC2F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC2PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC2PSC_Pos);

		// Configuranmos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			//Configuracion 00, ara rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}
		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;

		break;

	}

	case CAPTURE_CHANNEL_3:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR2_CC3S;
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR2_CC3S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR2_IC3F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR2_IC3PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos);

		// Configuranmos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			//Configuracion 00, ara rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}
		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		break;

	}

	case CAPTURE_CHANNEL_4:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR2_CC4S;
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR2_CC4S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR2_IC4F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR2_IC4PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC4PSC_Pos);

		// Configuranmos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			//Configuracion 00, ara rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}
		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		break;
	}

	default:{
		__NOP();
		break;
	}
	}//fin del switch

	// Configuramos el prescaler del timer, el cual define a que velocidad se incrementa nuestro timer
	ptrCaptureHandler->ptrTIMx->PSC = ptrCaptureHandler->config.timerSpeed;
}

/* Esta funcion se encarga de lanzar la captura de la frecuencia... en este caso funciona
 * con pulling, examinando cuando se levanta la bandera del evento de captura
 */
uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler){

	uint32_t timestamp = 0;
	uint32_t timestamp2 = 0;
	uint32_t deltaTimestamp = 0;

	// reiniciamos el contador del timer
	ptrCaptureHandler->ptrTIMx->CNT = 0;

	/* Todo lo que sigue de aca en adelante depende del canal que se ha seleccionado,
	 * por lo cual se escribe cada codigo dentro del case especifico.
	 * antes de lanzar la captura (encender el Timer para que cuenta),
	 * debemos borrar el valor de los registros CCP, de forma que arranquen limpios
	 */
	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1: {
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR1 = 0;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)){}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp = ptrCaptureHandler->ptrTIMx->CCR1;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;

		//Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)){}

		// Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR1;

		deltaTimestamp = timestamp2 - timestamp;

		break;
	}
	case CAPTURE_CHANNEL_2: {
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR2 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;


		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)){}

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp = ptrCaptureHandler->ptrTIMx->CCR2;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;


		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)){}


		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR2;

		deltaTimestamp = timestamp2 - timestamp;

		break;
	}

	case CAPTURE_CHANNEL_3: {
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR3 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;


		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)){}

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp = ptrCaptureHandler->ptrTIMx->CCR3;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;


		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)){}


		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR3;

		deltaTimestamp = timestamp2 - timestamp;

		break;
	}

	case CAPTURE_CHANNEL_4: {
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR4 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;


		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)){}

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp = ptrCaptureHandler->ptrTIMx->CCR4;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;


		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)){}


		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR4;

		deltaTimestamp = timestamp2 - timestamp;

		break;
	}
	default: {
		break;
	}
	}
	return deltaTimestamp;
}
