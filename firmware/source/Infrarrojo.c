/*
 * Infrarrojo.c
 *
 *  Created on: 13 ene. 2022
 *      Author: nicop
 */
#include "Infrarrojo.h"
#include "PR_UART0_2021.h"
#include "ESP8266.h"
#include "timer.h"
#include <string.h>

extern volatile uint16_t contador_Infra;

extern uint8_t estado_mensaje;

/**
 	\file	Infrarrojo.c
 	\fn		void Debounce_Infrarrojo(void)
 	\brief	Debounce del Infrarrojo
 	\param	void
	\return void
*/
void Debounce_Infrarrojo(void)
{
	static uint8_t estado_Anterior = LOW;
	static uint8_t debounce = 0;

	if(contador_Infra > MAX_SIZE)		//Reseteo el contador, el auto tiene que recorrer mas de 65 metros
		RESET_DISTANCIA;

	if(ESTADO_INFRA != estado_Anterior)
	{
		debounce++;

		if(debounce == N_DEBOUNCE)
		{
			debounce = 0;
			estado_Anterior = ESTADO_INFRA;
			contador_Infra++;
		}
	}
	else
	{
		debounce = 0;
	}
}

/**
 	\file	Infrarrojo.c
 	\fn		void Enviar_Distancia_USB(void)
 	\brief	Envia la cantidad de cambios de contador_Infra a la UART0
 	\param	void
	\return void
*/
void Enviar_Distancia_USB(void)
{
	static uint16_t contador_Anteior = 0;
	uint16_t contador_aux = 0;
	uint8_t p_aux[BUFFER_SIZE];
	uint8_t i = 0;

	if(contador_Anteior != contador_Infra)//Envio datos solo si cambiaron
	{
		contador_Anteior = contador_Infra;
		contador_aux = contador_Anteior;

		EnviarString("Cantidad de cambios: ");

		p_aux[0] = '+';
		p_aux[CANT_NUMEROS + 1] = '/';

		do
		{
			p_aux[CANT_NUMEROS-i] = (uint8_t)(contador_aux%10 + '0');
			contador_aux = contador_aux/10;

			i++;
		}while(i < CANT_NUMEROS);

		i = 0;
		while(i < BUFFER_SIZE)
		{
			UART0_pushTx(p_aux[i]);
			i++;
		}

		UART0_pushTx('\r');
		UART0_pushTx('\n');

	}

}

/**
 	\file	Infrarrojo.c
 	\fn		void Enviar_Distancia_PC(void)
 	\brief	Envia la distancia recorrida por el auto al dispositivo conectado
 	\param	void
	\return void
*/
void Enviar_Distancia_PC(void)
{
	static uint16_t contador_Anteior = 0;
	uint16_t contador_aux = 0;
	uint8_t p_aux[BUFFER_SIZE];
	uint8_t i = 0;

	if(contador_Anteior != contador_Infra && estado_mensaje == DESOCUPADO)	//Envio datos solo si cambiaron y si se esta desocupado el esp8266
	{
		contador_Anteior = contador_Infra;
		contador_aux = contador_Anteior;

		contador_aux = contador_aux*LONGITUD/NUM_CAM;

		p_aux[0] = '+';
		p_aux[CANT_NUMEROS + 1] = '/';

		do
		{
			p_aux[CANT_NUMEROS-i] = (uint8_t)(contador_aux%10 + '0');
			contador_aux = contador_aux/10;

			i++;
		}while(i < CANT_NUMEROS);

//		ESP8266_envio_vector_PC(p_aux, '7');

		Actualizo_mensaje(p_aux);
		ESP8266_comienzo_Envio('7');
	}

	t_Envio_Distancia();

}

/**
 	\file	Infrarrojo.c
 	\fn		void Reset_Contador_Infra(void)
 	\brief	Resetea la distancia recorrida por el auto
 	\param	void
	\return void
*/
void Reset_Contador_Infra(void)
{
	contador_Infra = 0;
}

