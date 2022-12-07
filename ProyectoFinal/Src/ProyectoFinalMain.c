/**
 **************************************************************************************************
 * @file           : ProyectoFinalMain.c
 * @authors        : Juan Camilo Gomez
 * @brief          : Main que integra el proyecto final: WORDLE
 *
 **************************************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "I2CDriver.h"
#include "LcdDriver.h"
#include "ExtiDriver.h"
#include "SH1106.h"
#include "Fonts.h"
#include "Wordle.h"
#include "DBWordle.h"


// Definicion de los handlers necesarios
GPIO_Handler_t handlerStateLed = {0};
GPIO_Handler_t handlerPinTx		= {0};
GPIO_Handler_t handlerPinRx		= {0};
GPIO_Handler_t  	handlerI2Cclk = {0};
GPIO_Handler_t  	handlerI2Cdata = {0};

// Timer encargado del Blinky
BasicTimer_Handler_t handlerStateTimer 	= {0};

// Utiliza la conexion USB
USART_Handler_t handlerUsart2		= {0};

// Configuración Joystick (ADC y EXTI)
ADC_Config_t 		adcConfigX 		= {0};
ADC_Config_t 		adcConfigY 		= {0};
GPIO_Handler_t handlerJoystickButton			= {0};
EXTI_Config_t handlerExtiJoystickButton	 	= {0};
BasicTimer_Handler_t	handlerAdcTimer		= {0};

//Handlers de las pantallas
I2C_Handler_t		handlerI2Clcd	={0};
I2C_Handler_t		handlerI2COled	={0};

// Variables USART
uint8_t rxData = 0;
uint8_t counter = 0;

// Variables ADC

int adcValue[2];
uint8_t adcIsComplete = RESET;

//Variables del programa
uint8_t charData = 0;
uint8_t noValidWord = 0;
uint8_t achieved = 0;
uint8_t victories = 48;
uint8_t defeats = 48;
uint16_t totalWords = 1022;
uint16_t validPosX[] = {0x05, 0x10, 0x1B, 0x26, 0x31};
int position = 0;
char bufferData[5];
char solution[5];

char chars[] = {'A', 'B', 'C', 'D', 'E', 'F', 0x7F, 'G', 'H', 'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

char keyboardPosition[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x28, 0x29, 0x2A,
		0x2B, 0x2C, 0x2D, 0x2E, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x3C, 0x3D,
		0x3E, 0x3F, 0x40, 0x41};

/* Definición de prototipos de funciones */
void InitSystem(void);
void askIfContinue(uint8_t x, char* firstWord, char* secWord);

int main(void) {

	// Inicializamos todos los elementos del sistema
	InitSystem();

	welcome();
	delay(2000);



	LCD_Clear(&handlerI2Clcd);
	SH1106_Clear();
	SH1106_UpdateScreen(&handlerI2COled);
	SH1106_GotoXY(5, 3);

	//Asignar un valor aleatorio de la base de datos a la palabra que se desea
	strcpy(solution, DBWordle[rand()%totalWords]);
	writeMsg(&handlerUsart2, solution);

	//Inicializamos el teclado y encendemos el blinky del cursor de LCD
	initKeyboard();
	LCD_sendCMD(&handlerI2Clcd, 0x0F);

	/* Loop forever */
	while (1) {

		cursorWJoystick();

		if (charData != '\0') {

			//Preguntamos si el caracter ingresado es backspace
			if(charData == 0x7F){

				//En la posicion de la primera letra no es valido
				if(SH1106_GetX() == 5 || SH1106_GetX() == 0x48){
					charData = '\0';
				}

				//En cualquier otra posición borrar la letra inmediatamente anterior
				else{
					SH1106_GotoXY(SH1106_GetX()-11, SH1106_GetY());
					bufferData[strlen(bufferData)-1] = '\0';
					SH1106_Putc(' ', &Font_11x18);
					SH1106_UpdateScreen(&handlerI2COled);
					SH1106_GotoXY(SH1106_GetX()-11, SH1106_GetY());
					charData = '\0';
				}
			}

			//Para cualquier caracter diferente a backspace
			else{

				//Bloque de codigo que borra victorias y derrotas de Oled cuando se inicia nueva partida
				if (position == 0 && SH1106_GetX()==5){
					SH1106_Clear();
					SH1106_UpdateScreen(&handlerI2COled);
				}
				//Agregamos el caracter al buffer de la Oled y lo cargamos para mostrar en pantalla
				SH1106_Putc((char) charData, &Font_11x18);
				SH1106_UpdateScreen(&handlerI2COled);

				//Agregamos el caracter al arreglo
				if(strlen(bufferData) < 5){
				appendChar(bufferData, charData);

				//En este punto se recupera la palabra de 5 letras en bufferData
				if(strlen(bufferData) == 5){

					//Buscar si la palabra que se ingreso es una palabra valida dentro de la base de datos
					int i = 0;
					for (i=0; i<totalWords; i++){
						//Esto ocurre si es una palabra valida
						if(!isInDB(bufferData,DBWordle[i])){
							compareStr(bufferData, solution);

							//Aumenta la posicion (posicion -> 6 intentos)
							position++;

							//Si la posicion es 6 y no se logro adivinar la palabra se pierde el juego
							if(position == 6 && achieved == 0){
								defeats++;
								askIfContinue(8, "Mala suerte!", "Reintentar:");
								position = 0;
							}
							positioning();

							//Borramos el buffer utilizado, para poder guardar una nueva palabra
							memset(bufferData, 0, strlen(bufferData));

							break;
						}


					}
					//Esto ocurre si la palabra NO esta en la base de datos
					if(i>=totalWords){
							writeMsg(&handlerUsart2, "\nLa palabra NO existe");
							writeMsg(&handlerUsart2, bufferData);

							LCD_setCursor(&handlerI2Clcd, 0, 11);
							delay(2);
							LCD_sendSTR(&handlerI2Clcd, "Palabra");
							delay(2);
							LCD_setCursor(&handlerI2Clcd, 1, 11);
							delay(2);
							LCD_sendSTR(&handlerI2Clcd, "Invalida");
							delay(2);
							cursorHome(&handlerI2Clcd);
							delay(5);
							cursorBS(&handlerI2Clcd);
							delay(2);

							charData = '\0';
							while(charData != 0x7F){
								cursorWJoystick();
							}

							clearHalfLCD();
							cursorHome(&handlerI2Clcd);
							delay(5);
							cursorBS(&handlerI2Clcd);
							delay(2);


							noValidWord = 1;
					}
				}
			}

			if(noValidWord){
				noValidWord = 0;
			}
			else{
				// Bajamos la bandera
				charData = '\0';
			}
		}
	}
	}

	return 0;
}

