/*
 * stick.h
 *
 *  Created on: 6 ago. 2021
 *      Author: socco
 */

#ifndef STICK_H_
#define STICK_H_

#include "LPC845.h"
#include "dr_gpio.h"

typedef signed char 	int8_t;
typedef unsigned char 	uint8_t;
typedef short 			int16_t;
typedef unsigned short 	uint16_t;
typedef int 			int32_t;
typedef unsigned int 	uint32_t;

#define   __I     	volatile const       /*!< Defines 'read only' permissions */
#define     __R   	volatile const       /*!< Defines 'read only' permissions */

#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */
#define     __RW    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */



//!< /////////////		SYSTICK		///////////////////////////

		//!< 0xE000E010UL: Registro de control del SysTick:
	#define 	SYSTICK		( (systick_t *) 0xE000E010UL )

	//!< Tipo de dato especÃ­fico para manejar el SYSTICK
	typedef struct
	{
		union{
			__RW uint32_t SYST_CSR;
			struct{
				__RW uint32_t ENABLE:1;
				__RW uint32_t TICKINT:1;
				__RW uint32_t CLKSOURCE:1;
				__RW uint32_t RESERVED0:13; //bit 3 a 15
				__RW uint32_t COUNTFLAG:1;
				__RW uint32_t RESERVED1:15; //bit 17 a 31
			}bits;
		};
		__RW uint32_t SYST_RVR;
		__RW uint32_t SYST_CVR;
		__R uint32_t  SYST_CALIB;
	}systick_t;

	#define		SYST_CSR		SYSTICK->SYST_CSR
	#define		ENABLE			SYSTICK->bits.ENABLE
	#define		TICKINT			SYSTICK->bits.TICKINT
	#define		CLKSOURCE		SYSTICK->bits.CLKSOURCE
	#define		COUNTFLAG		SYSTICK->bits.COUNTFLAG
	#define		SYST_RVR		SYSTICK->SYST_RVR
	#define		SYST_CVR		SYSTICK->SYST_CVR
	#define		SYST_CALIB		SYSTICK->SYST_CALIB


//-----------------------------------

#define PIN_LED_VERDE	IOCON_INDEX_PIO1_0
#define PIN_LED_AZUL	IOCON_INDEX_PIO1_1
#define PIN_LED_ROJO	IOCON_INDEX_PIO1_2
#define PIN_TECLA_USR	IOCON_INDEX_PIO0_4
#define PIN_TECLA_ISP	IOCON_INDEX_PIO0_12

#define LED_VERDE	1,0
#define LED_AZUL	1,1
#define LED_ROJO	1,2
#define	TECLA_USR	0,4
#define	TECLA_ISP	0,12

#define LED_VERDE_NOT	NotPIN(LED_VERDE)
#define LED_VERDE_ON	SetPIN(LED_VERDE,0)
#define LED_VERDE_OFF	SetPIN(LED_VERDE,1)

#define LED_ROJO_NOT	NotPIN(LED_ROJO)
#define LED_ROJO_ON		SetPIN(LED_ROJO,0)
#define LED_ROJO_OFF	SetPIN(LED_ROJO,1)

#define LED_AZUL_NOT	NotPIN(LED_AZUL)
#define LED_AZUL_ON		SetPIN(LED_AZUL,0)
#define LED_AZUL_OFF	SetPIN(LED_AZUL,1)


#endif /* STICK_H_ */
