#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSerialPort>
#include "sbus_settings.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using std::unique_ptr;

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
    QLabel *versionLabel;
    unique_ptr<QSerialPort> port;
    enum {RX_IDLE, RX_VERSION, RX_SETTINGS, RX_ACK} rxState;
    enum {ACK = 0x06, NACK = 0x15};
    int bytesNeeded;
    char rxBuffer[256];
    char *bufferPos;
    void updateControls(void);
    void updateSettings(void);
    Settings settings;
private slots:
    void updatePortMenu(void);
    void comPortSelected(void);
    void onReadyRead(void);
    void on_actionExit_triggered();
    void on_readPushButton_clicked();
    void on_out3FrameRate_currentIndexChanged(int index);
    void on_writePushButton_clicked();
    void on_actionAbout_triggered();
    void on_passThrough_stateChanged(int arg1);
    void rxTimeout(void);
};
#endif // MAINWINDOW_H
