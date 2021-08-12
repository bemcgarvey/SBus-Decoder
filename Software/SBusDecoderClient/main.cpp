#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setOrganizationName("QES");
    QApplication::setApplicationName("SBus Decoder");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
