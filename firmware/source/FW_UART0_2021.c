/**
 	\file FW_serie.c
 	\brief Drivers de comunicacion serie
*/

#include "PR_UART0_2021.h"
#include "LPC845.h"

uint8_t txStart = 0;  //flag para activar/desactivar transmisión serie


//USART0_IRQHandler   USART0_USART_IRQHANDLER
/**
 	\file FW_serie.c
 	\brief isr de transmisión/recepción serie.
 	\details Analiza si la interrupción fue de Rx o de Tx y opera en consecuencia. No contempla errores
 	\param [in] void
	\return void
*/
void USART0_IRQHandler (void)
{
	int16_t auxTemporal;
	uint32_t stat = USART0->STAT;   //consulto el estado de la UART
	
    // CASO RECEPCION
	if(stat & (1 << 0))  //Receiver Ready Flag. Si vale 1, indica que un dato es disponible para ser leído del buffer del
			             //receptor. Se borra solo luego de leer uno de los registros RXDAT o RXDATSTAT.
	{
		auxTemporal = (int32_t) USART0->RXDAT;    //recibo dato
		UART0_pushRx((uint8_t) auxTemporal);            //manejo bufferRx correctamente
	}

	//CASO TRANSMISION
	if(stat & (1 << 2))  //Transmitter Ready flag.  Si vale 1, indica que se puede escribir en TXDAT.
		// El dato previo podria aún estar en proceso de transmisión. Se borra solo cuando se escribe el registro TXDAT, y se pone en 1
		// cuando el dato es movido desde TXDAT al serializador.
	{
		auxTemporal = UART0_popTx();    //Si hubo una int de Tx, leo del bufferTx y almaceno en variable de trabajo (auxTemporal)
		if(auxTemporal >= 0)      //¿sigue habiendo datos para tx? (popTx() devuelve -1 cuando ya no hay datos para tx)
			USART0->TXDAT = (uint8_t)auxTemporal;  //escribo TXDAT (transmito)
		else
		{   //no hay mas datos.... Desactivo Int Tx
			USART0->INTENCLR = (1 << 2); //disable int TX
			txStart = 0;  //cuando vale 0 indica que no hay tx activa
		}
	}
}

//==============================================================================================================
//INFO ADICIONAL

/**
 	\file FW_serie.c
 	\brief Driver para inicializar la UART
 	\details En caso de no usar la herramienta gráfica de configuración, el programador debería escribir esta función
 	\details accediendo directamente a los registros del micro
 	\param [in] Configuración de la comunicación (baudrate, bits stop, paridad, etc)
	\return 0 por exito, -1 por Error (no hay mas datos para transmitir en el buffer)
*/

//void UART0_Init(............)
//{
//	// HABILITO LA UART0
//	SYSCON->SYSAHBCLKCTRL0 |= (1 << 14);						// 14 = UART0
//
//	// CONFIGURO LA MATRIX
//	SWM0->PINASSIGN.PINASSIGN0 = (8 << 0) | (9 << 8);			// TX0 = P0.8	RX0 = P0.9
//
//	// CONFIGURACION GENERAL
//	USART0->CFG = 	(0 << 0)		// 0=DISABLE 1=ENABLE
//					| (1 << 2)		// 0=7BITS 1=8BITS 2=9BITS
//					| (0 << 4)		// 0=NOPARITY 2=PAR 3=IMPAR
//					| (0 << 6)		// 0=1BITSTOP 1=2BITSTOP
//					| (0 << 9)		// 0=NOFLOWCONTROL 1=FLOWCONTROL
//					| (0 << 11)		// 0=ASINCRONICA 1=SINCRONICA
//					| (0 << 14);	// 0=SLAVE 1=MASTER
//
//	// CONFIGURACION EXTRA
//	USART0->CTL = 0;
//
//	// CONFIGURACION INTERRUPCIONES
//	USART0->INTENSET = (1 << 0);	//RX Y TX INTERRUPT
//
//	// CONFIGURACION DEL BAUDRATE
//	UART0CLKSEL = 1;
//
//	USART0->BRG = (FREQ_PRINCIPAL / baudrate) / 8;
//
//	// CONFIGURACION INTERRUPCIONES GLOBALES
//	NVIC->ISER[0] = (1 << 3); /* enable interrupt */
//
//	// ENABLE LA UART
//	USART0->CFG |= 	(1);			// ENABLE USART0
//
//	return;
//}
//==============================================================================================================

