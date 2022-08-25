/******************************************************************
 * stm32f411xx_hal.h
 *
 *  Created on: Aug 25, 2022
 *      Author: Juan Camilo
 *
 *Este archivo contiene la informacion mas basica del micro
 * - Valores del reloj principal
 * - Distribucion basica de la memoria (Descrito en la fig. 14 de la hoja de datos del micro)
 * - Porsiciones de memoria de los perifericos disponibles en el micro descrito en la tabla 1 (Memory Map)
 * - Incluye los demas registros de los perifericos
 * - definiciones de las constantes mas básicas.
 *
 * NOTA: La definicion del NVIC será realizada al momento de describir el uso de las interrupciones
 *
 *
 ***************************************************************/

#ifndef INC_STM32F411XX_HAL_H_
#define INC_STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED		16000000		//Value for the main clock signal (HSI -> High Speed Internal)
#define HSE_CLOCK_SPEED		4000000			//Value for the main clock signal (HSE -> High Speed External)

#define NOP()		asm("NOP")
#define __weak		__attribute__((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * (Remember, 1KByte = 1024 bytes
 */
#define FLASH_BASE_ADDR			0x08000000U		//Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR			0x20000000U		//Esta es la memoria RAM, 128KB.

/* NOTA: Observar que existen unos registros especificos del Cortex M4 en la región 0xE0000000U
 * Los controladores de las interrupciones se encuentran alli, por ejemplo.
 * Esto se vera a su debido tiempo.
 */

/*
 * NOTA:
 * Ahora agregamos la dirección de memoria base para cada uno de los perifericos que posee el micro
 * En el "datasheet" del micro, Figura 14 (Memory Map) encontramos el mapa de los buses:
 *  - APB1 (Advance Peripheral Bus)
 *  - APB2
 *  - AHB1 (Advance High-performance Bus)
 *  - AHB2
 */

/**
 * AHBx and APBx Bus Peripherals base addresses
 */
#define APB1_BASE_ADDR			0x40000000U
#define APB2_BASE_ADDR			0x40010000U
#define AHB1_BASE_ADDR			0x40020000U
#define AHB2_BASE_ADDR			0x50000000U

/**
 * Y ahora debemos hacer lo mismo pero cada una de las posiciones de memoria de cada uno de los
 * perifericos descritos en la Tabla 1 del manual de referencia del micro.
 * Observe que en dicha tabla está a su vez dividida en cuatro segmentos, cada uno correspondiente
 * a APB1, APB2, AHB1, AHB2.
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla. Inicia USB_OTG_FS (AHB2)
 *
 * NOTA: Solo lo vamos a hacer con los elementos que necesitamos en este ejercicio: RCC y GPIOx
 */

/* Posiciones de memoria para perifericos del AHB2 */
#define USB_OTG_FS_BASE_ADDR	(AHB2_BASE_ADDR + 0x0000U)

/* Pocisiones de memoria para perifericos del AHB1
 * Observar que NO esta completa */
#define RCC_BASE_ADDR			(AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR			(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR			(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR			(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR			(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR			(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR			(AHB1_BASE_ADDR + 0x0000U)

/* Posiciones de memoria par perifericos del APB2
 * NOTA:NO los necesitamos en este ejercicio
#define SPI5_BASE_ADDR			(APB2_BASE_ADDR + 0x5000U)
#define TIM11_BASE_ADDR			(APB2_BASE_ADDR + 0x4800U)
#define TIM10_BASE_ADDR			(APB2_BASE_ADDR + 0x4400U)
#define TIM9_BASE_ADDR			(APB2_BASE_ADDR + 0x4000U)
#define EXTI_BASE_ADDR			(APB2_BASE_ADDR + 0x3C00U)
#define SYSCFG_BASE_ADDR		(APB2_BASE_ADDR + 0x3800U)
#define SPI4_BASE_ADDR			(APB2_BASE_ADDR + 0x3400U)
#define SPI1_BASE_ADDR			(APB2_BASE_ADDR + 0x3000U)
#define SDIO_BASE_ADDR			(APB2_BASE_ADDR + 0x2C00U)
#define ADC1_BASE_ADDR			(APB2_BASE_ADDR + 0x2000U)
#define USART6_BASE_ADDR		(APB2_BASE_ADDR + 0x1400U)
#define USART1_BASE_ADDR		(APB2_BASE_ADDR + 0x1000U)
#define TIM1_BASE_ADDR			(APB2_BASE_ADDR + 0x0000U)
 */

/* Posiciones de memoria para perifericos del APB1
 * NOTA: NO los necesitamos en este ejercicio
#define PWR_BASE_ADDR			(APB1_BASE_ADDR + 0x7000U)
#define I2C3_BASE_ADDR			(APB1_BASE_ADDR + 0x5C00U)
#define I2C2_BASE_ADDR			(APB1_BASE_ADDR + 0x5800U)
#define I2C1_BASE_ADDR			(APB1_BASE_ADDR + 0x5400U)
#define USART2_BASE_ADDR		(APB1_BASE_ADDR + 0x4400U)
#define I2Sext_BASE_ADDR		(APB1_BASE_ADDR + 0x4000U)
#define SPI3_BASE_ADDR			(APB1_BASE_ADDR + 0x3C00U)
#define SPI2_BASE_ADDR			(APB1_BASE_ADDR + 0x3800U)
#define I2S2ext_BASE_ADDR		(APB1_BASE_ADDR + 0x3400U)
#define IWDG_BASE_ADDR			(APB1_BASE_ADDR + 0x3000U)
#define WWDG_BASE_ADDR			(APB1_BASE_ADDR + 0x2C00U)
#define RTC_BASE_ADDR			(APB1_BASE_ADDR + 0x2800U)
#define TIM5_BASE_ADDR			(APB1_BASE_ADDR + 0x0C00U)
#define TIM4_BASE_ADDR			(APB1_BASE_ADDR + 0x0800U)
#define TIM3_BASE_ADDR			(APB1_BASE_ADDR + 0x0400U)
#define TIM2_BASE_ADDR			(APB1_BASE_ADDR + 0x0000U)
 */
