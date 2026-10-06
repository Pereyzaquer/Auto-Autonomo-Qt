/*
 * movimiento.h
 *
 *  Created on: 9 nov. 2021
 *      Author: nicop
 */

#ifndef MOVIMIENTO_H_
#define MOVIMIENTO_H_


#include "unidades.h"
#include "dr_gpio.h"
#include "pinauto.h"
#include "ctimer.h"
#include "servo.h"
#include "../../drivers/fsl_swm_connections.h"
#include "../../drivers/fsl_swm.h"

#define APAGADO				0
#define ENCENDIDO			1

#define PERMITIR_MOVIMIENTO	Permitir_Movimiento()
#define ANULAR_MOVIMIENTO	Anular_Movimiento()

#define PIN_MOTOR_POTENCIA1	kSWM_PortPin_P0_20
#define PIN_MOTOR_POTENCIA2	kSWM_PortPin_P0_21

//Vamos a usar este periodo para cualquier PWM con el CTIMER
#define PERIODO_POTENCIA2				PERIODO_SERVO

void Motor_derecha_Avanzo(void);
void Motor_izquierda_Avanzo(void);

void Motor_derecha_Retroceder(void);
void Motor_izquierda_Retroceder(void);

void Motor_Avanzar(void);
void Motor_Detener(void);
void Motor_Retroceder(void);

void Permitir_Movimiento(void);
void Anular_Movimiento(void);

void ConfigurarMotorPotencia(void);
void MoficarVelocidadMotor(uint32_t);

#define POT_MAX			PERIODO_POTENCIA2
#define POT_MIN			0
#define POT_AJUSTADA	16300				//Potencia ajustada para que el auto vaya recto

#endif /* MOVIMIENTO_H_ */
