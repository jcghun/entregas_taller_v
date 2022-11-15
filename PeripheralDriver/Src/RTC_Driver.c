/*
 * RTCDriver.c
 *
 *  Created on: 6/11/2022
 *      Author: Juan Camilo Gomez
 */
#include "RTC_Driver.h"


void rtc_Config(RTC_Handler_t *ptrRtcHandler){

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	// Access to RTC, RTC Backup and RCC CSR registers enabled
	PWR->CR |= PWR_CR_DBP;
	// RTC clock enable
	RCC->BDCR |= RCC_BDCR_RTCEN;
	// External low-speed oscillator enable
	RCC->BDCR |= RCC_BDCR_LSEON;
	// LSE oscillator clock used as the RTC clock
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;

	//Se espera hasta que el LSE este listo
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP();
	}

	// Disable the write protection for RTC registers
	RTC->WPR |= 0xCA;
	RTC->WPR |= 0x53;

	// Set the initialization mode
	RTC->ISR |= RTC_ISR_INIT;

	//Wait for calendar registers update is allowed.
	while(!(RTC->ISR & RTC_ISR_INITF)){
		__NOP();
	}

	// Configure the RTC prescaler
	RTC->PRER = 0x007f00ff; // Asynch = 128, Synch = 256

	//Calendar values are taken directly from the calendar counters.
	RTC->CR |= RTC_CR_BYPSHAD;

	//Limpiando time y data register
	RTC->TR = 0;
	RTC->DR = 0;

	//Config AM/PM hour format
	RTC->CR |= RTC_CR_FMT;

	RTC->TR |= ptrRtcHandler->RTC_Hours/10 << RTC_TR_HT_Pos;
	RTC->TR |= ptrRtcHandler->RTC_Hours%10 << RTC_TR_HU_Pos;
	RTC->TR |= ptrRtcHandler->RTC_Minutes/10 << RTC_TR_MNT_Pos;
	RTC->TR |= ptrRtcHandler->RTC_Minutes%10 << RTC_TR_MNU_Pos;
	RTC->TR |= ptrRtcHandler->RTC_Seconds/10  << RTC_TR_ST_Pos;
	RTC->TR |= ptrRtcHandler->RTC_Seconds%10 << RTC_TR_SU_Pos;
	RTC->TR |= ptrRtcHandler->RTC_AmPm << RTC_TR_PM_Pos;

	RTC->DR |= ptrRtcHandler->RTC_Days/10 << RTC_DR_DT_Pos;
	RTC->DR |= ptrRtcHandler->RTC_Days%10 << RTC_DR_DU_Pos;
	RTC->DR |= ptrRtcHandler->RTC_Months%10 << RTC_DR_MU_Pos;
	RTC->DR |= ptrRtcHandler->RTC_Months/10 << RTC_DR_MT_Pos;
	RTC->DR |= ptrRtcHandler->RTC_Wdu<< RTC_DR_WDU_Pos;
	RTC->DR |= ((ptrRtcHandler->RTC_Years -2000)%10) << RTC_DR_YU_Pos;
	RTC->DR |= (ptrRtcHandler->RTC_Years - 2000)/10 << RTC_DR_YT_Pos;

	
	// Exit the RTC Initialization mode
	RTC->ISR &= ~RTC_ISR_INIT;
	// Access to RTC, RTC Backup and RCC CSR registers disabled
	PWR->CR &= ~ PWR_CR_DBP;
	// Enable the write protection for RTC registers
	RTC->WPR = (0xFF);

}

uint8_t tiempo[6]= {0};
uint8_t fecha[7] = {0};

uint8_t* read_time(void){

	uint8_t RTC_HoursUnits	 = 0;
	uint8_t RTC_HoursTens	 = 0;
	uint8_t RTC_MinutesUnits = 0;
	uint8_t RTC_MinutesTens = 0;
	uint8_t RTC_SecondsUnits = 0;
	uint8_t RTC_SecondsTens = 0;
	uint8_t RTC_AmPm = 0;


	RTC_HoursUnits = ((RTC->TR & RTC_TR_HU_Msk)>>RTC_TR_HU_Pos);
	RTC_HoursTens = ((RTC->TR & RTC_TR_HT_Msk)>> RTC_TR_HT_Pos);
	RTC_MinutesUnits = ((RTC->TR & RTC_TR_MNU_Msk)>>RTC_TR_MNU_Pos);
	RTC_MinutesTens = ((RTC->TR & RTC_TR_MNT_Msk)>>RTC_TR_MNT_Pos);
	RTC_SecondsUnits = ((RTC->TR & RTC_TR_SU_Msk)>>RTC_TR_SU_Pos);
	RTC_SecondsTens = ((RTC->TR & RTC_TR_ST_Msk)>>RTC_TR_ST_Pos);
	RTC_AmPm = ((RTC->TR & RTC_TR_PM_Msk)>>RTC_TR_PM_Pos);

	tiempo[0] = RTC_SecondsUnits;
	tiempo[1] = RTC_SecondsTens;
	tiempo[2] = RTC_MinutesUnits;
	tiempo[3] = RTC_MinutesTens;
	tiempo[4] = RTC_HoursUnits;
	tiempo[5] = RTC_HoursTens;
	tiempo[6] = RTC_AmPm;

	return tiempo;
}
uint8_t* read_date(void){

	uint8_t RTC_dayUnits = 0;
	uint8_t RTC_dayTens	 = 0;
	uint8_t RTC_month = 0;
	uint8_t RTC_yearUnits = 0;
	uint8_t RTC_yearTens = 0;
	uint8_t RTC_wdu = 0;

	RTC_dayTens=((RTC->DR & RTC_DR_DT_Msk)>> RTC_DR_DT_Pos);
	RTC_dayUnits=((RTC->DR & RTC_DR_DU_Msk)>>RTC_DR_DU_Pos);
	RTC_yearUnits=((RTC->DR & RTC_DR_YU_Msk)>>RTC_DR_YU_Pos);
	RTC_yearTens = ((RTC->DR & RTC_DR_YT_Msk)>>RTC_DR_YT_Pos);
	RTC_month = (((RTC->DR & RTC_DR_MT_Msk)>>RTC_DR_MT_Pos)*10) + (((RTC->DR & RTC_DR_MU_Msk)>>RTC_DR_MU_Pos)%10);
	RTC_wdu = (RTC->DR & RTC_DR_WDU_Msk)>>RTC_DR_WDU_Pos;

	fecha[0] = RTC_dayUnits;
	fecha[1] = RTC_dayTens;
	fecha[2] = RTC_month;
	fecha[3] = RTC_yearUnits;
	fecha[4] = RTC_yearTens;
	fecha[5] = RTC_wdu;

	return fecha;
}

