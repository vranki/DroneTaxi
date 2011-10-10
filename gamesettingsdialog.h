#ifndef GAMESETTINGSDIALOG_H
#define GAMESETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include "moresettingsdialog.h"

namespace Ui {
    class GameSettingsDialog;
}

class GameSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameSettingsDialog(QWidget *parent = 0);
    ~GameSettingsDialog();
private slots:
    void moreSettings();
    void quitGame();
    void closed();
    void help();
signals:
    void startGame(int gameMode, int time, float sensitivity, int pads, bool video);

private:
    Ui::GameSettingsDialog *ui;
    QSettings settings;
};

#endif // GAMESETTINGSDIALOG_H
