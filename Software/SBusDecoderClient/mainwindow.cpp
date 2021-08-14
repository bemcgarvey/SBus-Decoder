#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QtDebug>
#include "aboutdialog.h"
#include <QTimer>

//TODO remove qDebugs

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), port(nullptr)
{
    ui->setupUi(this);
    portLabel = new QLabel("    ");
    connectedLabel = new QLabel("Not Connected");
    versionLabel = new QLabel("");
    ui->statusbar->addWidget(portLabel);
    ui->statusbar->addWidget(connectedLabel);
    ui->statusbar->addPermanentWidget(versionLabel);
    connect(ui->menuPort, &QMenu::aboutToShow, this, &MainWindow::updatePortMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateControls()
{
    ui->tabWidget->setCurrentIndex(settings.requestedMode - 1);
    ui->out1Channel->setCurrentIndex(settings.outputs[0].channel);
    ui->out1FrameRate->setCurrentIndex(settings.outputs[0].frameRate - 1);
    ui->out1Failsafe->setCurrentIndex(settings.outputs[0].failsafeMode - 1);
    ui->out2Channel->setCurrentIndex(settings.outputs[1].channel);
    ui->out2FrameRate->setCurrentIndex(settings.outputs[1].frameRate - 1);
    ui->out2Failsafe->setCurrentIndex(settings.outputs[1].failsafeMode - 1);
    ui->out3Channel->setCurrentIndex(settings.outputs[2].channel);
    ui->out3FrameRate->setCurrentIndex(settings.outputs[2].frameRate - 1);
    ui->out3Failsafe->setCurrentIndex(settings.outputs[2].failsafeMode - 1);
    ui->out4Channel->setCurrentIndex(settings.outputs[3].channel);
    //ui->out4FrameRate->setCurrentIndex(settings.outputs[3].frameRate - 1);
    ui->out4Failsafe->setCurrentIndex(settings.outputs[3].failsafeMode - 1);
    ui->out1Reverse->setChecked(settings.outputs[0].reverse);
    ui->out1subTrim->setValue(settings.outputs[0].subTrim);
    ui->out2Reverse->setChecked(settings.outputs[1].reverse);
    ui->out2subTrim->setValue(settings.outputs[1].subTrim);
    ui->out3Reverse->setChecked(settings.outputs[2].reverse);
    ui->out3subTrim->setValue(settings.outputs[2].subTrim);
    ui->out4Reverse->setChecked(settings.outputs[3].reverse);
    ui->out4subTrim->setValue(settings.outputs[3].subTrim);
    ui->passThrough4->setChecked(settings.options & SBUS_PASSTHROUGH4);
    ui->passThrough3->setChecked(settings.options & SBUS_PASSTHROUGH3);
}

void MainWindow::updateSettings()
{
    settings.requestedMode = ui->tabWidget->currentIndex() + 1;
    settings.outputs[0].channel = ui->out1Channel->currentIndex();
    settings.outputs[0].frameRate = ui->out1FrameRate->currentIndex() + 1;
    settings.outputs[0].failsafeMode = ui->out1Failsafe->currentIndex() + 1;
    settings.outputs[1].channel = ui->out2Channel->currentIndex();
    settings.outputs[1].frameRate = ui->out2FrameRate->currentIndex() + 1;
    settings.outputs[1].failsafeMode = ui->out2Failsafe->currentIndex() + 1;
    settings.outputs[2].channel = ui->out3Channel->currentIndex();
    settings.outputs[2].frameRate = ui->out3FrameRate->currentIndex() + 1;
    settings.outputs[2].failsafeMode = ui->out3Failsafe->currentIndex() + 1;
    settings.outputs[3].channel = ui->out4Channel->currentIndex();
    settings.outputs[3].frameRate = ui->out4FrameRate->currentIndex() + 1;
    settings.outputs[3].failsafeMode = ui->out4Failsafe->currentIndex() + 1;
    settings.outputs[0].reverse = ui->out1Reverse->isChecked();
    settings.outputs[0].subTrim = ui->out1subTrim->value();
    settings.outputs[1].reverse = ui->out2Reverse->isChecked();
    settings.outputs[1].subTrim = ui->out2subTrim->value();
    settings.outputs[2].reverse = ui->out3Reverse->isChecked();
    settings.outputs[2].subTrim = ui->out3subTrim->value();
    settings.outputs[3].reverse = ui->out4Reverse->isChecked();
    settings.outputs[3].subTrim = ui->out4subTrim->value();
    if (ui->passThrough4->isChecked()) {
        settings.options |= SBUS_PASSTHROUGH4;
    } else {
        settings.options &= ~SBUS_PASSTHROUGH4;
    }
    if (ui->passThrough3->isChecked()) {
        settings.options |= SBUS_PASSTHROUGH3;
    } else {
        settings.options &= ~SBUS_PASSTHROUGH3;
    }
}

void MainWindow::updatePortMenu()
{
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    ui->menuPort->clear();
    if (portList.size() == 0) {
        ui->menuPort->addAction("No ports found");
        portLabel->setText("    ");
        connectedLabel->setText("Not Connected");
        return;
    }
    for (auto&& i : portList) {
        QAction *action = ui->menuPort->addAction(i.portName(), this, &MainWindow::comPortSelected);
        action->setCheckable(true);
        if (i.portName() == portLabel->text()) {
            action->setChecked(true);
        }
    }
}

void MainWindow::comPortSelected()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    port.reset(new QSerialPort(action->text(), this));
    if (port->open(QIODevice::ReadWrite)) {
        port->setBaudRate(QSerialPort::Baud115200);
        port->setDataBits(QSerialPort::Data8);
        port->setParity(QSerialPort::NoParity);
        port->setStopBits(QSerialPort::OneStop);
        portLabel->setText(action->text());
        connectedLabel->setText("Connected");
        ui->readPushButton->setEnabled(true);
        ui->writePushButton->setEnabled(true);
        action->setChecked(true);
        port->clear(QSerialPort::AllDirections);  //clear out extra bytes from device power on
        connect(port.get(), &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    } else {
        port.release();
        portLabel->setText("    ");
        connectedLabel->setText("Not Connected");
        ui->readPushButton->setEnabled(false);
        ui->writePushButton->setEnabled(false);
    }
}

void MainWindow::onReadyRead()
{
    int received;
    while (port->bytesAvailable() > 0) {
        switch (rxState) {
        case RX_IDLE:
            port->clear(QSerialPort::Input);
            break;
        case RX_VERSION:
            received = port->read(bufferPos, bytesNeeded);
            bytesNeeded -= received;
            bufferPos += received;
            if (bytesNeeded == 0) {
                versionLabel->setText(QString("Firmware Version %1.%2")
                        .arg(static_cast<int>(rxBuffer[0]))
                        .arg(static_cast<int>(rxBuffer[1])));
                char cmd = 's';
                port->write(&cmd, 1);
                rxState = RX_SETTINGS;
                bytesNeeded = sizeof(Settings) + 1; //Extra byte for checksum
                bufferPos = rxBuffer;
            }
            break;
        case RX_SETTINGS:
            received = port->read(bufferPos, bytesNeeded);
            bytesNeeded -= received;
            bufferPos += received;
            if (bytesNeeded == 0) {
                uint8_t chk = calcChecksum(reinterpret_cast<uint8_t *>(rxBuffer));
                if (chk == static_cast<uint8_t>(rxBuffer[sizeof (Settings)])) {
                    memcpy(&settings, rxBuffer, sizeof(Settings));
                    rxState = RX_IDLE;
                    updateControls();
                } else {
                    QMessageBox::critical(this, "sBus Decoder", "Error reading device");
                }
            }
            break;
        case RX_ACK:
            received = port->read(bufferPos, bytesNeeded);
            bytesNeeded -= received;
            bufferPos += received;
            if (bytesNeeded == 0) {
                rxState = RX_IDLE;
                if (rxBuffer[0] == ACK) {
                    QMessageBox::information(this, "sBus Decoder", "Settings saved");
                } else {
                    QMessageBox::critical(this, "sBus Decoder", "An Error occurred while saving settings.");
                }
            }
            break;
        }
    }
}


void MainWindow::on_actionExit_triggered()
{
    close();
}


void MainWindow::on_readPushButton_clicked()
{
    char tx = 'v';
    port->write(&tx, 1);
    bytesNeeded = 2;
    bufferPos = rxBuffer;
    rxState = RX_VERSION;
    QTimer::singleShot(500, this, &MainWindow::rxTimeout);
}


void MainWindow::on_out3FrameRate_currentIndexChanged(int index)
{
    ui->out4FrameRate->setCurrentIndex(index);
}


void MainWindow::on_writePushButton_clicked()
{
    char cmd = 'u';
    port->write(&cmd, 1);
    updateSettings();
    port->write(reinterpret_cast<char *>(&settings.requestedMode), sizeof(Settings));
    uint8_t chksum = calcChecksum(reinterpret_cast<uint8_t *>(&settings));
    port->write(reinterpret_cast<char *>(&chksum), 1);
    rxState = RX_ACK;
    bytesNeeded = 1;
    bufferPos = rxBuffer;
    rxBuffer[0] = 0xff; //make sure there is not a left over ACK in the buffer
    QTimer::singleShot(500, this, &MainWindow::rxTimeout);
}


void MainWindow::on_actionAbout_triggered()
{
    std::unique_ptr<AboutDialog> dlg(new AboutDialog(this));
    dlg->exec();
}


void MainWindow::rxTimeout()
{
    if (rxState == RX_VERSION || rxState == RX_SETTINGS || rxState == RX_ACK) {
        rxState = RX_IDLE;
        QMessageBox::critical(this, "sBus Decoder", "Communication timeout.  Operation failed");
    }
}


void MainWindow::on_passThrough3_stateChanged(int arg1)
{
    ui->out3Channel->setCurrentIndex(0);
    ui->output3Frame->setEnabled(!arg1);
}


void MainWindow::on_passThrough4_stateChanged(int arg1)
{
    ui->out4Channel->setCurrentIndex(0);
    ui->output4Frame->setEnabled(!arg1);
}

