#include <stdio.h>
#include "inicializacion.h"
#include "ESP8266.h"
#include "Infrarrojo.h"

volatile uint32_t distancia;
volatile uint8_t AnguloServo = FRENTE, MedicionCompleta = FALSE;
volatile uint8_t modo = MANUAL, accion = FRENO;
volatile uint8_t estado_conexion = DESCONECTADO;
volatile uint16_t contador_Infra = 0;
volatile uint8_t mensaje[BUFFER_SIZE];
volatile uint8_t estado_mensaje = DESOCUPADO;

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    Inicializacion();

    while(1)
    {
    	TimerEvent();
    	Verificar_datos();
    	Movimiento_Auto(modo);
    }

    return 0;
}
