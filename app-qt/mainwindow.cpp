#include "mainwindow.h"
#include "ui_mainwindow.h"

//movimiento:
//adelante: $f/ forward
//atras: $b/ back
//izq: $l/ left
//derecha: $r/
//parar: $s/ stop


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    socket_cliente = new QTcpSocket(this);

    connect(ui->boton_automatico_ON, SIGNAL(pressed()), this, SLOT(Deshabilitar_botones_control()));
    connect(ui->boton_automatico_OFF, SIGNAL(pressed()), this, SLOT(Habilitar_botones_control()));

    connect(socket_cliente, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    connect(socket_cliente, SIGNAL(readyRead()), this, SLOT(ESP8266_escucho()));

    ui->Mensaje_Conexion->setText("Desconectado");
    ui->Mensaje_Conexion->setPalette(QPalette(Qt::GlobalColor::red));

    //Variables para el manejo manual
    sentido = IDLE;
    giro = IDLE;

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_Boton_cerrar_ui_clicked()
{
    MainWindow::close();
}

/**
    \file mainwindow.cpp
    \fn   void MainWindow::Conectarse_Al_Wifi(bool conexion)
    \brief Se conecta o desconecta al wifi segun el parametro pasado
    \param  bool
    \return void
*/
void MainWindow::Conectarse_Al_Wifi(bool conexion)
{
    QString Conexion;

    if(conexion == true)
    {
        socket_cliente->connectToHost(QHostAddress(direccion_conectar),puerto_conectar);

        if(socket_cliente->state() == QAbstractSocket::ConnectedState || socket_cliente->waitForConnected(1000))
        {
            Conexion = "Conectado";
            ui->Mensaje_Conexion->setText(Conexion);
            ui->Mensaje_Conexion->setPalette(QPalette(Qt::GlobalColor::green));
            Conex_global = true;
            Modificar_Imagen_Auto(FRENAR);
        }
        else
        {
            Conexion = "Intente de nuevo";
            ui->Mensaje_Conexion->setText(Conexion);
            ui->Mensaje_Conexion->setPalette(QPalette(Qt::GlobalColor::yellow));
            Modificar_Imagen_Auto(SIN_SENAL);
            Conex_global = false;
        }
    }
    else
    {

        socket_cliente->disconnectFromHost();

        if(socket_cliente->state() == QAbstractSocket::UnconnectedState || socket_cliente->waitForDisconnected(1000))
        {
            Conexion = "Desconectado";
            ui->Mensaje_Conexion->setText(Conexion);
            ui->Mensaje_Conexion->setPalette(QPalette(Qt::GlobalColor::red));
            Modificar_Imagen_Auto(SIN_SENAL);
            Conex_global = false;
        }
        else
        {
            Conexion = "Intente de nuevo";
            ui->Mensaje_Conexion->setText(Conexion);
            ui->Mensaje_Conexion->setPalette(QPalette(Qt::GlobalColor::yellow));
            Modificar_Imagen_Auto(FRENAR);

        }
    }
}
/**
    \file mainwindow.cpp
    \fn   void MainWindow::Modificar_Imagen_Auto(int accion)
    \brief Modifica la imagen que aparece en la interfaz segun la accion del auto
    \param  int
    \return void
*/

void MainWindow::Modificar_Imagen_Auto(int accion)
{
    //declaro las imagenes con sus path
    QPixmap Imagen_auto_avanzar("../Imagen_Auto_Adelante.jpg");
    QPixmap Imagen_auto_frenado("../Imagen_Auto_Stop.jpg");
    QPixmap Imagen_auto_derecha("../Imagen_Auto_Derecha.jpg");
    QPixmap Imagen_auto_izquierda("../Imagen_Auto_Izquierda.jpg");
    QPixmap Imagen_auto_retroceder("../Imagen_Auto_Abajo.jpg");
    QPixmap Imagen_auto_sinsenal("../Imagen_Auto_SinSeñal.jpg");
    QPixmap Imagen_auto_mov_auto("../Imagen_Auto_MovAuto.jpg");

    //toma las dimensiones del label donde se va a poner la imagen y se acopla a este
    int alto = ui->label_Imagen_auto->height();
    int ancho = ui->label_Imagen_auto->width();

    if(Conex_global)
    {
        switch (accion)
        {
            case DERECHA:
                ui->label_Imagen_auto->setPixmap(Imagen_auto_derecha.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;
            case IZQUIERDA:
                ui->label_Imagen_auto->setPixmap(Imagen_auto_izquierda.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;
            case RETROCEDER:
                ui->label_Imagen_auto->setPixmap(Imagen_auto_retroceder.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;
            case AVANZAR:
                ui->label_Imagen_auto->setPixmap(Imagen_auto_avanzar.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;
            case FRENAR:
                ui->label_Imagen_auto->setPixmap(Imagen_auto_frenado.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;
            case SIN_SENAL:
                ui->label_Imagen_auto->setPixmap(Imagen_auto_sinsenal.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;
            case MOV_AUTO:
                 ui->label_Imagen_auto->setPixmap(Imagen_auto_mov_auto.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;
            default:
                ui->label_Imagen_auto->setPixmap(Imagen_auto_frenado.scaled(ancho,alto,Qt::KeepAspectRatio));
            break;

        }
    }
    else
    {
        ui->label_Imagen_auto->setPixmap(Imagen_auto_sinsenal.scaled(ancho,alto,Qt::KeepAspectRatio));
    }

}

/**
    \file mainwindow.cpp
    \fn   void MainWindow::on_boton_DESCONECTADO_clicked()
    \brief Se conecta al wifi desde el boton de la interfaz
    \param  void
    \return void
*/

void MainWindow::on_boton_CONECTADO_clicked()
{
    //Deshabilito el boton mientras me estoy conectando para que no sea pulsado mas de una vez
    ui->boton_CONECTADO->setDisabled(true);

    Conectarse_Al_Wifi(true);

    //Vuelvo a habilitarlo
    ui->boton_CONECTADO->setEnabled(true);
}

/**
    \file mainwindow.cpp
    \fn   void MainWindow::on_boton_DESCONECTADO_clicked()
    \brief Se desconecta del wifi desde el boton de la interfaz
    \param  void
    \return void
*/

void MainWindow::on_boton_DESCONECTADO_clicked()
{
    //Deshabilito el boton mientras me estoy conectando para que no sea pulsado mas de una vez
    ui->boton_DESCONECTADO->setDisabled(true);

    Conectarse_Al_Wifi(false);

    //Vuelvo a habilitarlo
    ui->boton_DESCONECTADO->setEnabled(true);
}
/**
    \file mainwindow.cpp
    \fn   bool MainWindow::Enviar_Trama_Mov(QString trama)
    \brief Envia la trama de la accion que debe realizar al auto
    \param  QString
    \return Bool
*/

bool MainWindow::Enviar_Trama_Mov(QString trama)
{
    QByteArray Aux = trama.toUtf8(); //convierte trama QString a qbytearray

     if(socket_cliente->state() == QAbstractSocket::ConnectedState) //si esta conectado al sv
     {
         socket_cliente->write(Aux);

         return true;
     }
     else
        return false;

}



//----mov automatico del auto----
void MainWindow::on_boton_automatico_ON_clicked()
{
    if(Conex_global)
    {
        Enviar_Trama_Mov(Modo_Automatico_ON);
        Mov_Automatico = true;
        Habilitar_botones_control();
        Modificar_Imagen_Auto(MOV_AUTO);
    }
}

void MainWindow::on_boton_automatico_OFF_clicked()
{
    if(Conex_global)
    {
        Enviar_Trama_Mov(Modo_Automatico_OFF);
        Deshabilitar_botones_control();
        Mov_Automatico = false;
        Modificar_Imagen_Auto(FRENAR);
    }
}
//---------------------

//ADELANTE
void MainWindow::on_boton_adelante_pressed()
{
    Auto_MoverAdelante();
    Modificar_Imagen_Auto(AVANZAR);
}

void MainWindow::on_boton_adelante_released()
{
    Auto_Frenar();
    Modificar_Imagen_Auto(FRENAR);
}

//DERECHA
void MainWindow::on_boton_derecha_pressed()
{
    Auto_MoverDerecha_Av();
    Modificar_Imagen_Auto(DERECHA);
}

void MainWindow::on_boton_derecha_released()
{
    Auto_Frenar();
    Modificar_Imagen_Auto(FRENAR);
}

//IZQUIERDA
void MainWindow::on_boton_izquierda_pressed()
{
    Auto_MoverIzquierda_Av();
    Modificar_Imagen_Auto(IZQUIERDA);
}

void MainWindow::on_boton_izquierda_released()
{
    Auto_Frenar();
    Modificar_Imagen_Auto(FRENAR);
}

//ABAJO
void MainWindow::on_boton_atras_pressed()
{
    Auto_MoverAtras();
    Modificar_Imagen_Auto(RETROCEDER);

}

void MainWindow::on_boton_atras_released()
{
    Auto_Frenar();
    Modificar_Imagen_Auto(FRENAR);

}

//BOTON DETENER
void MainWindow::on_boton_frenar_clicked()
{
   Auto_Frenar();
   Modificar_Imagen_Auto(FRENAR);
}

//BOTON RESET
void MainWindow::on_boton_reset_dis_clicked()
{
    Auto_ResetDistancia();
}

void MainWindow::Deshabilitar_botones_control(void)
{
    if(Mov_Automatico == true)
    {
        ui->boton_adelante->setDisabled(true);
        ui->boton_atras->setDisabled(true);
        ui->boton_derecha->setDisabled(true);
        ui->boton_izquierda->setDisabled(true);
        ui->boton_frenar->setDisabled(true);
    }
}

void MainWindow::Habilitar_botones_control(void)
{
    if(Mov_Automatico == false)
    {
        ui->boton_adelante->setDisabled(false);
        ui->boton_atras->setDisabled(false);
        ui->boton_derecha->setDisabled(false);
        ui->boton_izquierda->setDisabled(false);
        ui->boton_frenar->setDisabled(false);
    }
}


/**
    \file mainwindow.cpp
    \fn   void MainWindow::ESP8266_escucho(void)
    \brief Administra el ingreso de datos
    \param  void
    \return void
*/
void MainWindow::ESP8266_escucho(void)
{
    QTextStream dato(socket_cliente);
    QString aux = dato.readAll();

    //----------------------------- Infrarrojo
    if(aux.size() == 7)
    {
        if(aux.at(0) == '+' && aux.at(6) == '/')
        {
            aux = aux.left(6);
            aux = aux.right(5);
            ui->distancia_lineEdit->setText(aux);
        }
    //----------------------------- Movimiento automatico
        // movimiento:
        // adelante: $f/ forward
        // izq avanzando: $l/ left
        // derecha avanzando: $r/ right
        // izq retrocediendo: $k/
        // derecha retrocediendo: $q/
        // atras: $b/ back
        // parar: $s/ stop

        else if(aux.at(4) == '$' && aux.at(6) == '/')
        {
            switch (aux.at(5).toLatin1())
            {
                case 'f':
                    Modificar_Imagen_Auto(AVANZAR);
                    break;
                case 'l':
                    Modificar_Imagen_Auto(IZQUIERDA);
                      break;
                case 'r':
                    Modificar_Imagen_Auto(DERECHA);
                    break;
//                case 'k':
//                    ui->distancia_lineEdit->setText(aux);
//                    break;
//                case 'q':
//                    ui->distancia_lineEdit->setText(aux);
//                    break;
                case 'b':
                    Modificar_Imagen_Auto(RETROCEDER);
                    break;
                case 's':
                    Modificar_Imagen_Auto(FRENAR);
                    break;

            }
        }


    }


}

/**
    \file mainwindow.cpp
    \fn   void MainWindow::keyPressEvent(QKeyEvent *event)
    \brief Evento el cual adminitra las teclas pulsadas
    \param  QKeyEvent *event
    \return void
*/
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(Mov_Automatico == false && Conex_global == true)
    {
        if(event->isAutoRepeat())
        {
            return;
        }

        switch (event->key())
        {
            case Qt::Key::Key_Up:
                sentido = AVANZAR;
                break;

            case Qt::Key::Key_Right:
                giro = DERECHA;
                break;

            case Qt::Key::Key_Left:
                giro = IZQUIERDA;
                break;

            case Qt::Key::Key_Down:
                sentido = RETROCEDER;
                break;
        }

        Direccion_Auto();
    }
}


/**
    \file mainwindow.cpp
    \fn   void MainWindow::keyReleaseEvent(QKeyEvent *event)
    \brief Evento el cual adminitra las teclas que se dejaron de pulsar
    \param  QKeyEvent *event
    \return void
*/
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(Mov_Automatico == false && Conex_global == true)
    {
        //Al soltar cualquiera de ests 4 teclas, freno el auto
        if(event->isAutoRepeat())
        {
            return;
        }

        switch(event->key())
        {
            case Qt::Key::Key_Up:
                sentido = IDLE;
                break;

            case Qt::Key::Key_Right:
                giro = IDLE;
                break;

            case Qt::Key::Key_Left:
                giro = IDLE;
                break;

            case Qt::Key::Key_Down:
                sentido = IDLE;
                break;
        }

        Direccion_Auto();
    }
}

/**
    \file mainwindow.cpp
    \fn   void MainWindow::Direccion_Auto(void)
    \brief Administra el movimiento del auto en modo manual, pulsando las teclas en el teclado
    \param  void
    \return void
*/
void MainWindow::Direccion_Auto(void)
{
    if(sentido ==  RETROCEDER)
    {
        if(giro == DERECHA)
        {
            Auto_MoverDerecha_Re();
        }
        else if(giro == IZQUIERDA)
        {
            Auto_MoverIzquierda_Re();
        }
        else
        {
            Auto_MoverAtras();
        }
        Modificar_Imagen_Auto(RETROCEDER);
    }
    else if(giro || sentido)
    {
        if(giro == DERECHA)
        {
            Auto_MoverDerecha_Av();
            Modificar_Imagen_Auto(DERECHA);
        }
        else if(giro == IZQUIERDA)
        {
            Auto_MoverIzquierda_Av();
            Modificar_Imagen_Auto(IZQUIERDA);
        }
        else
        {
            Auto_MoverAdelante();
            Modificar_Imagen_Auto(AVANZAR);
        }

    }
    else
    {
        Auto_Frenar();
        Modificar_Imagen_Auto(FRENAR);
    }
}





