#include "stepdialog.h"
#include "ui_stepdialog.h"
#include <cmath>
#include <QMessageBox>
#include "mainwindow.h"

StepDialog::StepDialog(QWidget *parent, SequenceStep &st) :
    QDialog(parent),
    ui(new Ui::StepDialog), step(st), firstTest(true)
{
    ui->setupUi(this);
    ui->servoStep->setChecked(step.type == SERVO);
    ui->delayStep->setChecked(step.type == DELAY);
    ui->output->setCurrentIndex(step.output);
    ui->time->setValue(step.time / 10.0);
    ui->endPosition->setValue(round((step.position - 1024) * (150.0 / 1024.0)));
    if (step.type == DELAY) {
        ui->endPosition->setEnabled(false);
        ui->output->setEnabled(false);
        ui->testButton->setEnabled(false);
    } else {
        connect(this, &StepDialog::setServo, dynamic_cast<MainWindow *>(parent), &MainWindow::setServo);
    }
}

StepDialog::~StepDialog()
{
    delete ui;
}

void StepDialog::on_StepDialog_accepted()
{
    if (ui->servoStep->isChecked()) {
        step.type = SERVO;
    } else {
        step.type = DELAY;
    }
    step.output = ui->output->currentIndex();
    step.time = round(ui->time->value() * 10);
    step.position = round(ui->endPosition->value() * (1024.0 / 150.0)) + 1024;
    if (step.position > 2047) {
        step.position = 2047;
    }
}

void StepDialog::on_servoStep_clicked(bool checked)
{
    ui->endPosition->setEnabled(checked);
    ui->output->setEnabled(checked);
    ui->testButton->setEnabled(checked);
    if (!checked) {
        ui->output->setCurrentIndex(0);
    } else if (ui->output->currentIndex() == 0) {
        ui->output->setCurrentIndex(1);
    }
}


void StepDialog::on_delayStep_clicked(bool checked)
{
    ui->endPosition->setEnabled(!checked);
    ui->output->setEnabled(!checked);
    if (checked) {
        ui->testButton->setChecked(false);
        ui->testButton->setEnabled(false);
        ui->output->setCurrentIndex(0);
        ui->endPosition->setValue(0);
    }
}

void StepDialog::on_testButton_toggled(bool checked)
{
    if (checked) {
        if (firstTest) {
            firstTest = false;
            QMessageBox::warning(this, QApplication::applicationName(),
                                 "Before proceeding:\n"
                                 "   1) Remove the bind plug from output 4\n"
                                 "   2) Plug a servo into output 4\n"
                                 "Failure to do so can result in damage to the output pin");
        }
        int16_t position = round(ui->endPosition->value() * (1024.0 / 150.0)) + 1024;
        if (position > 2047) {
            position = 2047;
        }
        emit setServo(position);
    } else {
        emit setServo(-1);
    }
}


void StepDialog::on_endPosition_valueChanged(int arg1)
{
    if (ui->testButton->isChecked()) {
        int16_t position = round(arg1 * (1024.0 / 150.0)) + 1024;
        if (position > 2047) {
            position = 2047;
        }
        emit setServo(position);
    }
}

void StepDialog::on_okButton_clicked()
{
    ui->testButton->setChecked(false);
    if (ui->servoStep->isChecked() && ui->output->currentIndex() == 0) {
        QMessageBox::warning(this, QApplication::applicationName(), "Please select an output servo");
        return;
    } else {
        accept();
    }
}


void StepDialog::on_cancelButton_clicked()
{
    ui->testButton->setChecked(false);
    reject();
}

