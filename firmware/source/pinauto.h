//#define Funcion_del_Pin Puerto, Pin
// E->entrada S->salida

#define PIN_TX_esp          9
#define PIN_RX_esp          8
#define PIN_Infrarrojo      16
#define PIN_Ultra_Echo      27
#define PIN_Ultra_Trig      26
#define PIN_Servo           17
#define PIN_Motor_A_1       15
#define PIN_Motor_A_2       0
#define PIN_Motor_B_1       1
#define PIN_Motor_B_2       14
#define PIN_Motor_Potencia1 20
#define PIN_Motor_Potencia2 21


//Tx y Rx del ESP8266 01
//Hay q chequear con el programa de Gabi si esta bien o mal el orden del tx y el rx
#define TX_esp 0, PIN_TX_esp 
#define RX_esp 0, PIN_RX_esp 

//Sensor infrarrojo para conteo de vueltas de la rueda
#define E_Infrarrojo 0, PIN_Infrarrojo

//Sensor Ultrasonico
//La coneccion no esta echa para conectrar de una el sensor al pcb (revisar antes de colocar)
#define E_Ultra_Echo 0, PIN_Ultra_Echo 
#define S_Ultra_Trig 0, PIN_Ultra_Trig

//Salida del servo PWM
#define S_Servo 0, PIN_Servo

//Motor Puente H
//Rueda Izquierda
#define S_Motor_A_1 0, PIN_Motor_A_1 
#define S_Motor_A_2 0, PIN_Motor_A_2

// Rueda Derecha
#define S_Motor_B_1 0, PIN_Motor_B_1 
#define S_Motor_B_2 0, PIN_Motor_B_2

//Protencia de Ambas ruedas
//La potencia se puede manejar con una señal PWM en caso de no querer hacer una señar PWM puede poner un 1
//es IMPORTATE poner este pin en 1 para encender el motor 
#define S_Motor_Potencia1 0, PIN_Motor_Potencia1
#define S_Motor_Potencia2 0, PIN_Motor_Potencia2

//Leds
#define LEDGRN 1, 0
#define LEDBLUE 1, 1
#define LEDRED 1, 2

//Interrupciones externas
#define ECHO_INTERRUPT		    0

#define FALSE   0
#define TRUE    1
