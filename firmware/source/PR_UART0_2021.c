/**
 	\file PR_serie.c
 	\brief Primitivas de comunicación serie
 	\details buffers de RX y TX protegidos
*/

#include "PR_UART0_2021.h"
#include "LPC845.h"
#include <string.h>


extern uint8_t txStart;  //flag para activar/desactivar transmisión serie

// Buffer de Transmision
uint8_t bufferTx[TXBUFFER_SIZE];
// Buffer de Recepcion
uint8_t bufferRx[RXBUFFER_SIZE];

// Indices de Recepcion
uint8_t inx_rx_in = 0;
uint8_t inx_rx_out = 0;
// Indices de Transmision
uint8_t inx_tx_in = 0;
uint8_t inx_tx_out = 0;


/**
 	\file PR_serie.c
 	\brief Primitiva de transmisión serie
 	\details Escribe en el buffer de Tx protegiendo índices. Dispara la tx. Está del lado del main
 	\param [in] void
 	\return void
*/
void UART0_pushTx(uint8_t dato)
{
	bufferTx[inx_tx_in] = dato;
	inx_tx_in++;
	inx_tx_in %= TXBUFFER_SIZE;

	if(txStart == 0){   //si no hay transmisión activa.... la activo
		txStart = 1;
		//Escribo TXDAT y activo int Tx. Es una macro para no pinchar capas
		START_TX();     //#define START_TX()	( USART0->INTENSET = (1 << 2); ) // int tx
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
uint16_t UART0_popTx(void)
{
	uint16_t dato = -1;

	if(inx_tx_in != inx_tx_out){
		dato = (uint8_t) bufferTx[inx_tx_out];
		inx_tx_out++;
		inx_tx_out %= TXBUFFER_SIZE;
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
void UART0_pushRx(uint8_t dato)
{
	bufferRx[inx_rx_in] = dato;
	inx_rx_in++;
	inx_rx_in %= RXBUFFER_SIZE;
}


/**
 	\file PR_serie.c
 	\brief Primitiva de recepción serie.
 	\details Lee desde el buffer de Rx protegiendo índices. Está del lado del main
 	\details La consideramos primitiva pues aunque la llama la isr no está comprometida con el HW
 	\param [in] void
	\return 0 por exito, -1 por Error (no hay mas datos para recibir desde el buffer)
*/
uint16_t UART0_popRx(void)
{
	uint16_t dato = -1;

	if(inx_rx_in != inx_rx_out){
		dato = (uint16_t) bufferRx[inx_rx_out],
		inx_rx_out++;
		inx_rx_out %= RXBUFFER_SIZE;
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
void EnviarString (const char *str)
{
	uint8_t i;
	for( i = 0 ; str[i] ; i++ )
		UART0_pushTx(str[i]);
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
int16_t Transmitir (const void * datos , uint8_t cant )
{
	uint8_t	i;

	if ( cant > MAX_DATOS )
		return -1;
	
	if (cant == 0)
		cant = strlen (datos);

	for ( i = 0 ; i < cant ; i++ )
		UART0_pushTx( * ( ( uint8_t * ) ( datos + i ) ) );

	return 0 ;
}
//----------------------------------------------------------------------------
