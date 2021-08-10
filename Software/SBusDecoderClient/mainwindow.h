#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSerialPort>
#include "sbus_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *portLabel;
    QLabel *connectedLabel;
    QSerialPort *port;
    enum {RX_IDLE, RX_VERSION, RX_SETTINGS, RX_ACK} rxState;
    int bytesNeeded;
    char rxBuffer[100]; //TODO adjust size
    char *bufferPos;
    void updateControls(void);
    Settings settings;
private slots:
    void updatePortMenu(void);
    void comPortSelected(void);
    void onReadyRead(void);
    void on_actionExit_triggered();
    void on_readPushButton_clicked();
    void on_out3FrameRate_currentIndexChanged(int index);
};
#endif // MAINWINDOW_H
