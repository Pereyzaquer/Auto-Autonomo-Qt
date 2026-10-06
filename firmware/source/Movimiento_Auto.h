/*
 * Movimiento_Auto.h
 *
 *  Created on: 19 dic. 2021
 *      Author: franco
 */

#ifndef MOVIMIENTO_AUTO_H_
#define MOVIMIENTO_AUTO_H_

#include "movimiento.h"
#include "pinauto.h"
#include "servo.h"
#include "ultrasonido.h"

enum modoAuto {AUTOMATICO, MANUAL};
enum estadoMovimiento {REPOSO_AUTO, ESPERO_A_MEDIR, DECIDO_ACCION, GIRANDO_AUTO, GIRANDO_SERVO};
enum accionAuto {AVANZO, GIRO_DERECHA_AV, GIRO_IZQUIERDA_AV, GIRO_DERECHA_RE, GIRO_IZQUIERDA_RE, RETROCEDO, FRENO, IDLE};

/*
 * Si el ancho de pulso es igual a PERIODO_SERVO, match nunca se producira ya que la variable contadora
 * llegara hasta PERIODO_SERVO - 1 y luego se reiniciara, por lo que el pin del servo estara siempre en LOW
 */
#define APAGAR_SERVO			    Modificar_MatchValue_PWM(CTIMER0_SERVO_CHANNEL, PERIODO_SERVO)

#define GIRAR_DERECHA				Auto_girar_derecha()
#define GIRAR_IZQUIERDA			    Auto_girar_izquierda()

#define RETROCEDER_DERECHA			Auto_retroceder_rueda_derecha()
#define RETROCEDER_IZQUIERDA	    Auto_retroceder_rueda_izquierda()

#define AVANZAR_AUTO 				Auto_avanzar()
#define DETENER_AUTO 				Auto_detener()
#define RETROCEDER_AUTO				Auto_retroceder()

#define DETECCION_FRONTAL 		    GirarServo(FRENTE)
#define DETECCION_DERECHA		    GirarServo(DERECHA)
#define DETECCION_IZQUIERDA		    GirarServo(IZQUIERDA)

#define t_EsperoAMedir()			TimerStart(EVENTO_MEDIR_DISTANCIA, 1, medicion_handler, DEC)
#define t_EsperoAMedir_stop()		TimerStop(EVENTO_MEDIR_DISTANCIA)
#define t_MedicionTimeout()			TimerStart(EVENTO_MEDICION_TIMEOUT, TIEMPO_MAXIMO_ESPERA, timeout_handler, DEC)
#define t_MedicionTimeout_stop()	TimerStop(EVENTO_MEDICION_TIMEOUT)
#define t_GirarAuto()				TimerStart(EVENTO_GIRAR_RUEDAS, TIEMPO_DE_GIRO, girando_handler, DEC)
#define t_GirarAuto_stop()			TimerStop(EVENTO_GIRAR_RUEDAS)
#define t_GirarServo()				TimerStart(EVENTO_GIRAR_SERVO, 13, girar_servo_handler, DEC)
#define t_GirarServo_stop()			TimerStop(EVENTO_GIRAR_SERVO)
#define t_RetrocederAuto()			TimerStart(EVENTO_GIRAR_RUEDAS, TIEMPO_DE_GIRO_COMPLETO, girando_handler, SEG)
//#define t_RetrocederAuto_stop()			TimerStop(EVENTO_GIRAR_RUEDAS)

#define EVENTO_GIRAR_RUEDAS			29
#define EVENTO_GIRAR_SERVO			28
#define EVENTO_MEDIR_DISTANCIA		27
#define EVENTO_MEDICION_TIMEOUT		26

#define TIEMPO_DE_GIRO				6	//6 decenas de segundo o 600ms. Tiempo para girar 90º
#define TIEMPO_DE_GIRO_COMPLETO		1	//1s. Tiempo para girar 180º
#define TIEMPO_MAXIMO_ESPERA		5	//5 decenas de segundo o 500ms

#define DISTANCIA_MAX_OBJETO		20	//20cm
#define DISTANCIA_FUERA_DE_LIMITE	0xffffffff

void Movimiento_Auto(uint8_t modo);
void girando_handler(void);
void girar_servo_handler(void);
void medicion_handler(void);
void timeout_handler(void);

void Auto_avanzar(void);
void Auto_retroceder(void);
void Auto_detener(void);
void Auto_girar_derecha(void);
void Auto_girar_izquierda(void);
void Auto_retroceder_rueda_derecha(void);
void Auto_retroceder_rueda_izquierda(void);

void Decido_Movimiento(void);


#endif /* MOVIMIENTO_AUTO_H_ */
