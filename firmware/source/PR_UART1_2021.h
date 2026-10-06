/*
 * PR_UART1_2021.h
 *
 *  Created on: 12 oct. 2021
 *      Author: socco
 */

#ifndef PR_UART1_2021_H_
#define PR_UART1_2021_H_

#include "LPC845.h"
#include "PR_UART1_2021.h"


#define TXBUFFER1_SIZE	120    //arbitrario
#define RXBUFFER1_SIZE	120    //arbitrario
#define MAX_DATOS1	    30     //arbitrario

//primitivas de recepción y transmisión. No están comprometidas con el HW
void UART1_pushTx(uint8_t dato);
uint16_t UART1_popTx(void);
void UART1_pushRx(uint8_t dato);
uint16_t UART1_popRx(void);
void EnviarString1 (const char *str);
int16_t Transmitir1 (const void * , uint8_t );
//------------------------------------------------------------------------

//Macro definida para arrancar la Tx y no pinchar capas
#define   START_TX1()	USART1->INTENSET = (1 << 2)


#endif /* PR_UART1_2021_H_ */
