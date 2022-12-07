/*
 * Wordle.h
 *
 *  Created on: 6/12/2022
 *      Author: camil
 */

#ifndef WORDLE_H_
#define WORDLE_H_

#include <stdlib.h>
#include "stm32f4xx.h"
#include "Fonts.h"
#include "AdcDriver.h"
#include "I2CDriver.h"
#include "LcdDriver.h"
#include "GPIOxDriver.h"
#include "SH1106.h"
#include "USARTxDriver.h"
//#include "DBWordle.h"



uint8_t keyboardPosition2Char(void);
void appendChar(char* s, char c);
uint8_t isInDB(char *s, char *DB);
void compareStr (char *buffer, char* solution);
void positioning(void);
void shuffle(char **array, size_t n);
void cursorWJoystick(void);
void cursorWJoystickRestart(void);
void initKeyboard(void);
void askIfContinue(uint8_t x, char* firstWord, char* secWord);
void welcome(void);
void clearHalfLCD (void);
void joystickButton(void);

#endif /* WORDLE_H_ */
