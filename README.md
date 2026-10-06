# Auto-Autonomo-Qt

Firmware de un **auto con control remoto por WiFi y modo autónomo**, para el microcontrolador **NXP LPC845** (Cortex-M0+). Un programa en **Qt** en la computadora maneja el auto, activa el modo autónomo y muestra la distancia recorrida.

Proyecto de la materia Informática 2 (UTN FRBA), 2021. El profesor lo felicitó y lo seleccionó para presentarlo al año siguiente.

> Este repositorio contiene el **firmware del auto** (proyecto de MCUXpresso). La aplicación de Qt de la computadora no está incluida.

## Qué hace

- **Control remoto:** el auto recibe comandos por WiFi y se mueve en consecuencia (avanzar, retroceder, girar hacia cada lado, frenar).
- **Modo autónomo:** el auto avanza solo y esquiva obstáculos. Un servo mueve el sensor de ultrasonido para mirar al frente, a la derecha y a la izquierda, y el auto elige hacia dónde ir.
- **Odometría:** un encoder casero cuenta los cambios de la rueda y el auto informa a la PC la distancia recorrida.

## Hardware

| Función | Componente |
|---|---|
| Microcontrolador | NXP LPC845 |
| Alimentación | Conversor DC desde celdas 18650 |
| Motores | Puente H L293D, con PWM de potencia |
| Distancia | Ultrasonido HC-SR04 sobre un servo SG90 |
| Odometría | Encoder casero con sensor infrarrojo de presencia (rueda de 66 mm, 5 radios) |
| Comunicación | ESP8266-01 por UART, con comandos AT |

Los pines están definidos en `source/pinauto.h`.

## Comunicación

El ESP8266 se configura como **access point** (`AT+CWMODE=2`, red `ESP8266-Wifi`) y abre un **servidor TCP en el puerto 8888** (`AT+CIPSERVER=1,8888`). La computadora se conecta a esa red y abre el socket. Los comandos viajan en tramas con carácter inicial y final:

| Comando | Acción |
|---|---|
| `f` | Avanzar |
| `b` | Retroceder |
| `r` / `l` | Girar a la derecha / izquierda avanzando |
| `q` / `k` | Girar a la derecha / izquierda retrocediendo |
| `s` | Frenar |
| `o` / `x` | Modo autónomo activado / desactivado |
| `h` | Reiniciar el contador de distancia |

## Cómo está organizado el firmware

No usa sistema operativo: un lazo principal cooperativo atiende tres tareas.

```c
while(1)
{
    TimerEvent();              // temporizadores por eventos
    Verificar_datos();         // comunicación con el ESP8266
    Movimiento_Auto(modo);     // control del auto (manual o autónomo)
}
```

| Archivo | Contenido |
|---|---|
| `source/auto4.c` | Programa principal |
| `source/Movimiento_Auto.c` | Máquina de estados del modo autónomo y modo manual |
| `source/movimiento.c` | Control de los motores y su potencia (PWM) |
| `source/ultrasonido.c` | Medición con HC-SR04 usando SCTimer e interrupción de pin |
| `source/servo.c` | PWM del servo con CTIMER |
| `source/Infrarrojo.c` | Antirrebote y conteo del encoder, y envío de la distancia |
| `source/ESP8266.c` | Configuración del ESP8266 y manejo de tramas |
| `source/timer.c` | Sistema propio de temporizadores por eventos |
| `source/*UART*` | Drivers de UART con buffers circulares |
| `board/`, `drivers/`, `CMSIS/`, `device/`, `startup/` | SDK de NXP para el LPC845 |

### Modo autónomo

Es una máquina de estados: `REPOSO_AUTO → ESPERO_A_MEDIR → DECIDO_ACCION → GIRANDO_AUTO / GIRANDO_SERVO`. Si hay más de 20 cm libres al frente, avanza. Si no, se detiene, mira primero a la derecha y después a la izquierda, y gira hacia el lado con más espacio. Si ninguno de los dos tiene espacio, da media vuelta.

### Encoder

La rueda de 66 mm tiene 5 radios, así que el sensor cambia de estado 10 veces por vuelta (unos 20,7 mm por cambio). Cada cambio se confirma con un antirrebote de 5 muestras.

## Compilar

Se abre con **MCUXpresso IDE** (File → Import → Existing Projects into Workspace) y se depura con LinkServer sobre una placa LPC845-BRK. Los archivos compilados (`Debug/`) no están en el repositorio.
