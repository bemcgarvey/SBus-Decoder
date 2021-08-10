#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), port(nullptr)
{
    ui->setupUi(this);
    portLabel = new QLabel("    ");
    connectedLabel = new QLabel("Not Connected");
    ui->statusbar->addWidget(portLabel);
    ui->statusbar->addWidget(connectedLabel);
    connect(ui->menuPort, &QMenu::aboutToShow, this, &MainWindow::updatePortMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateControls()
{
    ui->tabWidget->setCurrentIndex(settings.requestedMode - 1);
    ui->out1Channel->setCurrentIndex(settings.outputs[0].channel - 1);
    ui->out1FrameRate->setCurrentIndex(settings.outputs[0].frameRate - 1);
    ui->out1Failsafe->setCurrentIndex(settings.outputs[0].failsafeMode - 1);
    ui->out2Channel->setCurrentIndex(settings.outputs[1].channel - 1);
    ui->out2FrameRate->setCurrentIndex(settings.outputs[1].frameRate - 1);
    ui->out2Failsafe->setCurrentIndex(settings.outputs[1].failsafeMode - 1);
    ui->out3Channel->setCurrentIndex(settings.outputs[2].channel - 1);
    ui->out3FrameRate->setCurrentIndex(settings.outputs[2].frameRate - 1);
    ui->out3Failsafe->setCurrentIndex(settings.outputs[2].failsafeMode - 1);
    ui->out4Channel->setCurrentIndex(settings.outputs[3].channel - 1);
    //ui->out4FrameRate->setCurrentIndex(settings.outputs[3].frameRate - 1);
    ui->out4Failsafe->setCurrentIndex(settings.outputs[3].failsafeMode - 1);
}

void MainWindow::updatePortMenu()
{
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    ui->menuPort->clear();
    if (portList.size() == 0) {
        ui->menuPort->addAction("No ports found");
        if (port != nullptr) {
            port->close();
            delete port;
        }
        port = nullptr;
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
    if (port != nullptr) {
        port->close();
        delete port;
        port = nullptr;
    }
    port = new QSerialPort(action->text(), this);
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
        connect(port, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    } else {
        delete port;
        port = nullptr;
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
            qDebug() << "*";
            break;
        case RX_VERSION:
            received = port->read(bufferPos, bytesNeeded);
            bytesNeeded -= received;
            bufferPos += received;
            if (bytesNeeded == 0) {
                /*QMessageBox::information(this, "", QString("Verions %1.%2")
                        .arg(static_cast<int>(rxBuffer[0]))
                        .arg(static_cast<int>(rxBuffer[1])));
                */
                char cmd = 's';
                port->write(&cmd);
                rxState = RX_SETTINGS;
                bytesNeeded = 13;
                bufferPos = rxBuffer;
            }
            break;
        case RX_SETTINGS:
            received = port->read(bufferPos, bytesNeeded);
            bytesNeeded -= received;
            bufferPos += received;
            if (bytesNeeded == 0) {
                //TODO add checksum
                memcpy(&settings, rxBuffer, sizeof(Settings));
                rxState = RX_IDLE;
                updateControls();
            }
            break;
        case RX_ACK:
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
    port->write(&tx);
    bytesNeeded = 2;
    bufferPos = rxBuffer;
    rxState = RX_VERSION;
}


void MainWindow::on_out3FrameRate_currentIndexChanged(int index)
{
    ui->out4FrameRate->setCurrentIndex(index);
}

