#ifndef STEPDIALOG_H
#define STEPDIALOG_H

#include <QDialog>
#include "sbus_settings.h"

namespace Ui {
class StepDialog;
}

class StepDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StepDialog(QWidget *parent, SequenceStep &st, bool out4Allowed);
    ~StepDialog();
private slots:
    void on_StepDialog_accepted();
    void on_servoStep_clicked(bool checked);
    void on_delayStep_clicked(bool checked);
    void on_testButton_toggled(bool checked);
    void on_endPosition_valueChanged(int arg1);
    void on_okButton_clicked();
    void on_cancelButton_clicked();
private:
    Ui::StepDialog *ui;
    SequenceStep &step;
    bool allowOut4;
signals:
    void setServo(int16_t value);
};

#endif // STEPDIALOG_H
