# Auto-Autonomo-Qt

Un **auto con control remoto por WiFi y modo autónomo**, con dos partes:

- **`firmware/`**: el programa del auto, para el microcontrolador **NXP LPC845** (Cortex-M0+).
- **`app-qt/`**: la **aplicación de escritorio en Qt** que maneja el auto desde la computadora.

Proyecto de la materia Informática 2 (UTN FRBA), 2021. El profesor lo felicitó y lo seleccionó para presentarlo al año siguiente.

## Qué hace

- **Control remoto:** desde la aplicación se maneja el auto con el teclado o con botones: avanzar, retroceder, girar hacia cada lado (avanzando o retrocediendo) y frenar.
- **Modo autónomo:** el auto avanza solo y esquiva obstáculos. Un servo mueve el sensor de ultrasonido para mirar al frente, a la derecha y a la izquierda, y el auto elige hacia dónde ir.
- **Odometría:** un encoder casero cuenta los cambios de la rueda y el auto informa a la aplicación la distancia recorrida.

## Cómo se conectan las dos partes

```
  Aplicación Qt (PC)  ──── WiFi ────  ESP8266-01  ──UART──  LPC845
   cliente TCP                       access point           firmware
   192.168.4.1:8888                  + servidor TCP
```

El ESP8266 se configura como **access point** (red `ESP8266-Wifi`) y abre un **servidor TCP en el puerto 8888**. La computadora se conecta a esa red y la aplicación abre un socket contra `192.168.4.1:8888` (la IP por defecto del ESP8266).

Los mensajes son tramas cortas con un carácter inicial y uno final:

| Sentido | Trama | Significado |
|---|---|---|
| PC → auto | `$f/` | Avanzar |
| PC → auto | `$b/` | Retroceder |
| PC → auto | `$r/` · `$l/` | Girar a la derecha / izquierda avanzando |
| PC → auto | `$q/` · `$k/` | Girar a la derecha / izquierda retrocediendo |
| PC → auto | `$s/` | Frenar |
| PC → auto | `$o/` · `$x/` | Modo autónomo activado / desactivado |
| PC → auto | `$h/` | Reiniciar el contador de distancia |
| auto → PC | `+NNNNN/` | Distancia recorrida (5 dígitos, en milímetros) |
| auto → PC | `$f/`, `$l/`, … | Acción que está haciendo el auto (en modo autónomo) |

## Hardware

| Función | Componente |
|---|---|
| Microcontrolador | NXP LPC845 |
| Alimentación | Conversor DC desde celdas 18650 |
| Motores | Puente H L293D, con PWM de potencia |
| Distancia a obstáculos | Ultrasonido HC-SR04 sobre un servo SG90 |
| Odometría | Encoder casero con sensor infrarrojo de presencia (rueda de 66 mm, 5 radios) |
| Comunicación | ESP8266-01 por UART, con comandos AT |

Los pines están definidos en `firmware/source/pinauto.h`.

## Aplicación Qt (`app-qt/`)

Hecha con **Qt 5 (Widgets y Network)**.

- **Conexión:** botones Conectar / Desconectar, con un indicador de estado (rojo, amarillo, verde).
- **Control manual:** botones que mandan el comando al presionarse y frenan al soltarse, y las **flechas del teclado**. Se pueden combinar: por ejemplo, flecha arriba + izquierda gira avanzando a la izquierda.
- **Modo autónomo:** un botón lo activa y otro lo desactiva. Con el modo activo, el teclado deja de mandar comandos al auto.
- **Imagen del auto:** una imagen cambia según la acción (avanzar, retroceder, girar, frenar, modo autónomo y sin señal).
- **Distancia:** muestra la distancia recorrida que informa el auto, con un botón para reiniciarla.
- **Estilo:** hoja de estilos oscura (`Darkeum.qss`) y logo de la UTN.

| Archivo | Contenido |
|---|---|
| `mainwindow.cpp` / `.h` / `.ui` | Ventana principal: conexión, envío de tramas, teclado y recepción |
| `main.cpp` | Arranque y carga del estilo |
| `Darkeum.qss` | Hoja de estilos |
| `Imagen_Auto_*.jpg`, `LogoUTN.png` | Imágenes de la interfaz |

Para compilar: abrir `TPO_Interfaz_cliente.pro` con **Qt Creator** (Qt 5). La aplicación carga `Darkeum.qss` y las imágenes con rutas relativas a una carpeta por encima del ejecutable (`../`), por lo que el ejecutable debe correr desde una carpeta de compilación dentro de `app-qt/`.

## Firmware (`firmware/`)

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

### Compilar

Se abre con **MCUXpresso IDE** (File → Import → Existing Projects into Workspace) y se depura con LinkServer sobre una placa LPC845-BRK.

## Resultados

El control remoto cumplió: el auto respondía a los comandos de la aplicación. El modo autónomo no funcionó correctamente, por el uso de sensores poco precisos y por debilidades mecánicas del auto, que estaba desbalanceado y se tiraba más hacia un lado que hacia el otro.
