/*
 * Infrarrojo.h
 *
 *  Created on: 13 ene. 2022
 *      Author: nicop
 */

#ifndef INFRARROJO_H_
#define INFRARROJO_H_

#include "dr_gpio.h"
#include "ESP8266.h"
#include "pinauto.h"

#define ESTADO_INFRA	GetPIN(E_Infrarrojo)

#define LOW				0
#define HIGH			1

#define N_DEBOUNCE		5
#define MAX_SIZE		65536

#define PI				3.141592
#define DIAMETRO		66				//Esta en milimetros
#define LONGITUD		DIAMETRO*PI
#define NUM_CAM			10				//Cantidad de cambios en una vuelta (hay 5 radios)

#define CANT_NUMEROS	5				//Cantidad de digitos que tiene contador_Infra

#define EVENTO_INFORMO_DISTANCIA	3

#define t_Envio_Distancia()			TimerStart(EVENTO_INFORMO_DISTANCIA, 2, Enviar_Distancia_PC, SEG)
#define t_Envio_Distancia_stop()	TimerStop(EVENTO_INFORMO_DISTANCIA)


void Debounce_Infrarrojo(void);
void Enviar_Distancia_PC(void);
void Enviar_Distancia_USB(void);
void Reset_Contador_Infra(void);

#define RESET_DISTANCIA Reset_Contador_Infra()

#endif /* INFRARROJO_H_ */
