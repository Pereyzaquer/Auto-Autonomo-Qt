#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication Interfaz(argc, argv);
    MainWindow w;

    QFile styleSheetFile("../Darkeum.qss");


    styleSheetFile.open(QFile::ReadOnly);

    QString styleSheet = QLatin1String(styleSheetFile.readAll());

    Interfaz.setStyleSheet(styleSheet);

    //Agrego el logo a la aplicacion
    Interfaz.setWindowIcon(QIcon("../LogoUTN"));

    w.show();

    return Interfaz.exec();
}
