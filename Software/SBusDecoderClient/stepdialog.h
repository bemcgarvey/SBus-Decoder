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
    explicit StepDialog(QWidget *parent, SequenceStep &st);
    ~StepDialog();

private slots:
    void on_StepDialog_accepted();

    void on_buttonBox_accepted();

    void on_servoStep_clicked(bool checked);

    void on_delayStep_clicked(bool checked);

private:
    Ui::StepDialog *ui;
    SequenceStep &step;
};

#endif // STEPDIALOG_H
