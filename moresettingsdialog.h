#ifndef MORESETTINGSDIALOG_H
#define MORESETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class MoreSettingsDialog;
}

class MoreSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoreSettingsDialog(QWidget *parent, QSettings *settings);
    ~MoreSettingsDialog();

private:
    Ui::MoreSettingsDialog *ui;
    QSettings *settings;
};

#endif // MORESETTINGSDIALOG_H
