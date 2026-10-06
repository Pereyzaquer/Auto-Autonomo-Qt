#ifndef ULTRASONIDO_H
#define ULTRASONIDO_H

#include "../../drivers/fsl_pint.h"
#include "../../drivers/fsl_sctimer.h"
#include "../../drivers/fsl_swm_connections.h"
#include "../../drivers/fsl_swm.h"
#include "dr_gpio.h"
#include "unidades.h"
#include "pinauto.h"
#include "LPC845.h"

/* Definition of peripheral ID */
#define SCT0_PERIPHERAL SCT0
/* Definition of clock source frequency */
#define SCT0_CLOCK_FREQ 18000000UL

#define VELOCIDAD_DEL_SONIDO    0.03432   //343.2 (m/s) = 34320 (cm/s) = 0.03432 (cm/us)
#define DISTANCIA_MAX           450.0     //450 cm

#define TIEMPO_ENTRE_MEDICION           (100*mS)    //50ms. Es lo recomendado por el fabricante
#define DURACION_PULSO_TRIG             (10*uS)

#define SCTIMER_COUNTER			        kSCTIMER_Counter_U
#define PIN_SERVO                       kSWM_PortPin_P0_17
#define PIN_TRIG_ULTRASONIDO            kSWM_PortPin_P0_26

#define APAGO_TRIG		0
#define	DISPARO_TRIG	1

//Se divide por 2 ya que el sonido recorre el doble de distancia, entre que rebota en una superficie y regresa al sensor
#define DISTANCIA(t) ((float) ((float) (t) * VELOCIDAD_DEL_SONIDO / 2.0))

//Pongo el contador en 0 y comienzo a contar
#define m_IniciarMedicion()				SCTIMER_SetCOUNTValue(SCT0, SCTIMER_COUNTER, 0)

//Freno el timer
#define m_FrenarMedicion()				SCTIMER_StopTimer(SCT0, SCTIMER_COUNTER)

#define LLEGO_SONIDO		(PINT->FALL & (1 << ECHO_INTERRUPT))
#define SONIDO_ENVIADO		(PINT->RISE & (1 << ECHO_INTERRUPT))

#define Ultrasonido_Handler     PIN_INT0_IRQHandler

void ConfigurarUltrasonido(void);

#endif