/*
 * Funcion encargada de la inicializacion de los elementos del sistema
 */
void InitSystem(void){

/************************Configuración del led de estado*********************/
	// Configurando el pin para el Led_Blinky
	handlerStateLed.pGPIOx 								= GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	// Cargamos la configuración del Pin del led de estado
	GPIO_Config(&handlerStateLed);
	GPIO_WritePin(&handlerStateLed, RESET);

	// Configurando el Timer2 para que funcione con el blinky
	handlerStateTimer.ptrTIMx 							= TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed			= BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period			= 250;

	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerStateTimer);

	// Activamos el TIM2
	startTimer(&handlerStateTimer);

	/************************Configuración del USART*********************/
	/* Configurando los pines sobre los que funciona el USART2 (TX) */
	handlerPinTx.pGPIOx 								= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
	GPIO_Config(&handlerPinTx);

	/* Configurando los pines sobre los que funciona el USART2 (RX) */
	handlerPinRx.pGPIOx 								= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
	GPIO_Config(&handlerPinRx);

	// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart2.ptrUSARTx 							= USART2;
	handlerUsart2.USART_Config.USART_baudrate			= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity				= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX		= USART_RX_INTERRUP_ENABLE;

	// Cargamos la configuración del USART
	USART_Config(&handlerUsart2);


	/************************Configuración del dispositivo ADC (Joystick)*********************/
	// Configuramos el timer del ADC
	handlerAdcTimer.ptrTIMx								= TIM4;
	handlerAdcTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerAdcTimer.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerAdcTimer.TIMx_Config.TIMx_period 			= 150;

	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerAdcTimer);
	startTimer(&handlerAdcTimer);

	//Se establecen las configuraciones del canal 0, eje X
	adcConfigX.channel									= ADC_CHANNEL_0;
	adcConfigX.dataAlignment 							= ADC_ALIGNMENT_RIGHT;
	adcConfigX.resolution 								= ADC_RESOLUTION_12_BIT;
	adcConfigX.samplingPeriod							= ADC_SAMPLING_PERIOD_15_CYCLES;

	//Se carga la configuración del ADC
	adc_Config(&adcConfigX);

	//Se establecen las configuraciones del canal 1, eje Y
	adcConfigY.channel									= ADC_CHANNEL_1;
	adcConfigY.dataAlignment 							= ADC_ALIGNMENT_RIGHT;
	adcConfigY.resolution 								= ADC_RESOLUTION_12_BIT;
	adcConfigY.samplingPeriod							= ADC_SAMPLING_PERIOD_15_CYCLES;

	//Se carga la configuración del ADC
	adc_Config(&adcConfigY);

/************************Configuración de la EXTI para boton de Joystick*********************/
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

/************************Configuración de lineas seriales I2C*********************/
	//Señal clock PB6  - I2C1
	handlerI2Cclk.pGPIOx									= GPIOB;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerI2Cclk.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2Cclk);

	//Señal data PB7  - I2C1
	handlerI2Cdata.pGPIOx									= GPIOB;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_OPENDRAIN;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerI2Cdata.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF4;
	GPIO_Config(&handlerI2Cdata);

