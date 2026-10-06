/*
 * ctimer.c
 *
 *  Created on: 2 feb. 2022
 *      Author: franco
 */

#include "ctimer.h"

/**
 	\file		ctimer.c
 	\fn   		void Modificar_MatchValue_PWM(uint32_t channel, uint32_t MatchValue)
 	\brief 		Actualiza el ancho de pulso del PWM de canal 'channel'
 	\details	Modifica el ancho de pulso cambiando el match value del canal que genera el PWM correspondiente.
 				El match value es el numero de ticks al que debe llegar el contador interno del CTIMER para que
 				ocurra la transicion de LOW a HIGH en el pin de salida elegido
 	\param		void
	\return		void
*/
void Modificar_MatchValue_PWM(uint32_t channel, uint32_t MatchValue)
{
    CTIMER_UpdatePwmPulsePeriod(CTIMER, channel, MatchValue);
}

/**
 	\file 		ctimer.c
 	\fn   		void Inicializar_CTIMER(void)
 	\brief 		Inicializa el CTIMER
 	\details	El prescale value es elegido de tal modo que la frecuencia de ticks del contador interno del
 				CTIMER sea 1MHz (este valor es de 17 si SystemCoreClock = 18MHz)
 	\param		void
	\return		void
*/
void Inicializar_CTIMER(void)
{
	const ctimer_config_t CTIMER0_config = {
			.mode = kCTIMER_TimerMode,
	        .input = kCTIMER_Capture_0,
			.prescale = 17	//(SystemCoreClock / 1*MHz) - 1    //Es el factor de escala al que operara el CTIMER con respecto al SystemCoreClock, 17 = (SystemCoreClock / CTIMER0_FRECUENCIA_TICK) - 1
	};

	CTIMER_Init(CTIMER, &CTIMER0_config);
}

