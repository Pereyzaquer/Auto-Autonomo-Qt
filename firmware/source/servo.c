#include "servo.h"

/**
 	\file		servo.c
 	\fn			void GirarServo(uint8_t angulo)
 	\brief		Gira el servo al angulo pasado como argumento
 	\details	Modifica el ancho de pulso del PWM de modo tal que el servo gire el angulo correspondiente
 	\param		angulo: numero entre 0 y 180 que servira como nuevo angulo en el que se ubique el servo
	\return		void
*/
void GirarServo(uint8_t angulo)
{
    uint32_t T_on = TIEMPO_DE_PULSO(angulo);

    //Modifico el tiempo de pulso del PWM lo que corresponda para girar al servo
    Modificar_MatchValue_PWM(CTIMER0_SERVO_CHANNEL, PERIODO_SERVO - T_on);
}

/**
 	\file		servo.c
 	\fn			void ConfigurarServo(void)
 	\brief		Funcion que configura el LPC para que funcione el servo
 	\details	Configura el CTIMER para poder generar el PWM necesario para girar el servo
 	\param  	void
	\return 	void
*/
void ConfigurarServo(void)
{
    /* Enables clock for switch matrix.: enable */
    CLOCK_EnableClock(kCLOCK_Swm);

    /* TimerMatchChannel0 connect to PIN_SERVO */
    SWM_SetMovablePinSelect(SWM0, kSWM_T0_MAT_CHN0, PIN_SERVO);

    /* Disable clock for switch matrix. */
    CLOCK_DisableClock(kCLOCK_Swm);

    /*
        PWM SERVO channel of CTIMER0 peripheral initialization.
        Inicializo el periodo en 1.5ms para que comienze mirando al frente, a pesar de que esto se ira modificando en cada instante
    */
    CTIMER_SetupPwmPeriod(CTIMER, CTIMER0_PWM_PERIOD_CHANNEL, CTIMER0_SERVO_CHANNEL, CTIMER0_PWM_PERIOD, SERVO_DUTY_INICIAL, false);
}
