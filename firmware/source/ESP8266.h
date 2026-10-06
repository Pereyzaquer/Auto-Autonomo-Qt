/*
 * ESP8266.h
 *
 *  Created on: 12 oct. 2021
 *      Author: socco
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include <stdint.h>

void ESP8266_enviaAT(const char *);
void ESP8266_inicializar(void);
void ESP8266_conectarAP(const char *, const char *);
void ESP8266_configuraSoftAP(const char*, const char*, const char*, const char*);

uint16_t verificador_de_tramas(const char, const char*, uint8_t);
void Verificar_datos(void);
void ESP8266_envio_PC(const char*, uint8_t);
void ESP8266_envio_vector_PC(uint8_t* mensaje, uint8_t longitud);
uint8_t ESP8266_espero_fin_de_trama(const char* mensaje);
void ESP8266_Estado_Conexion(uint8_t dato);

void Verificar_Mov_Auto(uint16_t);
void Reenvio_CWMODE(void);

uint8_t ESP8266_comienzo_Envio(uint8_t longitud);
void ESP8266_finalizo_Envio(uint8_t dato);
void Actualizo_mensaje(uint8_t *p_aux);
void ESP8266_Envio_Movimiento(uint8_t mov);

#define COMANDO_CWMODE			ESP8266_enviaAT("AT+CWMODE=2\r\n")
#define COMANDO_SOFTAP			ESP8266_configuraSoftAP("ESP8266-Wifi", "pas123", "3", "0")
#define COMANDO_CIPMUX			ESP8266_enviaAT("AT+CIPMUX=1\r\n")
#define COMANDO_CIPSERVER		ESP8266_enviaAT("AT+CIPSERVER=1,8888\r\n")

#define EVENTO_No_Respuesta			4
#define EVENTO_Repito_CWMODE		5

#define t_No_respuesta()			TimerStart(EVENTO_No_Respuesta, 1, Reenvio_CWMODE, SEG)
#define t_No_respuesta_stop()		TimerStop(EVENTO_No_Respuesta)

#define t_Repito_CWMODE()			TimerStart(EVENTO_Repito_CWMODE, 5, Reenvio_CWMODE, DEC)
#define t_Repito_CWMODE_stop()		TimerStop(EVENTO_Repito_CWMODE)


// Estados de la máquina del receptor
#define	ESPERO		0
#define	CHMODE		1
#define	CWSAP		2
#define	SOFTAP		3
#define	CIPMUX		4
#define	CIPSERVER	7
#define	TERMINE		8

#define TAMBUFCHEQUEO	30
#define ERROR			0
#define EXITO			1

#define ESTADO0			0
#define ESTADO1			1
#define ESTADO2			2
#define ESTADO3			3
#define ESTADO4			4
#define ESTADO5			5
#define ESTADO6			6
#define ESTADO7			7
#define ESTADO8			8
#define ESTADO9			9
#define ESTADO10		10

#define FALLO			-1

#define DESCONECTADO	0
#define CONECTADO		1

#define REPOSO 				0
#define CARACTER_INICIAL 	1
#define CARACTER_FINAL		2

#define MOVER_ADELANTE			'f'
#define MOVER_IZQUIERDA_AV		'l'
#define MOVER_DERECHA_AV		'r'
#define MOVER_IZQUIERDA_RE		'k'
#define MOVER_DERECHA_RE		'q'
#define MOVER_ATRAS 			'b'
#define FRENAR					's'

#define MOV_AUTOMATICO_OFF		'x'
#define MOV_AUTOMATICO_ON		'o'

#define RESETEAR_DISTANCIA		'h'

#define BUFFER_SIZE				7
#define OCUPADO					0
#define DESOCUPADO				1
#define ESPERO_RESPUESTA		2
#define ESCRIBO_MENSAJE			3

#define ERROR_LEN				5
#define OK_LEN					2

#endif /* ESP8266_H_ */
