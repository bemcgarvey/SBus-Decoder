#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSerialPort>
#include "sbus_settings.h"
#include <memory>
#include <QListWidgetItem>
#include <QList>

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
    QList<SequenceStep> lowSteps;
    QList<SequenceStep> highSteps;
    QString itemString(const SequenceStep &step) const;
private slots:
    void updatePortMenu(void);
    void comPortSelected(void);
    void onReadyRead(void);
    void on_actionExit_triggered();
    void on_readPushButton_clicked();
    void on_out3FrameRate_currentIndexChanged(int index);
    void on_writePushButton_clicked();
    void on_actionAbout_triggered();
    void rxTimeout(void);
    void on_passThrough3_stateChanged(int arg1);
    void on_passThrough4_stateChanged(int arg1);
    void on_pwmInputRadioButton_clicked(bool checked);
    void on_sBusInputRadioButton_clicked(bool checked);
    void on_seqPassThrough_stateChanged(int arg1);
    void on_lowPlusButton_clicked();
    void on_lowSequenceList_itemDoubleClicked(QListWidgetItem *item);
    void updateLowSequenceList(int select = -1);
    void updateHighSequenceList(int select = -1);
    void on_lowXButton_clicked();
    void on_highPlusButton_clicked();
    void on_highXButton_clicked();
    void on_highSequenceList_itemDoubleClicked(QListWidgetItem *item);
    void on_lowUpButton_clicked();
    void on_lowDownButton_clicked();
    void on_rightButton_clicked();
    void on_leftButton_clicked();
    void on_highUpButton_clicked();
    void on_highDownButton_clicked();
    void on_tabWidget_currentChanged(int index);
public slots:
    void setServo(int16_t value);
};
#endif // MAINWINDOW_H
