/*
 * PR_UART1_2021.c
 *
 *  Created on: 12 oct. 2021
 *      Author: socco
 */

#include "PR_UART1_2021.h"
#include "LPC845.h"
#include <string.h>


extern uint8_t txStart1;  //flag para activar/desactivar transmisión serie

// Buffer de Transmision
uint8_t bufferTx1[TXBUFFER1_SIZE];
// Buffer de Recepcion
uint8_t bufferRx1[RXBUFFER1_SIZE];

// Indices de Recepcion
uint8_t inx_rx_in1 = 0;
uint8_t inx_rx_out1 = 0;
// Indices de Transmision
uint8_t inx_tx_in1 = 0;
uint8_t inx_tx_out1 = 0;


/**
 	\file PR_serie.c
 	\brief Primitiva de transmisión serie
 	\details Escribe en el buffer de Tx protegiendo índices. Dispara la tx. Está del lado del main
 	\param [in] void
 	\return void
*/
void UART1_pushTx(uint8_t dato)
{
	bufferTx1[inx_tx_in1] = dato;
	inx_tx_in1++;
	inx_tx_in1 %= TXBUFFER1_SIZE;

	if(txStart1 == 0){   //si no hay transmisión activa.... la activo
		txStart1 = 1;
		//Escribo TXDAT y activo int Tx. Es una macro para no pinchar capas
		START_TX1();     //#define START_TX()	( USART0->INTENSET = (1 << 2); ) // int tx
	}
}


/**
 	\file PR_serie.c
 	\brief Transmisión serie.
 	\details Es llamada por la isr extrayendo el dato a transmitir desde el buffer de Tx protegiendo índices
 	\details La consideramos primitiva pues aunque la llama la isr no está comprometida con el HW
 	\param [in] void
	\return 0 por exito, -1 por Error (no hay mas datos para transmitir en el buffer)
*/
uint16_t UART1_popTx(void)
{
	uint16_t dato = -1;

	if(inx_tx_in1 != inx_tx_out1){
		dato = (uint8_t) bufferTx1[inx_tx_out1];
		inx_tx_out1++;
		inx_tx_out1 %= TXBUFFER1_SIZE;
	}
	return dato;
}

/**
 	\file PR_serie.c
 	\brief Recepción serie.
 	\details Es invocada por la isr para escribir en el buffer de Rx protegiendo índices
 	\details La consideramos primitiva pues aunque la llama la isr no está comprometida con el HW
 	\param [in] Recibe el dato que fue leído en el registro de recepción serie RXDAT
	\return void
*/
void UART1_pushRx(uint8_t dato)
{
	bufferRx1[inx_rx_in1] = dato;
	inx_rx_in1++;
	inx_rx_in1 %= RXBUFFER1_SIZE;
}


/**
 	\file PR_serie.c
 	\brief Primitiva de recepción serie.
 	\details Lee desde el buffer de Rx protegiendo índices. Está del lado del main
 	\details La consideramos primitiva pues aunque la llama la isr no está comprometida con el HW
 	\param [in] void
	\return 0 por exito, -1 por Error (no hay mas datos para recibir desde el buffer)
*/
uint16_t UART1_popRx(void)
{
	uint16_t dato = -1;

	if(inx_rx_in1 != inx_rx_out1){
		dato = (uint16_t) bufferRx1[inx_rx_out1],
		inx_rx_out1++;
		inx_rx_out1 %= RXBUFFER1_SIZE;
	}
	return dato;
}

/**
 	\file PR_serie.c
 	\brief Primitiva de transmisión serie de mas alto nivel para enviar cadenas.
 	\details Descompone la cadena en caracteres e invoca a pushTx() caracter a caracter.
 	\param [in] cadena a transmitir
	\return void
*/
void EnviarString1 (const char *str)
{
	uint8_t i;
	for( i = 0 ; str[i] ; i++ )
		UART1_pushTx(str[i]);
}

/**
	\fn int16_t Transmitir ( const void * datos , uint8_t cant)
	\brief Primitiva de transmisión serie de mas alto nivel para enviar bloques de datos.
	\brief despacha los datos a transmitir desde un buffer
 	\author Ing. Marcelo Trujillo
 	\date 5 de oct. de 2017
	\param [in] const void * datos: datos a transmitir
	\param [in] uint8_t cant: cantidad de datos a transmitir
	\return 0 por exito, -1 por Error (datos exedidos)
*/
int16_t Transmitir1 (const void * datos , uint8_t cant )
{
	uint8_t	i;

	if ( cant > MAX_DATOS1 )
		return -1;

	if (cant == 0)
		cant = strlen (datos);

	for ( i = 0 ; i < cant ; i++ )
		UART1_pushTx( * ( ( uint8_t * ) ( datos + i ) ) );

	return 0 ;
}
//----------------------------------------------------------------------------


