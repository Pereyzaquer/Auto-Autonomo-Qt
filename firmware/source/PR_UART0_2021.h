/*
 * PR_Serie.h
 *
 *  Created on: 09/07/2013
 *      Author: Pablo
 */

#ifndef PR_SERIE_H_
#define PR_SERIE_H_

#include "LPC845.h"
#include "PR_UART0_2021.h"


#define TXBUFFER_SIZE	240    //arbitrario
#define RXBUFFER_SIZE	120    //arbitrario
#define MAX_DATOS	    30     //arbitrario

//primitivas de recepción y transmisión. No están comprometidas con el HW
void UART0_pushTx(uint8_t dato);
uint16_t UART0_popTx(void);
void UART0_pushRx(uint8_t dato);
uint16_t UART0_popRx(void);
void EnviarString (const char *str);
int16_t Transmitir (const void * , uint8_t );
//------------------------------------------------------------------------

//Macro definida para arrancar la Tx y no pinchar capas
#define   START_TX()	USART0->INTENSET = (1 << 2)


#endif /* PR_SERIE_H_ */
