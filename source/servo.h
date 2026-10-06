#ifndef SERVO_H
#define SERVO_H

#include "LPC845.h"
#include "ctimer.h"
#include "../../drivers/fsl_swm_connections.h"
#include "../../drivers/fsl_swm.h"
#include "unidades.h"

#define PIN_SERVO                       kSWM_PortPin_P0_17

#define PERIODO_SERVO                   (20*mS) /* 20000us = 20ms */  //T_on + T_off = T (periodo)

#define LIMITE_INFERIOR_PULSO           (500*uS) /* 500us = 0,5ms */ //Corresponde a 0º (apuntando hacia la izquierda)
#define LIMITE_SUPERIOR_PULSO           (2500*uS) /* 2500us = 2,5ms */ //Corresponde a 180º (apuntando hacia la derecha)

//Devuelve el tiempo de pulso necesario para girar el servo un determinado angulo
#define TIEMPO_DE_PULSO(angulo)	        (LIMITE_INFERIOR_PULSO + (angulo) * (LIMITE_SUPERIOR_PULSO - LIMITE_INFERIOR_PULSO) / 180)

#define CTIMER0_PWM_PERIOD              (PERIODO_SERVO - 1)
#define SERVO_DUTY_INICIAL              (PERIODO_SERVO - TIEMPO_DE_PULSO(FRENTE))              //Inicio el servo en 90º

#define DERECHA					        0
#define FRENTE					        90
#define IZQUIERDA				        180

#define m_GirarIzquierda()              GirarServo(IZQUIERDA)
#define m_GirarFrente()                 GirarServo(FRENTE)
#define m_GirarDerecha()                GirarServo(DERECHA)

void GirarServo(uint8_t angulo);
void ConfigurarServo(void);

#endif
