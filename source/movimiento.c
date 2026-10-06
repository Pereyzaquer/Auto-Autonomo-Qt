/*
 * movimiento.c
 *
 *  Created on: 9 nov. 2021
 *      Author: nicop
 */
#include "movimiento.h"
#include "string.h"
#include "PR_UART0_2021.h"


/**
 	\file		movimiento.c
 	\fn			void Motor_derecha_Avanzo(void)
 	\brief		Enciende el motor derecho
 	\details	Se enciende con el mismo sentido que Motor_Avanza()
 	\param		void
	\return		void
*/
void Motor_derecha_Avanzo(void)
{
	SetPIN(S_Motor_A_1, APAGADO);
	SetPIN(S_Motor_A_2, ENCENDIDO);

	SetPIN(S_Motor_B_1, APAGADO);
	SetPIN(S_Motor_B_2, APAGADO);
}

/**
 	\file		movimiento.c
 	\fn			void Motor_izquierda_Avanzo(void)
 	\brief		Enciende el motor izquierdo
 	\details	Se enciende con el mismo sentido que Motor_Avanza()
 	\param		void
	\return		void
*/
void Motor_izquierda_Avanzo(void)
{
	SetPIN(S_Motor_A_1, APAGADO);
	SetPIN(S_Motor_A_2, APAGADO);

	SetPIN(S_Motor_B_1, ENCENDIDO);
	SetPIN(S_Motor_B_2, APAGADO);
}

/**
 	\file		movimiento.c
 	\fn			void Motor_derecha_Retroceder(void)
 	\brief		Enciende el motor derecho
 	\details	Se enciende con el mismo sentido que Motor_Retroceder()
 	\param		void
	\return		void
*/
void Motor_derecha_Retroceder(void)
{
	SetPIN(S_Motor_A_1, ENCENDIDO);
	SetPIN(S_Motor_A_2, APAGADO);

	SetPIN(S_Motor_B_1, APAGADO);
	SetPIN(S_Motor_B_2, APAGADO);
}

/**
 	\file		movimiento.c
 	\fn			void Motor_izquierda_Retroceder(void);
 	\brief		Enciende el motor izquierdo
 	\details	Se enciende con el mismo sentido que Motor_Retroceder()
 	\param		void
	\return		void
*/
void Motor_izquierda_Retroceder(void)
{
	SetPIN(S_Motor_A_1, APAGADO);
	SetPIN(S_Motor_A_2, APAGADO);

	SetPIN(S_Motor_B_1, APAGADO);
	SetPIN(S_Motor_B_2, ENCENDIDO);
}

/**
 	\file		movimiento.c
 	\fn			void Motor_Avanzar(void)
 	\brief		Enciende el motor derecho y el izquierdo
 	\details	Se enciende de forma que el auto avance hacia adelante
 	\param		void
	\return		void
*/
void Motor_Avanzar(void)
{
	SetPIN(S_Motor_A_1, APAGADO);
	SetPIN(S_Motor_A_2, ENCENDIDO);

	SetPIN(S_Motor_B_1, ENCENDIDO);
	SetPIN(S_Motor_B_2, APAGADO);
}

/**
 	\file		movimiento.c
 	\fn			void Motor_Retroceder(void)
 	\brief		Enciende el motor derecho y el izquierdo
 	\details	Se enciende de forma que el auto retroceda
 	\param		void
	\return		void
*/
void Motor_Retroceder(void)
{
	SetPIN(S_Motor_A_1, ENCENDIDO);
	SetPIN(S_Motor_A_2, APAGADO);

	SetPIN(S_Motor_B_1, APAGADO);
	SetPIN(S_Motor_B_2, ENCENDIDO);
}


/**
 	\file		movimiento.c
 	\fn			void Motor_Detener(void)
 	\brief		Detiene los motores
 	\param		void
	\return		void
*/
void Motor_Detener(void)
{
	SetPIN(S_Motor_A_1, APAGADO);
	SetPIN(S_Motor_A_2, APAGADO);

	SetPIN(S_Motor_B_1, APAGADO);
	SetPIN(S_Motor_B_2, APAGADO);
}

/**
 	\file		movimiento.c
 	\fn			void Permitir_Movimiento(void)
 	\brief		Habilita el movimiento de los moteres
 	\details	Pone un 1 en el los pines habilitadores del L293d
 	\param		void
	\return		void
*/
void Permitir_Movimiento(void)
{
	SetPIN(S_Motor_Potencia1, ENCENDIDO);
	MoficarVelocidadMotor(POT_AJUSTADA);
}

/**
 	\file		movimiento.c
 	\fn			void Permitir_Movimiento(void)
 	\brief		Anula el movimiento de los moteres
 	\details	Pone un cero en el los pines habilitadores del L293d
 	\param		void
	\return		void
*/
void Anular_Movimiento(void)
{
	SetPIN(S_Motor_Potencia1, APAGADO);
	MoficarVelocidadMotor(POT_MIN);
}

/**
 	\file		movimiento.c
 	\fn			void ConfigurarMotorPotencia(void)
 	\brief		Configura la señar pwm del PIN_MOTOR_POTENCIA2
 	\param		void
	\return		void
*/
void ConfigurarMotorPotencia(void)
{
	/* Enables clock for switch matrix.: enable */
	CLOCK_EnableClock(kCLOCK_Swm);

	/* TimerMatchChannel1 connect to PIN_MOTOR_POTENCIA1 */
	//No se si esto va a funcionar porque estaria conectando el mismo canal a 2 pines distintos
	//SWM_SetMovablePinSelect(SWM0, kSWM_T0_MAT_CHN1, PIN_MOTOR_POTENCIA1);

	/* TimerMatchChannel1 connect to PIN_MOTOR_POTENCIA2 */
	SWM_SetMovablePinSelect(SWM0, kSWM_T0_MAT_CHN1, PIN_MOTOR_POTENCIA2);

	/* Disable clock for switch matrix. */
	CLOCK_DisableClock(kCLOCK_Swm);

	/*
	 * Inicializo el match value en 0, por lo que el ancho de pulso deberia ser igual al periodo
	 * y el motor deberia ir a maxima velocidad
	 */
	CTIMER_SetupPwmPeriod(CTIMER, CTIMER0_PWM_PERIOD_CHANNEL, CTIMER0_MOTOR_POTENCIA_CHANNEL, CTIMER0_PWM_PERIOD, 0, false);
}

/**
 	\file		movimiento.c
 	\fn			void MoficarVelocidadMotor(uint32_t T_on)
 	\brief		Modifica la velocidad de la rueda izquierda
 	\param		T_on: valor entre 0 y PERIODO_POTENCIA2 (20*mS o 20000)
				0 corresponde a la velocidad minima, 20000 corresponde a la maxima
	\return		void
*/
void MoficarVelocidadMotor(uint32_t T_on)
{
	Modificar_MatchValue_PWM(CTIMER0_MOTOR_POTENCIA_CHANNEL, PERIODO_POTENCIA2 - T_on);
}
