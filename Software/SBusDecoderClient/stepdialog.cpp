#include "stepdialog.h"
#include "ui_stepdialog.h"
#include <cmath>
#include <QDebug>
#include <QMessageBox>

StepDialog::StepDialog(QWidget *parent, SequenceStep &st) :
    QDialog(parent),
    ui(new Ui::StepDialog), step(st)
{
    ui->setupUi(this);
    ui->servoStep->setChecked(step.type == SERVO);
    ui->delayStep->setChecked(step.type == DELAY);
    ui->output->setCurrentIndex(step.output);
    ui->time->setValue(step.time / 10.0);
    ui->endPosition->setValue(round((step.position - 1024) * (150.0 / 1024.0)));
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


void StepDialog::on_buttonBox_accepted()
{
    if (ui->servoStep->isChecked() && ui->output->currentIndex() == 0) {
        QMessageBox::warning(this, QApplication::applicationName(), "Please select an output servo");
        return;
    } else {
        accept();
    }
}


void StepDialog::on_servoStep_clicked(bool checked)
{
    ui->endPosition->setEnabled(checked);
    ui->output->setEnabled(checked);
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
        ui->output->setCurrentIndex(0);
    }
}

