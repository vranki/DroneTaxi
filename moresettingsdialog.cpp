#include "moresettingsdialog.h"
#include "ui_moresettingsdialog.h"

MoreSettingsDialog::MoreSettingsDialog(QWidget *parent, QSettings *s) :
        QDialog(parent), settings(s),
        ui(new Ui::MoreSettingsDialog)
{
    ui->setupUi(this);
    ui->padCountSpinner->setValue(settings->value("pad_count", 3).toInt());
    ui->controlSensitivityCombo->setCurrentIndex(settings->value("sensitivity", 1).toInt());
    ui->enableVideoCheckBox->setChecked(settings->value("enable_video", true).toBool());
    ui->frameSkipSpinBox->setValue(settings->value("frameskip", 1).toInt());
}

MoreSettingsDialog::~MoreSettingsDialog()
{
    settings->setValue("pad_count", ui->padCountSpinner->value());
    settings->setValue("sensitivity", ui->controlSensitivityCombo->currentIndex());
    settings->setValue("enable_video", ui->enableVideoCheckBox->isChecked());
    settings->setValue("frameskip", ui->frameSkipSpinBox->value());
    delete ui;
}
