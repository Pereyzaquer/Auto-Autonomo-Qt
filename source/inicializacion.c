/*
 * inicializacion.c
 *
 *  Created on: 22 dic. 2021
 *      Author: franco
 */
#include "inicializacion.h"

extern uint8_t ServoYaGiro;

void Inicializacion(void)
{
	LED_RED_ON();

    SysTickInic(SystemCoreClock/400);

    //Habilito clock para el GPIO puerto 0
    CLOCK_EnableClock(kCLOCK_Gpio0);
	//SYSCON->SYSAHBCLKCTRL0 |= (1 << SYSCON_SYSAHBCLKCTRL0_GPIO0_SHIFT);

    //Inicio el CTIMER
    Inicializar_CTIMER();

    ConfigurarServo();
    ConfigurarMotorPotencia();

    //Enciendo el PWM para el servo y el control de la velocidad de los motores
    m_EncenderCTIMER();

    //Lo enderezo para que comienze mirando al frente
    m_GirarFrente();

    //Hago tiempo para que el servo se pueda enderezar
	ServoYaGiro = FALSE;
	t_GirarServo();

	while (!ServoYaGiro)
		TimerEvent();

    ConfigurarUltrasonido();

    ESP8266_inicializar();
    PERMITIR_MOVIMIENTO;

    LED_RED_OFF();
    LED_BLUE_ON();
}
