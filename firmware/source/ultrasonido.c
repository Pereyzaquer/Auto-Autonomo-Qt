#include "ultrasonido.h"
#include "../../drivers/fsl_sctimer.h"

extern volatile uint32_t distancia;
extern volatile uint8_t MedicionCompleta;

/**
 	\file		ultrasonido.c
 	\fn			void Ultrasonido_Handler(void)
 	\brief		ISR de del pin ECHO del ultrasonido
 	\details	Detecta cuando se envio el sonido y cuando, actuando en consecuencia. Este no es en
 	 	 		realidad el nombre real de la funcion. Es solo un nombre de fantasia para PIN_INT0_IRQHandler
 	\param  	void
	\return 	void
*/
void Ultrasonido_Handler(void)
{
	static uint32_t to = 0, tf = 0;

	if (SONIDO_ENVIADO)
	{
		//Capturo el instante inicial en el que se pone en high el ECHO.
		to = SCTIMER_GetCOUNTValue(SCT0, SCTIMER_COUNTER);

		//Limpio el registro que detecta flanco ascendente (con un 1)
		PINT->RISE |= (1 << ECHO_INTERRUPT);
	}

	if (LLEGO_SONIDO)
	{
		//Capturo el instante en el que se pone en low el ECHO
		tf = SCTIMER_GetCOUNTValue(SCT0, SCTIMER_COUNTER);

		//Freno el timer
		m_FrenarMedicion();

		distancia = DISTANCIA(tf - to);

		//Limpio el registro que detecta flanco descendente (con un 1)
		PINT->FALL |= (1 << ECHO_INTERRUPT);

		tf = to = 0;

		//Aviso que se completo la medicion
		MedicionCompleta = TRUE;
	}
}

