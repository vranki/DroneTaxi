#include "gamesettingsdialog.h"
#include "ui_gamesettingsdialog.h"
#include "helpdialog.h"

GameSettingsDialog::GameSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameSettingsDialog), settings("com.symbio", "DroneTaxi")
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(closed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(quitGame()));
    connect(ui->settingsPushButton, SIGNAL(clicked()), this, SLOT(moreSettings()));
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(help()));
    ui->gameModeCombo->setCurrentIndex(settings.value("game_mode", 0).toInt());
    ui->buttonBox->button(QDialogButtonBox::Close)->setText("Quit");
}

GameSettingsDialog::~GameSettingsDialog()
{
    delete ui;
}

void GameSettingsDialog::quitGame() {
    QCoreApplication::quit();
}

void GameSettingsDialog::closed() {
    int gameTime = 2;
    float sensitivity = 1.0;

    if(ui->gameModeCombo->currentIndex()==1) {
        gameTime = 4;
    }

    if(settings.value("sensitivity", 1).toInt()==0)
        sensitivity = 0.5;
    if(settings.value("sensitivity", 1).toInt()==2)
        sensitivity = 1.2;

    bool enableVideo = settings.value("enable_video", true).toBool();
    settings.setValue("game_mode", ui->gameModeCombo->currentIndex());
    emit startGame(ui->gameModeCombo->currentIndex(), gameTime*60*1000, sensitivity, settings.value("pad_count", 3).toInt(), enableVideo);
}

void GameSettingsDialog::moreSettings() {
    MoreSettingsDialog msd(this, &settings);
    msd.exec();
}

void GameSettingsDialog::help() {
    HelpDialog h(this);
    h.exec();
}