/************************Configuración del esclavo LCD*********************/
	handlerI2Clcd.ptrI2Cx									= I2C1;
	handlerI2Clcd.slaveAddress								= 0x27;
	handlerI2Clcd.modeI2C									= I2C_MODE_SM;

	i2c_config(&handlerI2Clcd);
	LCD_Init(&handlerI2Clcd);

/************************Configuración del esclavo Oled*********************/
	handlerI2COled.ptrI2Cx									= I2C1;
	handlerI2COled.slaveAddress								= 0x3C;
	handlerI2COled.modeI2C									= I2C_MODE_FM;

	i2c_config(&handlerI2COled);

	sh1106_Init(&handlerI2COled);
}

// ADC (conversión) Callback
void adcComplete_Callback(void){
	// Leemos los valores X e Y de la conversión ADC, al estar en modo scan llegan
	//En el orden que se especifico en el driver ADC

	if(counter == 0){
		adcValue[0] = getADC();
		counter++;
	}
	else if(counter == 1){
		adcValue[1] = getADC();
		counter = 0;
	}

	// Levantamos la bandera
	adcIsComplete = SET;
}

/* Callback del Timer4 - Hacemos una conversión ADC */
void BasicTimer4_Callback(void){
	// Comienza una convesión ADC
	startSingleADC();
}

/* Callback del Timer2 - Hacemos un blinky */
void BasicTimer2_Callback(void){
	handlerStateLed.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

/* Callback relacionado con la recepción del USART2*/
void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

void callback_extInt7(void){
	joystickButton();
}

/*
 * El programa pregunta si el jugador desea continuar, aqui se genera una nueva palabra
 * solucion en caso de adivinar la actual o si se desea reintentar en caso de perder
 * tambien es posible terminar el juego y cerrar el programa, ademas, aqui se presenta al usuario
 * por medio de la Oled la cuenta de sus aciertos y derrotas.
 */

void askIfContinue(uint8_t x, char* firstWord, char* secWord){

	//Indicamos al jugar si gano o perdio
	LCD_setCursor(&handlerI2Clcd, 0, x);
	delay(1);
	LCD_sendSTR(&handlerI2Clcd, firstWord);
	delay(1);
	LCD_setCursor(&handlerI2Clcd, 1, 8);
	delay(1);
	LCD_sendSTR(&handlerI2Clcd, secWord);
	delay(1);
	LCD_setCursor(&handlerI2Clcd, 2, 13);
	delay(1);
	LCD_sendSTR(&handlerI2Clcd, "S");
	delay(1);
	LCD_setCursor(&handlerI2Clcd, 2, 15);
	delay(1);
	LCD_sendSTR(&handlerI2Clcd, "N");
	delay(1);

	//En caso de ganar se indica la solucion y se genera una nueva para seguir jugando
	if(achieved){
		LCD_setCursor(&handlerI2Clcd, 3, 12);
		delay(1);
		LCD_sendSTR(&handlerI2Clcd, solution);
		delay(1);
		strcpy(solution, DBWordle[rand()%totalWords]);
	}

	cursorInRestart(&handlerI2Clcd);

	while(charData != 0x23 && charData != 0x25){
		cursorWJoystickRestart();
	}
	SH1106_Clear();
	SH1106_UpdateScreen(&handlerI2COled);

	//Si se escoge que se desea continuar
	if(charData == 0x23){
		clearHalfLCD();
		LCD_setCursor(&handlerI2Clcd, 3, 12);
		LCD_sendSTR(&handlerI2Clcd, "_____");
		delay(1);

		SH1106_GotoXY(0x2E, 14);
		SH1106_Puts("V: ", &Font_11x18);
		SH1106_Putc(victories, &Font_11x18);
		SH1106_GotoXY(0x2E, 39);
		SH1106_Puts("D: ", &Font_11x18);
		SH1106_Putc(defeats, &Font_11x18);
		SH1106_UpdateScreen(&handlerI2COled);
		cursorHome(&handlerI2Clcd);
		achieved = 0;
		cursorWJoystick();
	}
	//Si se desea abandonar el juego
	else if(charData == 0x25){
		LCD_Clear(&handlerI2Clcd);
		LCD_sendCMD(&handlerI2Clcd, 0x0C);
		LCD_setCursor(&handlerI2Clcd, 1, 7);
		LCD_sendSTR(&handlerI2Clcd, "ADIOS!");
		SH1106_GotoXY(10, 18);
		SH1106_Puts("WORDLE!", &Font_16x26);
		SH1106_UpdateScreen(&handlerI2COled);
		delay(1500);

		LCD_sendCMD(&handlerI2Clcd, 0x08);
		SH1106_Clear();
		SH1106_UpdateScreen(&handlerI2COled);
		exit(0);
	}
}