/**
 	\file		ultrasonido.c
 	\fn			void ConfigurarUltrasonido(void)
 	\brief		Funcion que configura el LPC para que funcione el sensor ultrasonido
 	\details	Inicializa el SCTimer y configura el LPC para generar los pulsos del trigger y contar el tiempo.
 				El prescale value es elegido de tal modo que la frecuencia de ticks del contador interno del
 				SCTimer sea 1MHz (este valor es de 17 si SystemCoreClock = 18MHz)
 	\param  	void
	\return 	void
*/
void ConfigurarUltrasonido(void)
{
    /*
     *	Por recomendacion del fabricante, el tiempo de espera entre pulsos debe ser como minimo de 50ms,
     *	lo que equivale a una frecuencia de FRECUENCIA_PWM = 20Hz para los pulsos
     */

    //Se recomienda tambien que la duracion del pulso sea de 10us
    uint32_t periodoPulso_matchValue = DURACION_PULSO_TRIG;
    uint32_t SCT0_event[2];

    const sctimer_config_t SCT0_initConfig = {

      .enableCounterUnify = true,
      .clockMode = kSCTIMER_System_ClockMode,
      .clockSelect = kSCTIMER_Clock_On_Rise_Input_0,
      .enableBidirection_l = false,
      .enableBidirection_h = false,
      .prescale_l = 17U,	//(SystemCoreClock / 1*MHz) - 1
      .prescale_h = 17U,	//(SystemCoreClock / 1*MHz) - 1
      .outInitState = 0U,
      .inputsync = 0U
    };

    /* Inicializo el SCTimer */
    SCTIMER_Init(SCT0, &SCT0_initConfig);

    //Creo un evento para que se ejecute apenas empieza el timer a contar
    SCTIMER_CreateAndScheduleEvent(SCT0_PERIPHERAL, kSCTIMER_MatchEventOnly, 0, kSCTIMER_Out_0, SCTIMER_COUNTER, &SCT0_event[DISPARO_TRIG]);

    //Que se ponga el output en high al ejecutarse al evento
    SCTIMER_SetupOutputSetAction(SCT0_PERIPHERAL, kSCTIMER_Out_0, SCT0_event[DISPARO_TRIG]);

    //Creo un evento para que se ejecute a los 10us (10 ticks del Counter_U)
    SCTIMER_CreateAndScheduleEvent(SCT0_PERIPHERAL, kSCTIMER_MatchEventOnly, periodoPulso_matchValue, kSCTIMER_Out_0, SCTIMER_COUNTER, &SCT0_event[APAGO_TRIG]);

    //Que se ponga el output en low
    SCTIMER_SetupOutputClearAction(SCT0_PERIPHERAL, kSCTIMER_Out_0, SCT0_event[APAGO_TRIG]);

    //Pin echo como entrada
    SetDIR(E_Ultra_Echo, ENTRADA);

    //Pongo como pull up el pin 27 del puerto 0, que es el echo del ultrasonido
    SetPINMODE_IN(IOCON_INDEX_PIO0_27, PULL_UP);

    /* Enables clock for switch matrix.: enable */
    CLOCK_EnableClock(kCLOCK_Swm);

    /* SCT_OUT0 connect to PIN_TRIG_ULTRASONIDO */
    SWM_SetMovablePinSelect(SWM0, kSWM_SCT_OUT0, PIN_TRIG_ULTRASONIDO);

    /* Disable clock for switch matrix. */
    CLOCK_DisableClock(kCLOCK_Swm);

    /*
     * El pin ECHO del ultrasonido se pone en high al mandarse los pulsos en TRIG y vuelve a ponerse en low al recibir el sonido.
     *
     * Llamando to: tiempo en el que se envia el pulso TRIG (el pin ECHO se pone en high), y tf: tiempo en el que se recibe el sonido (el pin
     * ECHO se pone en low), el tiempo transcurrido entre que se envio el sonido y este llego sera tf - to.
     *
     * El tiempo to en el que se envia el sonido es aproximadamente 10us despues de que el pin TRIG se pone en high.
     * Para calculat to, configuro el pin ECHO para que genere una interrupcion de flanco ascendente
     * Para calcular tf, configuro el pin ECHO para que genere una interrupcion de flanco descendente
     */

    //Habilito clocks para el periferico PINT (GPIO_INT)
    CLOCK_EnableClock(kCLOCK_GpioInt);
    //SYSCON->SYSAHBCLKCTRL0 |= (1 << SYSCON_SYSAHBCLKCTRL0_GPIO_INT_SHIFT);

    //Reseteo el PINT (con un 0) y lo vuelvo a poner en funcionamiento (con un 1)
    RESET_PeripheralReset(kGPIOINT_RST_N_SHIFT_RSTn);
    //SYSCON->PRESETCTRL0 &= ~(1 << SYSCON_SYSAHBCLKCTRL0_GPIO_INT_SHIFT);
    //SYSCON->PRESETCTRL0 |= (1 << SYSCON_SYSAHBCLKCTRL0_GPIO_INT_SHIFT);

    //Que el pin 0.27 funcione como interrupcion PININT0
    SYSCON->PINTSEL[ECHO_INTERRUPT] = PIN_Ultra_Echo;

    /*
     * Bits 0-7:	Selects the interrupt mode for each pin interrupt. Bit n
	 *				configures the pin interrupt selected in PINTSELn.
	 *				0 = Edge sensitive
	 *				1 = Level sensitive
	 *
	 * Quiero que la interrupcion se genero por flanco, osea al pasar de un nivel a otro.
	 * Elijo 'edge senstivite'
     */

    PINT->ISEL &= ~(1 << ECHO_INTERRUPT);

    //Que genere interrupcion por flanco ascendente (para capturar to)
    PINT->SIENR = (1 << ECHO_INTERRUPT);

    //Que genere interrupcion por flanco descendente (para capturar tf)
    PINT->SIENF = (1 << ECHO_INTERRUPT);

    /*
     * Habilito interrupcion por PININT0 desde el NVIC.
     * Esto debe ir al final para que no se llame el handler del PIN_INT0_IRQn por accidente y
     * que llame a alguna funcion del SCTIMER antes de ser inicializado
     * */
    EnableIRQ(PIN_INT0_IRQn);
    //NVIC->ISER[0] |= (1 << PIN_INT0_IRQn);
}
