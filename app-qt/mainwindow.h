#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define Q_OBJECT

#include <QMainWindow>
#include <QAbstractSocket>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <QStyle>
#include <string.h>
#include <QPixmap>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    protected:
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);


    public slots:
        void Conectarse_Al_Wifi(bool);
        void Deshabilitar_botones_control(void);
        void Habilitar_botones_control(void);

        void Modificar_Imagen_Auto(int);


    private slots:
        //----conexion----
        void on_boton_CONECTADO_clicked();
        void on_boton_DESCONECTADO_clicked();

        //----movimiento----
        void on_boton_adelante_pressed();
        void on_boton_adelante_released();
        void on_boton_derecha_pressed();
        void on_boton_derecha_released();
        void on_boton_izquierda_pressed();
        void on_boton_izquierda_released();
        void on_boton_frenar_clicked();
        void on_boton_atras_pressed();
        void on_boton_atras_released();

        void on_boton_automatico_ON_clicked();
        void on_boton_automatico_OFF_clicked();

        void ESP8266_escucho();

        void on_boton_reset_dis_clicked();

        void on_Boton_cerrar_ui_clicked();

private:
        bool Enviar_Trama_Mov(QString);
        void Auto_MoverAdelante(void) { Enviar_Trama_Mov(Adelante); };
        void Auto_MoverAtras(void) { Enviar_Trama_Mov(Atras); };
        void Auto_MoverIzquierda_Av(void) { Enviar_Trama_Mov(Izquierda_Av); };
        void Auto_MoverDerecha_Av(void) { Enviar_Trama_Mov(Derecha_Av); };
        void Auto_MoverIzquierda_Re(void) { Enviar_Trama_Mov(Izquierda_Re); };
        void Auto_MoverDerecha_Re(void) { Enviar_Trama_Mov(Derecha_Re); };
        void Auto_Frenar(void) { Enviar_Trama_Mov(Frenar); };
        void Auto_ResetDistancia(void) { Enviar_Trama_Mov(Reset_Distancia);};

        void Direccion_Auto(void);

        //-----declaraciones-----
        Ui::MainWindow *ui;

        QTcpSocket *socket_cliente;
        QString direccion_conectar = "192.168.4.1"; //ip default del ESP8266
        int puerto_conectar = 8888;

        QString Adelante = "$f/";
        QString Atras = "$b/";
        QString Izquierda_Av = "$l/";
        QString Derecha_Av = "$r/";
        QString Izquierda_Re = "$k/";
        QString Derecha_Re = "$q/";

        QString Frenar = "$s/";
        QString Modo_Automatico_ON = "$o/";
        QString Modo_Automatico_OFF = "$x/";
        QString Reset_Distancia = "$h/";

        int sentido;
        int giro;

        bool Mov_Automatico = false;
        bool Conex_global = false;



};

#define IDLE            0
#define DERECHA         1
#define IZQUIERDA       2
#define RETROCEDER      3
#define AVANZAR         4
#define FRENAR          5
#define SIN_SENAL       6
#define MOV_AUTO        7

#endif // MAINWINDOW_H
