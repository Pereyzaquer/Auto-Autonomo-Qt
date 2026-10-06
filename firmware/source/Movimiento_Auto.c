#include "Movimiento_Auto.h"
#include "timer.h"
#include "ESP8266.h"

uint8_t GiroCompleto, ServoYaGiro, YaSePuedeMedir, MedicionTimeout;
extern volatile uint32_t distancia;
extern volatile uint8_t AnguloServo, MedicionCompleta;
extern volatile uint8_t accion;
volatile uint8_t mov_Anterior = FRENAR;
volatile uint32_t distanciaDerecha, distanciaIzquierda;

/**
 	\file		Movimiento_Auto.c
 	\fn			void Movimiento_Auto(uint8_t modo)
 	\brief		Funcion encargada de controlar todo lo referido al movimiento del auto
 	\details	Si el modo es AUTOMATICO, la funcion actua como una Maquina de estados encargandose de mover
 				el auto hacia donde haya espacio y se encarga de todas las temporizaciones necesarias.
 				Si el modo es MANUAL, la funcion ejecutara las acciones necesarias para moverse como el usuario
 				indique por la GUI usando la variable global 'accion'
 	\param  	modo: modo operativo del auto, puede tomar el valor MANUAL o AUTOMATICO.
 				AUTOMATICO: el auto se controla automaticamente esquivando obstaculos que se le cruzen
 				MANUAL: el auto es controlado desde la interfaz grafica de la computadora
	\return 	void
*/
void Movimiento_Auto(uint8_t modo)
{
	static uint8_t estado = REPOSO_AUTO, modoAnterior = AUTOMATICO;

	if (modo == AUTOMATICO)
	{
		if (modoAnterior == MANUAL)
		{
			//Si acabo de pasar del modo manual al automatico, comienzo el auto desde el reposo
			DETENER_AUTO;
			DETECCION_FRONTAL;
			AnguloServo = FRENTE;

			ServoYaGiro = FALSE;
			t_GirarServo();
			estado = GIRANDO_SERVO;
		}

		switch (estado)
		{
			case REPOSO_AUTO:
//				APAGAR_SERVO;
				GiroCompleto = FALSE;
				ServoYaGiro = FALSE;

				/*
				 * Debe haber un minimo de 50ms entre medicion y medicion por recomendacion
				 * del fabricante del ultrasonido. Con 100ms (1/10 segundos) es suficiente
				 */
				t_EsperoAMedir();
				YaSePuedeMedir = FALSE;
				estado = ESPERO_A_MEDIR;
				break;

			case ESPERO_A_MEDIR:

				if (YaSePuedeMedir)
				{
					m_IniciarMedicion();

					/*
					 * Enciendo un timer para esperar un tiempo maximo a que llegue el sonido.
					 * Si se cumple el tiempo maximo, vuelvo a medir
					 */
					t_MedicionTimeout();
					MedicionTimeout = FALSE;
					YaSePuedeMedir = FALSE;

					//Esta variable es modificada en la ISR del ultrasonido al terminar la medicion
					MedicionCompleta = FALSE;
					estado = DECIDO_ACCION;
				}

				break;

			case DECIDO_ACCION:

				if (MedicionTimeout)
				{
					//Apago el trig ya que si llegue a este punto es porque no se apago al llegar sonido
					m_FrenarMedicion();
					MedicionTimeout = FALSE;
					MedicionCompleta = FALSE;

					/*
					 * Si se acabo el tiempo de medicion es porque no tengo ningun objeto para
					 * donde estoy mirando, o esta muy lejos. Por lo que el auto puede moverse
					 */
					switch (AnguloServo)
					{
						case FRENTE:
							//Si hay suficiente espacio de frente para avzanzar, avanzo
							AVANZAR_AUTO;

							//Vuelvo a medir
							estado = REPOSO_AUTO;
							break;

						case DERECHA:
							distanciaDerecha = DISTANCIA_FUERA_DE_LIMITE;

							//Si ya mire hacia la derecha, debo mirar ahora para la izquierda
							DETECCION_IZQUIERDA;
							AnguloServo = IZQUIERDA;

							ServoYaGiro = FALSE;
							t_GirarServo();
							estado = GIRANDO_SERVO;
							break;

						case IZQUIERDA:
							distanciaIzquierda = DISTANCIA_FUERA_DE_LIMITE;
							Decido_Movimiento();
							estado = GIRANDO_AUTO;
							break;
					}
				}
				else if (MedicionCompleta)
				{
					MedicionCompleta = FALSE;
					t_MedicionTimeout_stop();

					switch (AnguloServo)
					{
						case FRENTE:

							if (distancia >= DISTANCIA_MAX_OBJETO)
							{
								//Si hay suficiente espacio de frente para avzanzar, avanzo
								AVANZAR_AUTO;

								//Vuelvo a medir
								estado = REPOSO_AUTO;
							}
							else
							{
								DETENER_AUTO;

								//Miro primero a la derecha
								DETECCION_DERECHA;
								AnguloServo = DERECHA;

								ServoYaGiro = FALSE;
								t_GirarServo();
								estado = GIRANDO_SERVO;
							}

							break;

						case DERECHA:
							distanciaDerecha = distancia;

							//Ahora miro a la izquierda
							DETECCION_IZQUIERDA;
							AnguloServo = IZQUIERDA;

							ServoYaGiro = FALSE;
							t_GirarServo();
							estado = GIRANDO_SERVO;
							break;

						case IZQUIERDA:
							distanciaIzquierda = distancia;
							Decido_Movimiento();
							estado = GIRANDO_AUTO;
							break;
					}
				}

				break;

			case GIRANDO_AUTO:

				if (GiroCompleto)
				{
					//Freno las ruedas
					DETENER_AUTO;

					//Una vez que gire, debo enderezar el servo
					DETECCION_FRONTAL;
					AnguloServo = FRENTE;

					ServoYaGiro = FALSE;
					t_GirarServo();

					GiroCompleto = FALSE;
					estado = GIRANDO_SERVO;
				}

				break;

			case GIRANDO_SERVO:

				if (ServoYaGiro)
				{
//					APAGAR_SERVO;
					ServoYaGiro = FALSE;
					estado = REPOSO_AUTO;
				}

				break;

			default: estado = REPOSO_AUTO;
		}
	}
	else if (modo == MANUAL)
	{
		if (modoAnterior == AUTOMATICO)
		{
			//Si acabo de pasar del modo automatico al manual, no voy a usar ni el servo ni el ultra
//			APAGAR_SERVO;

			//Freno el auto;
			DETENER_AUTO;

			//Freno cualquier timer que este corriendo aun
			t_EsperoAMedir_stop();
			t_GirarAuto_stop();
			t_GirarServo_stop();
			t_MedicionTimeout_stop();
		}

		switch (accion)
		{
			case AVANZO:
				AVANZAR_AUTO;
				break;

			case GIRO_DERECHA_AV:
				GIRAR_DERECHA;
				break;

			case GIRO_IZQUIERDA_AV:
				GIRAR_IZQUIERDA;
				break;

			case GIRO_DERECHA_RE:
				RETROCEDER_IZQUIERDA;
				break;

			case GIRO_IZQUIERDA_RE:
				RETROCEDER_DERECHA;
				break;

			case RETROCEDO:
				RETROCEDER_AUTO;
				break;

			case FRENO:
				DETENER_AUTO;
				break;
		}

		//Pongo esto para que no se quede todo el tiempo realizando la misma accion
		accion = IDLE;
	}

	modoAnterior = modo;
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void girando_handler(void)
 	\brief		Handler del timer usado al girar el auto en cualquier direccion
 	\details	-
 	\param  	void
	\return 	void
*/
void girando_handler(void)
{
	GiroCompleto = TRUE;
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void girar_servo_handler(void)
 	\brief		Handler del timer usado al girar el servo
 	\details	-
 	\param  	void
	\return 	void
*/
void girar_servo_handler(void)
{
	ServoYaGiro = TRUE;
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void medicion_handler(void)
 	\brief		Handler del timer usado para esperar entre mediciones
 	\details	-
 	\param  	void
	\return 	void
*/
void medicion_handler(void)
{
	YaSePuedeMedir = TRUE;
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void timeout_handler(void)
 	\brief		Handler del timer que sirve para establecer un tiempo maximo de espera del sonido tras una medicion
 	\details	-
 	\param  	void
	\return 	void
*/
void timeout_handler(void)
{
	MedicionTimeout = TRUE;
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Auto_avanzar(void)
 	\brief		Avanza el auto y comunica la accion al ESP8266
 	\details	Avanza encendiendo los 2 motores hacia adelante a la vez.
 				Si el auto estaba avanzando previamente, la funcion no hace nada
 	\param  	void
	\return 	void
*/
void Auto_avanzar(void)
{
	if(mov_Anterior != MOVER_ADELANTE)
	{
		mov_Anterior = MOVER_ADELANTE;
		Motor_Avanzar();
		ESP8266_Envio_Movimiento(MOVER_ADELANTE);
	}
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Auto_retroceder(void)
 	\brief		Retrocede el auto y comunica la accion al ESP8266
 	\details	Retrocede encendiendo los 2 motores hacia atras a la vez.
 				Si el auto estaba retrocediendo previamente, la funcion no hace nada
 	\param  	void
	\return 	void
*/
void Auto_retroceder(void)
{
	if(mov_Anterior != MOVER_ATRAS)
	{
		mov_Anterior = MOVER_ATRAS;
		Motor_Retroceder();
		ESP8266_Envio_Movimiento(MOVER_ATRAS);
	}
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Auto_detener(void)
 	\brief		Detiene el auto y comunica la accion al ESP8266
 	\details	Detiene los 2 motores del auto a la vez.
 				Si el auto estaba detenido previamente, la funcion no hace nada
 	\param  	void
	\return 	void
*/
void Auto_detener(void)
{
	if(mov_Anterior != FRENAR)
	{
		mov_Anterior = FRENAR;
		Motor_Detener();
		ESP8266_Envio_Movimiento(FRENAR);
	}
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Auto_girar_derecha(void)
 	\brief		Gira el auto hacia la derecha y comunica la accion al ESP8266
 	\details	Gira a la derecha encendiendo el motor izquierdo hacia adelante.
 				Si el auto estaba girando a la derecha previamente, la funcion no hace nada
 	\param  	void
	\return 	void
*/
void Auto_girar_derecha(void)
{
	if(mov_Anterior != MOVER_IZQUIERDA_AV)
	{
		mov_Anterior = MOVER_DERECHA_AV;
		Motor_izquierda_Avanzo();
		ESP8266_Envio_Movimiento(MOVER_DERECHA_AV);
	}
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Auto_girar_izquierda(void)
 	\brief		Gira el auto hacia la izquierda y comunica la accion al ESP8266
 	\details	Gira a la izquierda encendiendo el motor derecho hacia adelante.
 				Si el auto estaba girando a la izquierda previamente, la funcion no hace nada
 	\param  	void
	\return 	void
*/
void Auto_girar_izquierda(void)
{
	if(mov_Anterior != MOVER_DERECHA_AV)
	{
		mov_Anterior = MOVER_IZQUIERDA_AV;
		Motor_derecha_Avanzo();
		ESP8266_Envio_Movimiento(MOVER_IZQUIERDA_AV);
	}
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Auto_retroceder_rueda_derecha(void)
 	\brief		Retrocede el auto hacia la izquierda y comunica la accion al ESP8266
 	\details	Retrocede hacia la izquierda encendiendo el motor derecho hacia atras.
 				Si el auto estaba retrocediendo hacia la izquierda previamente, la funcion no hace nada
 	\param  	void
	\return 	void
*/
void Auto_retroceder_rueda_derecha(void)
{
	if(mov_Anterior != MOVER_DERECHA_RE)
	{
		mov_Anterior = MOVER_DERECHA_RE;
		Motor_derecha_Retroceder();
		ESP8266_Envio_Movimiento(MOVER_DERECHA_RE);
	}
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Auto_retroceder_rueda_izquierda(void)
 	\brief		Retrocede el auto hacia la derecha y comunica la accion al ESP8266
 	\details	Retrocede hacia la derecha encendiendo el motor izquierdo hacia atras.
 				Si el auto estaba retrocediendo hacia la derecha previamente, la funcion no hace nada
 	\param  	void
	\return 	void
*/
void Auto_retroceder_rueda_izquierda(void)
{
	if(mov_Anterior != MOVER_IZQUIERDA_RE)
	{
		mov_Anterior = MOVER_IZQUIERDA_RE;
		Motor_izquierda_Retroceder();
		ESP8266_Envio_Movimiento(MOVER_IZQUIERDA_RE);
	}
}

/**
 	\file		Movimiento_Auto.c
 	\fn			void Decido_Movimiento(void)
 	\brief		Analiza las distancias laterales y gira el auto hacia donde hay mas espacio
 	\details	-
 	\param  	void
	\return 	void
*/
void Decido_Movimiento(void)
{
	if (distanciaDerecha >= distanciaIzquierda && distanciaDerecha >= DISTANCIA_MAX_OBJETO)
	{
		//Si hay mas espacio a la derecha, giro para la derecha
		GIRAR_DERECHA;
		t_GirarAuto();
	}
	else if (distanciaDerecha < distanciaIzquierda && distanciaIzquierda >= DISTANCIA_MAX_OBJETO)
	{
		//Si hay mas espacio a la izquierda, giro a la izquierda
		GIRAR_IZQUIERDA;
		t_GirarAuto();
	}
	else if (distanciaDerecha < DISTANCIA_MAX_OBJETO && distanciaIzquierda < DISTANCIA_MAX_OBJETO)
	{
		//Si no tengo espacio para avanzar a los costados, doy media vuelta para retroceder
		RETROCEDER_DERECHA;
		t_RetrocederAuto();
	}

	GiroCompleto = FALSE;
}
