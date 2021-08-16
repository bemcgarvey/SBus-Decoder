#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setOrganizationName("QES");
    QApplication::setApplicationName("SBus Decoder");
    QApplication a(argc, argv);
    MainWindow w;
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int width = screenGeometry.width();
    w.move(width / 2 - w.width() / 2, 10);
    w.show();
    return a.exec();
}
