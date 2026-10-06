/*
 * ctimer.h
 *
 *  Created on: 2 feb. 2022
 *      Author: franco
 */

#ifndef CTIMER_H_
#define CTIMER_H_

#include "../../drivers/fsl_ctimer.h"

#define CTIMER                          CTIMER0
#define CTIMER0_SERVO_CHANNEL           kCTIMER_Match_0
#define CTIMER0_MOTOR_POTENCIA_CHANNEL	kCTIMER_Match_1
#define CTIMER0_PWM_PERIOD_CHANNEL      kCTIMER_Match_3

#define m_EncenderCTIMER()				CTIMER_StartTimer(CTIMER)
#define m_ApagarCTIMER()				CTIMER_StopTimer(CTIMER)
#define m_ReiniciarCTIMER()             CTIMER_Reset(CTIMER)

void Modificar_MatchValue_PWM(uint32_t channel, uint32_t MatchValue);
void Inicializar_CTIMER(void);

#endif /* CTIMER_H_ */
