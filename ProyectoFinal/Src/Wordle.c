/*
 * Wordle.c
 *
 *  Created on: 6/12/2022
 *      Author: camil
 */

#include "Wordle.h"


//Handlers de las pantallas
I2C_Handler_t		handlerI2Clcd;
I2C_Handler_t		handlerI2COled;
USART_Handler_t handlerUsart2;

uint8_t achieved;
int adcValue[2];
uint8_t charData;
char chars[27];
char keyboardPosition[27];
uint8_t victories;
uint8_t defeats;
uint16_t totalWords;
uint16_t validPosX[5];
char solution[5];
int position;

/*
 * Con esta funcion podemos recuperar un caracter a partir de la posición en el teclado
 */
uint8_t keyboardPosition2Char(void){
	uint8_t newChar;

	//Recuperamos el indice del caracter a partir de la posicion
	uint8_t i;
	for(i=0; i<27; i++){
		if(LCD_GetX() == keyboardPosition[i]){
			break;
		}
	}

	//Devolvemos el caracter a partir del abecedario
	newChar = chars[i];
	return newChar;
}

/*
 * Funcion que permite adicionar un caracter al final de un string
 */
void appendChar(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

/*
 * Funcion que es utilizada para comparar strings
 */
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

/*
 * Funcion que determina si el string solucion y el ingresado son iguales, tambien
 * busca por las letras que estan en la solucion, pero en distinta posicion
 */

void compareStr (char *buffer, char* solution){
	int i;
	int j;

	//Esto pasa si se logro adivinar la palabra ***** Agregar opcion para nuevo juego
	if(strcmp(buffer, solution) == 0){
		achieved = 1;
		victories++;

		askIfContinue(11, "Genial!", "Nuevo juego:");
		position = -1;
		positioning();
	}
	else{

		//
		for(i=0; i<5; i++){
			for(j=0; j<5; j++){

				//Esto ocurre si las letras en la iteración, coinciden
				if(buffer[i] == solution[j]){

					//Esto courre si la letra esta ademas, en la misma posicion
					//Aparece la letra en la posicion asignada en la LCD.
					if(i == j){
						LCD_setCursor(&handlerI2Clcd, 3, 12 + i);
						delay(2);
						LCD_sendata(&handlerI2Clcd, solution[i]);
						delay(2);
						cursorHome(&handlerI2Clcd);
						delay(5);
					}
					//Esto ocurre si las letras coinciden, pero en distinta posicion,
					//Se dibuja un caracter negativo en la Oled
					else{
						//Este codigo especifica buscar la posicion de la letra para la
						//columna de la izquierda en la Oled
						if(position<3){
						SH1106_GotoXY(validPosX[i], SH1106_GetY());
						SH1106_PutNegch(buffer[i], &Font_11x18);
						SH1106_UpdateScreen(&handlerI2COled);
						}
						//Aqui se especifica la columna de la derecha Oled
						else{
							SH1106_GotoXY(validPosX[i] +67, SH1106_GetY());
							SH1106_PutNegch(buffer[i], &Font_11x18);
							SH1106_UpdateScreen(&handlerI2COled);
						}
					}
				}
			}
		}
	}
}

/*
 * Funcion que permite seleccionar la posicion donde se va a escribir en la Oled
 * en funcion del intento en el cual este el jugador
 */
void positioning(void){
	//Switch para ubicar las palabras en el arreglo 3x2 de OLED
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

/*
 * Uso del valor obtenido en la ADC para mover el cursor en la LCD
 */
void cursorWJoystick(void){
	if(adcValue[0] > 4000){
		cursorShiftR(&handlerI2Clcd);
		delay(350);
	}

	else if(adcValue[0] < 50){
		cursorShiftL(&handlerI2Clcd);
		delay(350);
	}

	if(adcValue[1] > 4000){
		cursorShiftD(&handlerI2Clcd);
		delay(350);
	}

	if(adcValue[1] < 50){
		cursorShiftU(&handlerI2Clcd);
		delay(350);
	}
}


/*
 * Uso del valor obtenido en la ADC para mover el cursor en la LCD
 */
void cursorWJoystickRestart(void){
	if(adcValue[0] > 4000){
		cursorRestartShiftR(&handlerI2Clcd);
		delay(100);
	}

	else if(adcValue[0] < 50){
		cursorRestartShiftL(&handlerI2Clcd);
		delay(100);
	}
}

/*
 * Generamos el teclado en pantalla y la barra que indica las letras que el jugador ha
 * conseguido adivinar
 */
void initKeyboard(void){
	uint8_t y;
	uint8_t j;
	uint8_t c = 0;
	for(y=0; y<4; y++){

		for(j=0; j<7; j++){
			if(y==3 && j==6){break;}
			LCD_setCursor(&handlerI2Clcd, y, j);
			LCD_sendata(&handlerI2Clcd, chars[c]);
			delay(1);
			c++;
		}
	}

	LCD_setCursor(&handlerI2Clcd, 3, 12);
	LCD_sendSTR(&handlerI2Clcd, "_____");
	delay(1);

	LCD_sendCMD(&handlerI2Clcd, 0x02);
	delay(5);
}

/*
 * Mensajes de bienvenida en las dos pantallas
 */
void welcome(void){
	LCD_setCursor(&handlerI2Clcd, 1, 4);
	LCD_sendSTR(&handlerI2Clcd, "Bienvenid@!");
	SH1106_GotoXY(41, 25);
	SH1106_Puts("WORDLE!", &Font_7x10);
	SH1106_UpdateScreen(&handlerI2COled);
	delay(500);
	SH1106_GotoXY(29, 21);
	SH1106_Puts("WORDLE!", &Font_11x18);
	SH1106_UpdateScreen(&handlerI2COled);
	delay(500);
	SH1106_GotoXY(10, 18);
	SH1106_Puts("WORDLE!", &Font_16x26);
	SH1106_UpdateScreen(&handlerI2COled);
}

/*
 * Funcion que limpia algunos mensajes de la LCD
 */
void clearHalfLCD (void){
	LCD_setCursor(&handlerI2Clcd, 0, 8);
	delay(2);
	LCD_sendSTR(&handlerI2Clcd, "            ");
	delay(2);
	LCD_setCursor(&handlerI2Clcd, 1, 8);
	delay(2);
	LCD_sendSTR(&handlerI2Clcd, "            ");
	delay(2);
	LCD_setCursor(&handlerI2Clcd, 2, 10);
	delay(2);
	LCD_sendSTR(&handlerI2Clcd, "         ");
	delay(2);
}

/*
 * Funcion encargada de asignar valores a chardata cuando se presiona el boton
 */
void joystickButton(void){
	//Si se esta decidiendo continuar o no
	if(achieved || position == 6){
		charData = LCD_GetX();
	}
	//Durante el juego
	else{
		charData = keyboardPosition2Char();
	}
}
