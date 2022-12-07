/**
 ******************************************************************************
 * @file           : OledUsartMain.c
 * @author         : Juan Camilo Gomez Hidalgo
 * @brief          : Main program body
 ******************************************************************************
 */


#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#include <BasicTimer.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "I2CDriver.h"
#include "SH1106.h"
#include "Fonts.h"
#include "USARTxDriver.h"
#include "DBWordle.h"

BasicTimer_Handler_t handlerStateTimer = {0};
GPIO_Handler_t  	handlerStateLed = {0};

GPIO_Handler_t  	handlerI2Cclk = {0};
GPIO_Handler_t  	handlerI2Cdata = {0};

I2C_Handler_t		handlerI2Clcd	={0};

GPIO_Handler_t handlerPinTx		= {0};
GPIO_Handler_t handlerPinRx		= {0};
USART_Handler_t handlerUsart2 = {0};



uint8_t rxData = 0;
uint8_t noValidWord = 0;
uint8_t achieved = 0;
uint16_t validPosX[] = {0x05, 0x10, 0x1B, 0x26, 0x31};
uint8_t position = 0;
char bufferData[5];
char solution[5];



void InitSystem(void);
uint8_t isInArr(uint8_t x, uint16_t *arr, uint8_t arrLen);
void appendChar(char* s, char c);
uint8_t isInDB(char *s, char *DB);
void compareStr (char *buffer, char* solution);

void positioning(void);
void shuffle(char **array, size_t n);

int main(void) {


	InitSystem();
	SH1106_GotoXY(5, 3);
	strcpy(solution, DBWordle[rand()%7872]);
	writeMsg(&handlerUsart2, solution);


	while (1) {
		if (rxData != '\0') {

//			writeChar(&handlerUsart2, rxData);

			//Preguntamos si el caracter ingresado es backspace
			if(rxData == 0x08){
				if(SH1106_GetX() == 5 || SH1106_GetX() == 0x48){

					//En la posicion de la primera letra, solo levantar la bandera
					rxData = '\0';
				}
				else{

					//En cualquier otra posición borrar la letra inmediatamente anterior
					SH1106_GotoXY(SH1106_GetX()-11, SH1106_GetY());
					bufferData[strlen(bufferData)-1] = '\0';
					SH1106_Putc(' ', &Font_11x18);
					SH1106_UpdateScreen(&handlerI2Clcd);
					SH1106_GotoXY(SH1106_GetX()-11, SH1106_GetY());
					rxData = '\0';
				}
			}
			else if (achieved || position == 6) {
				rxData = '\0';
			}


			//Para cualquier caracter diferente a backspace
			else{
			//Agregamos el caracter al buffer de la Oled y lo cargamos para mostrar en pantalla
			SH1106_Putc((char) rxData, &Font_11x18);
			SH1106_UpdateScreen(&handlerI2Clcd);

			//Agregamos el caracter al arreglo
			if(strlen(bufferData) < 5){

				appendChar(bufferData, rxData);

				//En este punto se recupera la palabra de 5 letras en bufferData
				if(strlen(bufferData) == 5){

					//Si la palabra no ha sido adivinada
					while(achieved){
						__NOP();
					}
					//Buscar si la palabra que se ingreso es una palabra valida dentro de la base de datos
					int i = 0;
					for (i=0; i<7872; i++){
						//Esto ocurre si es una palabra valida
						if(!isInDB(bufferData,DBWordle[i])){
							compareStr(bufferData, solution);

							//Cambia la posicion
							position++;

							//Si la posicion es 6 y no se logro adivinar la palabra se pierde el juego
							if(position == 6 && achieved == 0){

							}
							positioning();

							//Borramos el buffer utilizado, para poder guardar una nueva palabra
							memset(bufferData, 0, strlen(bufferData));

							break;
						}


					}
					//Esto ocurre si la palabra NO esta en la base de datos
					if(i>=7872){
							writeMsg(&handlerUsart2, "\nLa palabra NO existe");
							writeMsg(&handlerUsart2, bufferData);
							rxData = '\0';
							while(rxData != 0x08){
								__NOP();
							}
							noValidWord = 1;
					}
				}
			}

			if(noValidWord){
				noValidWord = 0;
			}
			else{
				// Bajamos la bandera
				rxData = '\0';
			}
		}
	}

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
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_OPENDRAIN;
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


	/* Configurando los pines sobre los que funciona el USART2 (TX) */
	handlerPinTx.pGPIOx 							= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinTx);

	/* Configurando los pines sobre los que funciona el USART2 (RX) */
	handlerPinRx.pGPIOx 							= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinRx);

	// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart2.ptrUSARTx 						= USART2;
	handlerUsart2.USART_Config.USART_baudrate		= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;

	// Cargamos la configuración del USART
	USART_Config(&handlerUsart2);

	i2c_config(&handlerI2Clcd);

	sh1106_Init(&handlerI2Clcd);

}

void BasicTimer2_Callback (void){

	GPIOxTooglePin(&handlerStateLed);

}

void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}


uint8_t isInArr(uint8_t x, uint16_t *arr, uint8_t arrLen){
    int isElementPresent = 0;

    for (int i = 0; i < arrLen; i++) {
        if (arr[i] == x) {
            isElementPresent = 1;
            break;
        }
    }
    return isElementPresent;
}

void appendChar(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

uint8_t isInDB(char *s, char *DB){
	int isElementPresent = 1;

        if(!strcmp(DB, s))
        {
            isElementPresent = 0;
        }
        return isElementPresent;
    }

void shuffle(char **array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          char* t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void compareStr (char *buffer, char* solution){
	int i;
	int j;

	//Esto pasa si se logro adivinar la palabra ***** Agregar opcion para nuevo juego
	if(strcmp(buffer, solution) == 0){
		achieved = 1;
		writeMsg(&handlerUsart2, "LO LOGRASTE!");
	}
	else{
	for(i=0; i<5; i++){
		for(j=0; j<5; j++){

			//Esto ocurre si las letras en la iteración, coinciden
			if(buffer[i] == solution[j]){

				//Esto courre si las letras estan ademas, en la misma posicion
				if(i == j){

//					SH1106_Putc(solution[j], &Font_11x18);
//					SH1106_UpdateScreen(&handlerI2COled);
				}
				//Esto ocurre si las letras coinciden, pero en distinta posicion
				else{
					if(position<3){
					SH1106_GotoXY(validPosX[i], SH1106_GetY());
					SH1106_PutNegch(buffer[i], &Font_11x18);
					SH1106_UpdateScreen(&handlerI2Clcd);

//					//Para tener en cuenta o no, letras repetidas en la solucion
//					i = j = 5;
//					break;

					}
					else{

						SH1106_GotoXY(validPosX[i] +67, SH1106_GetY());
//						i -= 67;
						SH1106_PutNegch(buffer[i], &Font_11x18);
						SH1106_UpdateScreen(&handlerI2Clcd);

					}
				}
			}
		}
	}
	}
}


void positioning(void){
	//Ciclo para ubicar las palabras en el arreglo 3x2 de OLED
	switch(position){
	case 0:{
		SH1106_GotoXY(5, 3);
		break;
	}
	case 1:{
		SH1106_GotoXY(5, 24);
		break;
	}
	case 2:{
		SH1106_GotoXY(5, 44);
		break;
	}
	case 3:{
		SH1106_GotoXY(0x48, 3);
		break;
	}
	case 4:{
		SH1106_GotoXY(0x48, 24);
		break;
	}
	case 5:{
		SH1106_GotoXY(0x48, 44);
		break;
	}
	}
}
