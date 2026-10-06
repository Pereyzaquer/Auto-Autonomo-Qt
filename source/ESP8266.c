/*
 * ESP8266.c
 *
 *  Created on: 12 oct. 2021
 *      Author: socco
 */
#include "ESP8266.h"
#include "PR_UART0_2021.h"
#include "PR_UART1_2021.h"
#include <string.h>
#include "stick.h"
#include "Movimiento_Auto.h"
#include "Infrarrojo.h"
#include "timer.h"

volatile uint8_t id = 0;
extern volatile uint8_t estado_conexion;
extern volatile uint32_t distancia;

volatile uint16_t dato;
extern volatile uint8_t modo, accion;

extern uint8_t mensaje[BUFFER_SIZE];
extern uint8_t estado_mensaje;


/**
 	\file ESP8266.c
 	\fn   void ESP8266_enviaAT(const char *cmd)
 	\brief Envia un comando al ESP8266
 	\param const char *cmd: String enviado
	\return void
*/
void ESP8266_enviaAT(const char *cmd)
{
	EnviarString1(cmd);
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_conectarAP(const char *ssid, const char *psw)
 	\brief Se conecta a una red
 	\details Se conecta a una red pre-existente
 	\param const char *ssid: Nombre de la red
 	\param const char *pwd: Contaseña de la red
	\return void
*/
void ESP8266_conectarAP(const char *ssid, const char *psw)
{
	EnviarString1("AT+CWJAP=");
	UART1_pushTx('"');
	EnviarString1(ssid);
	UART1_pushTx('"');
	UART1_pushTx(',');
	UART1_pushTx('"');
	EnviarString1(psw);
	UART1_pushTx('"');
	UART1_pushTx('\r');
	UART1_pushTx('\n');
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_configuraSoftAP(const char *ssid, const char *pwd, const char *ch, const char *ecn)
 	\brief Configura el SoftAP
 	\details Setea un nombre, una contraseña y otros parametros para la creacion de una SoftAP
 	\param const char *ssid: Nombre de la red
 	\param const char *pwd: Contaseña de la red
 	\param const char *ch: Canal inalámbrico
 	\param const char *ecn: Tipo de codificacion
	\return void
*/
void ESP8266_configuraSoftAP(const char *ssid, const char *pwd, const char *ch, const char *ecn)
{
	EnviarString1("AT+CWSAP=");
	UART1_pushTx('"');
	EnviarString1(ssid);
	UART1_pushTx('"');
	UART1_pushTx(',');
	UART1_pushTx('"');
	EnviarString1(pwd);
	UART1_pushTx('"');
	UART1_pushTx(',');
	EnviarString1(ch);
	UART1_pushTx(',');
	EnviarString1(ecn);

	UART1_pushTx('\r');
	UART1_pushTx('\n');
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_inicializar(void)
 	\brief Inicializacion del ESP8266 para la creacion de un SoftAP
 	\details Al finalizar el led del LPC845 pasara de rojo a azul, esto significa que la
 	inicializacion fue existoso.
 	\details En caso de que el esp no se encuentre conectado correctamente el le rojo va a
 	titilar con u8n periodo de 1 segundo mientras reenvia el primer comando de inicializacion
 	\param void
	\return void
*/
void ESP8266_inicializar(void)
{

	int16_t dato;
	static uint16_t estado_error = ESTADO0;
	static uint8_t estado = CWSAP;
	static uint16_t estado_aux = ESTADO0;
	uint32_t strlen_ok = strlen("OK");
	uint32_t strlen_error = strlen("ERROR");

	COMANDO_CWMODE;
	t_No_respuesta();

	while(estado != TERMINE)
	{
		dato = UART1_popRx();
		if(dato > 0)
		{

			switch(estado)
			{
				case CWSAP:
					estado_aux = verificador_de_tramas(dato, "OK",estado_aux);
					if(estado_aux == strlen_ok)
					{
						t_No_respuesta_stop();
						t_Repito_CWMODE_stop();

						COMANDO_SOFTAP;
						estado_aux = ESTADO0;
						estado = CIPMUX;

						LED_ROJO_ON;		//Lo enciendo por si se apago en el titileo

					}
					break;

				case CIPMUX:
					estado_aux = verificador_de_tramas(dato, "OK",estado_aux);
					if(estado_aux == strlen_ok)
					{
						COMANDO_CIPMUX;
						estado_aux = ESTADO0;
						estado = CIPSERVER;
					}
					break;

				case CIPSERVER:
					estado_aux = verificador_de_tramas(dato, "OK",estado_aux);
					if(estado_aux == strlen_ok)
					{
						COMANDO_CIPSERVER;
						estado_aux = ESTADO0;
						estado = ESPERO;
					}
					break;
				case ESPERO:
					estado_aux = verificador_de_tramas(dato, "OK",estado_aux);
					if(estado_aux == strlen_ok)
					{
						estado_aux = ESTADO0;
						estado = TERMINE;
					}
					break;
			}

			estado_error = verificador_de_tramas(dato, "ERROR", estado_error);
			if(estado_error == strlen_error)
			{
				LED_AZUL_OFF;
				LED_ROJO_OFF;
				LED_VERDE_OFF;

				estado = CWSAP;
				estado_error = ESTADO0;
				estado_aux = ESTADO0;

				COMANDO_CWMODE;
			}

//			UART0_pushTx(dato);
		}

		TimerEvent();
	}
}


/**
 	\file ESP8266.c
 	\fn   uint16_t verificador_de_tramas(const char dato, const char* mensaje, uint8_t estado_aux)
 	\brief Verifica las tramas
 	\details Devuelve la longitud de la trama que se esta esperando para finalizar
 	\param const char dato: informacion entrante, se consigue con UART1_popRx()
 	\param const char* mensaje: Trama que se esta esperando
 	\param const uint8_t estado_aux: Posicion de la trama en la que se encuentra
	\return uint16_t posicion de la trama en la que se encuentra
*/
uint16_t verificador_de_tramas(const char dato, const char* mensaje, uint8_t estado_aux)
{
	switch (estado_aux)
	{
		case ESTADO0:
			if(dato == mensaje[estado_aux])
				return ESTADO1;
			break;

		case ESTADO1:
			if(dato == mensaje[estado_aux])
				return ESTADO2;
			else
				return ESTADO0;
			break;

		case ESTADO2:
			if(dato == mensaje[estado_aux])
				return ESTADO3;
			else
				return ESTADO0;
			break;

		case ESTADO3:
			if(dato == mensaje[estado_aux])
				return ESTADO4;
			else
				return ESTADO4;
			break;

		case ESTADO4:
			if(dato == mensaje[estado_aux])
				return ESTADO5;
			else
				return ESTADO0;
			break;

		case ESTADO5:
			if(dato == mensaje[estado_aux])
				return ESTADO6;
			else
				return ESTADO0;
			break;

		case ESTADO6:
			if(dato == mensaje[estado_aux])
				return ESTADO7;
			else
				return ESTADO0;
			break;

		case ESTADO7:
			if(dato == mensaje[estado_aux])
				return ESTADO8;
			else
				return ESTADO0;
			break;

		case ESTADO8:
			if(dato == mensaje[estado_aux])
				return ESTADO9;
			else
				return ESTADO0;
			break;

		case ESTADO9:
			if(dato == mensaje[estado_aux])
				return ESTADO10;
			else
				return ESTADO0;
			break;

		case ESTADO10:
			if(dato == mensaje[estado_aux])
				return ESTADO0;
			else
				return ESTADO0;
			break;

		default:
			return ESTADO0;
			break;
	}
	return ESTADO0;
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_envio_PC(const char* mensaje, uint8_t longitud)
 	\brief Envia datos del ESP8266 al dispositivo conectado
 	\details Esta funcion tiene un comprobante de error en caso de que se intente mandar
 	datos a un dispositivo no conectado (el led se pone en rojo). Esta funcion envia datos
 	al id que se adquirio en ESP8266_Estado_Conexion(). No puede mandar carractes mas largos que 9 bytes
 	\param const char* mensaje: String que se quiera mandar
 	\param uint8_t longitud: Longuitud del string (se manda en caracter ASCII)
	\return void
*/
void ESP8266_envio_PC(const char* mensaje, uint8_t longitud)
{
	ESP8266_enviaAT("AT+CIPSEND=");
	UART1_pushTx(id);
	UART1_pushTx(',');
	UART1_pushTx(longitud);
	ESP8266_enviaAT("\r\n");

	ESP8266_espero_fin_de_trama("OK");

	ESP8266_enviaAT(mensaje);
	ESP8266_enviaAT("\r\n");
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_envio_vector_PC(char* mensaje, uint8_t longitud, uint8_t tamanio)
 	\brief Envia datos del ESP8266 al dispositivo conectado
 	\details Esta funcion tiene un comprobante de error en caso de que se intente mandar
 	datos a un dispositivo no conectado (el led se pone en rojo). Esta funcion envia datos
 	al id que se adquirio en ESP8266_Estado_Conexion(). No puede mandar carractes mas largos que 9 bytes
 	\param char* mensaje: String que se quiera mandar
 	\param uint8_t longitud: Longuitud del string (se manda en caracter ASCII)
	\return void
*/
void ESP8266_envio_vector_PC(uint8_t* mensaje, uint8_t longitud)
{
	uint8_t i = 0;

	ESP8266_enviaAT("AT+CIPSEND=");
	UART1_pushTx(id);
	UART1_pushTx(',');
	UART1_pushTx(longitud);
	ESP8266_enviaAT("\r\n");

	ESP8266_espero_fin_de_trama("OK");

	while(i < (longitud - '0'))
	{
		UART1_pushTx(mensaje[i]);
		i++;
	}

	ESP8266_enviaAT("\r\n");
}

/**
 	\file ESP8266.c
 	\fn   uint8_t ESP8266_comienzo_Envio(uint8_t longitud)
 	\brief Envia datos del ESP8266 al dispositivo conectado
 	\details Envia datos de una forma mas eficiente que las otras funciones
 	\param uint8_t longitud: Longuitud del string (se manda en caracter ASCII)
	\return uint8_t: devuelve 0 en caso de que este desocupado y 1 si esta ocupado
*/
uint8_t ESP8266_comienzo_Envio(uint8_t longitud)
{
	if(estado_mensaje == DESOCUPADO)
	{
		ESP8266_enviaAT("AT+CIPSEND=");
		UART1_pushTx(id);
		UART1_pushTx(',');
		UART1_pushTx(longitud);
		ESP8266_enviaAT("\r\n");

		estado_mensaje = ESPERO_RESPUESTA;

		return 0;
	}
	return 1;
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_finalizo_Envio(uint8_t dato)
 	\brief Finaliza el envio del menesaje
 	\details Esta funcion no termina la comunicacion sino finaliza el envio
 	de un mensaje. En caso de error el led se pone en rojo
 	\param uint8_t dato: dato que envia el ESP8266 a traves de la funcion UART1_popRx()
	\return void
*/
void ESP8266_finalizo_Envio(uint8_t dato)
{
	static uint8_t estado_ok = ESTADO0;
	static uint8_t estado_error = ESTADO0;
	uint8_t i;

	if(estado_mensaje == ESPERO_RESPUESTA)
	{
		estado_ok = verificador_de_tramas(dato, "OK", estado_ok);
		estado_error = verificador_de_tramas(dato, "ERROR", estado_error);

		if(estado_error == ERROR_LEN)
		{
			estado_error = ESTADO0;
			estado_ok = ESTADO0;
			estado_mensaje = DESOCUPADO;

			LED_ROJO_ON;
			LED_AZUL_OFF;
			LED_VERDE_OFF;
		}
		else if(estado_ok == OK_LEN)
		{
			i = 0;
			estado_error = ESTADO0;
			estado_ok = ESTADO0;
			estado_mensaje = ESCRIBO_MENSAJE;

			LED_ROJO_OFF;
			LED_AZUL_OFF;
			LED_VERDE_ON;
		}
	}
	if(estado_mensaje == ESCRIBO_MENSAJE)
	{
		while(i <BUFFER_SIZE)
		{
			UART1_pushTx(mensaje[i]);
			i++;
		}

		ESP8266_enviaAT("\r\n");

		estado_mensaje = DESOCUPADO;
	}
}

/**
 	\file ESP8266.c
 	\fn   uint8_t ESP8266_espero_fin_de_trama(const char* mensaje)
 	\brief Espera la respuesta del ESP8266
 	\details En caso de que la respuesta sea ERROR el led del LPC845 se pondra en rojo
 	\param const char* mensaje: mensaje que se esta esperando
	\return uint8_t: devuelve 0 en caso de que la respuesta del ESP8266 sea ERROR
*/
uint8_t ESP8266_espero_fin_de_trama(const char* mensaje)
{
	uint8_t estado = ESTADO0;
	uint8_t estadoaux = ESTADO0;
	int16_t dato;
	uint32_t strlen_mensaje = strlen(mensaje);
	uint32_t strlen_error = strlen("ERROR");

	while(estado != strlen_mensaje && estadoaux != strlen_error)
	{
		dato = UART1_popRx();
		if(dato > 0)
		{
			estadoaux = verificador_de_tramas(dato, "ERROR", estadoaux);
			estado = verificador_de_tramas(dato, mensaje,estado);
//			UART0_pushTx(dato);
		}
	}
	if(estadoaux == strlen_error)
	{
		LED_AZUL_OFF;
		LED_ROJO_ON;
		LED_VERDE_OFF;
		return 0;
	}
	else
		return 1;
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_Estado_Conexion(uint8_t dato)
 	\brief Maquina de estado principal del ESP8266
 	\details Pone un led verde en caso de estar conectado y uno azul en caso de no estarlo
 	\param uint8_t dato: dato que envia el ESP8266 a traves de la funcion UART1_popRx()
	\return void
*/
void ESP8266_Estado_Conexion(uint8_t dato)
{
	static uint16_t estado_aux = ESTADO0;
	static uint16_t estado_fail = ESTADO0;
	switch (estado_conexion)
	{
		case CONECTADO:

		 	Verificar_Mov_Auto(dato);
			ESP8266_finalizo_Envio(dato);

			estado_aux = verificador_de_tramas(dato, "CLOSED", estado_aux);
			estado_fail = verificador_de_tramas(dato, "FAIL", estado_fail);		//En caso de cerrado forzoso por el lado de la aplicacion
			if(estado_aux == strlen("CLOSED") || estado_fail == strlen("FAIL"))
			{
				estado_conexion = DESCONECTADO;
				estado_aux = ESTADO0;
				estado_fail = ESTADO0;

				modo = MANUAL;

				t_Envio_Distancia_stop();
				RESET_DISTANCIA;

				LED_AZUL_ON;
				LED_ROJO_OFF;
				LED_VERDE_OFF;
			}


			break;
		case DESCONECTADO:

			estado_aux = verificador_de_tramas(dato, ",CONNECT", estado_aux);
//    		UART0_pushTx(dato);
			if(!estado_aux)
				id = dato;
			if(estado_aux == strlen(",CONNECT"))
			{
				estado_conexion = CONECTADO;
				estado_aux = ESTADO0;
				estado_fail = ESTADO0;

				RESET_DISTANCIA;
				t_Envio_Distancia();

				LED_AZUL_OFF;
				LED_ROJO_OFF;
				LED_VERDE_ON;

			}
			break;
	}



}

/**
 	\file ESP8266.c
 	\fn   void Verificar_datos(void)
 	\brief Funcion encargada de repartir y verificar los datos dados por el ESP8266
 	\details Se introduce en el main
 	\param void
	\return void
*/
void Verificar_datos(void)
{
	dato = UART1_popRx();

	if(dato > 0 && dato < 255) //si es un caracter ascii legible
	{
//		UART0_pushTx(dato);	//Consola para debuggear
		ESP8266_Estado_Conexion(dato);
	}
}

/**
 	\file ESP8266.c
 	\fn   void Verificar_Mov_Auto(uint16_t dato)
 	\brief Funcion encargada de determinar el movimiento del auto
 	\param uint8_t dato: dato que envia el ESP8266 a traves de la funcion UART1_popRx()
	\return void
*/
void Verificar_Mov_Auto(uint16_t dato)
{
	// movimiento:
	// adelante: $f/ forward
	// izq avanzando: $l/ left
	// derecha avanzando: $r/ right
	// izq retrocediendo: $k/
	// derecha retrocediendo: $q/
	// atras: $b/ back
	// parar: $s/ stop
    // modo automatico ON: $o/
    // modo automatico OFF $x/
	// reset distancia: $h/

	static uint8_t estado_mov = REPOSO;
	static uint16_t letra = 0;

	if(dato > 0 && dato < 255) //si recibo un caracter ascii legible
	{
		switch(estado_mov)
		{
			case REPOSO:

				if(dato == '$')
				{
					 estado_mov = CARACTER_INICIAL;
				}

				break;

			case CARACTER_INICIAL:

				if(dato >= 'a' && dato <= 'z')// si el dato es una letra minuscula
				{
					letra = dato;
					estado_mov = CARACTER_FINAL;
				}
				else
				{
					estado_mov = REPOSO;
				}

				break;

			case CARACTER_FINAL:

				if(dato == '/')
				{
					PERMITIR_MOVIMIENTO;

					switch(letra)
					{
						case MOVER_ADELANTE:
//							EnviarString("El auto avanza\r\n");
							accion = AVANZO;
							break;

						case MOVER_IZQUIERDA_AV:
//							EnviarString("El auto avanza izq\r\n");
							accion = GIRO_IZQUIERDA_AV;
							break;

						case MOVER_DERECHA_AV:
//							EnviarString("El auto avanza derecha\r\n");
							accion = GIRO_DERECHA_AV;
							break;

						case MOVER_IZQUIERDA_RE:
//							EnviarString("El auto retrocede izq\r\n");
							accion = GIRO_IZQUIERDA_RE;
							break;

						case MOVER_DERECHA_RE:
//							EnviarString("El auto retrocede derecha\r\n");
							accion = GIRO_DERECHA_RE;
							break;

						case MOVER_ATRAS:
//							EnviarString("El auto va hacia atras\r\n");
							accion = RETROCEDO;
							break;

						case FRENAR:
//							EnviarString("El auto se detiene\r\n");
							accion = FRENO;
							break;

						case MOV_AUTOMATICO_ON:
//							EnviarString("Cambiando a modo automatico\r\n");
							modo = AUTOMATICO;
							break;

						case MOV_AUTOMATICO_OFF:
//							EnviarString("Cambiando a modo manual\r\n");
							modo = MANUAL;
							break;

						case RESETEAR_DISTANCIA:
//							EnviarString("Se reseteo la distancia\r\n");
							RESET_DISTANCIA;
							break;

						default:
							estado_mov = REPOSO;
					}
				}
				else
				{
					estado_mov = REPOSO;
				}

				letra = 0; //reinicio letra

				break;

			default:
				estado_mov = REPOSO;
		}
	}
}

/**
 	\file ESP8266.c
 	\fn   void Reenvio_CWMODE(void)
 	\brief Reenvia el primer mensaje
 	\param void
	\return void
*/
void Reenvio_CWMODE(void)
{
	COMANDO_CWMODE;
	LED_ROJO_NOT;
	t_Repito_CWMODE();
}

/**
 	\file ESP8266.c
 	\fn   void Actualizo_mensaje(uint8_t *p_aux)
 	\brief Actualiza el mensaje que se quiere mandar
 	\param uint8_t *p_aux: nuevo mensaje
	\return void
*/
void Actualizo_mensaje(uint8_t *p_aux)
{
	mensaje[0] = p_aux[0];
	mensaje[1] = p_aux[1];
	mensaje[2] = p_aux[2];
	mensaje[3] = p_aux[3];
	mensaje[4] = p_aux[4];
	mensaje[5] = p_aux[5];
	mensaje[6] = p_aux[6];
}

/**
 	\file ESP8266.c
 	\fn   void ESP8266_Envio_Movimiento(uint8_t mov)
 	\brief Funcion encargada de envia el movimiento del auto
 	\param uint8_t mov: codigo del movimiento del auto
	\return void
*/
void ESP8266_Envio_Movimiento(uint8_t mov)
{
	// movimiento:
	// adelante: $f/ forward
	// izq avanzando: $l/ left
	// derecha avanzando: $r/ right
	// izq retrocediendo: $k/
	// derecha retrocediendo: $q/
	// atras: $b/ back
	// parar: $s/ stop

	uint8_t p_aux[BUFFER_SIZE];

	if(estado_mensaje == DESOCUPADO)
	{
		memset(p_aux, '0', BUFFER_SIZE);

		p_aux[BUFFER_SIZE-3] = '$';
		p_aux[BUFFER_SIZE-2] = mov;
		p_aux[BUFFER_SIZE-1] = '/';

		Actualizo_mensaje(p_aux);
		ESP8266_comienzo_Envio('7');
	}

}
