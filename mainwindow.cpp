#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QProcess>
#include <QMessageBox>
#include "helpdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    controlsEnabled(false), flightMode(FM_Landed),
    emergencyPressed(0),  joystick(this),
    dataReceived(false), batteryWarned(0), connectionProgress(this), gameMode(0)
{
    intro = 0;
    qsrand(QTime::currentTime().msec());
    ui->setupUi(this);
    ui->videoGraphicsView->setScene(&scene);
    scene.setBackgroundBrush(QBrush(Qt::black));
    scene.addItem(&video);

    ui->rightSide->layout()->addWidget(&joystick);
    connect(ui->landButton, SIGNAL(clicked()), this, SLOT(flyToggled()));
    connect(ui->panicButton, SIGNAL(clicked()), this, SLOT(panicButton()));
    connect(&control, SIGNAL(navDataChanged()), this, SLOT(navDataChanged()));
    connect(&joystick, SIGNAL(stickPosChanged(QPointF, float)), this, SLOT(joystickPositionChanged(QPointF, float)));
    connect(&droneAliveTimer, SIGNAL(timeout()), this, SLOT(droneHasDied()));
    connect(&panicRecoveryTimer, SIGNAL(timeout()), this, SLOT(panicRecovery()));
    connect(&joystick, SIGNAL(landButtonPressed()), this, SLOT(flyToggled()));
    connect(&joystick, SIGNAL(panicButtonPressed()), this, SLOT(panicButton()));
    panicRecoveryTimer.setInterval(10000);
    panicRecoveryTimer.setSingleShot(true);
    grabKeyboard();
    resizeEvent(0);
    ui->modeLabel->hide();
    ui->landingModeLabel->hide();
    ui->rightSide->layout()->update();
    gameSettingsDialog = 0;
    connectionProgress.setLabelText("Connecting to ar.drone\n"
                                    "Make sure you are connected to ardrone wlan\n"
                                    "and drone has green lights.");
    connect(&connectionProgress, SIGNAL(canceled()), this, SLOT(close()));
    intro = new Intro(this);
    connect(intro, SIGNAL(destroyed()), this, SLOT(introFinished()));
    intro->run();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::flyToggled() {
    switch(flightMode) {
    case FM_Landed:
    {
        if(gameMode && gameMode->gameRunning())
            setFlightMode(FM_Flying);
        break;
    }
    case FM_Flying:
    {
        setFlightMode(FM_Landing);
        break;
    }
    case FM_Landing:
    {
        if(gameMode && gameMode->gameRunning())
            setFlightMode(FM_Flying);
        break;
    }
    case FM_Final:
    {
        break;
    }
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *e) {
}

void MainWindow::keyPressEvent ( QKeyEvent * event ) {
    qDebug() << Q_FUNC_INFO << event->text();
    if(event->key()==Qt::Key_Space) {
        flyToggled();
    } else if(event->key()==Qt::Key_Escape) {
        panicButton();
    } else if((event->key()==Qt::Key_Left || event->key()==Qt::Key_A)&& controlsEnabled) {
        joystick.setX(-1);
    } else if((event->key()==Qt::Key_Right || event->key()==Qt::Key_D) && controlsEnabled) {
        joystick.setX(1);
    } else if((event->key()==Qt::Key_Up || event->key()==Qt::Key_W) && controlsEnabled) {
        joystick.setY(-1);
    } else if((event->key()==Qt::Key_Down || event->key()==Qt::Key_S) && controlsEnabled) {
        joystick.setY(1);
    } else if(event->key()==Qt::Key_F) {
        if(isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    }  else if(event->key()==Qt::Key_B) {
        viewBottomCamera(!video.bottomCamera);
    }
}

void MainWindow::joystickPositionChanged(QPointF pos, float z) {
    if(!controlsEnabled) return;
    control.setPitch(pos.y()*PITCH_SPEED*controlSensitivity);
    // qDebug() << Q_FUNC_INFO  << "setting pitch: " << pos.y()*PITCH_SPEED;
    if(!joystick.supportsZ()) {
        control.setRoll(pos.x()*ROLL_SPEED*controlSensitivity);

        if(flightMode == FM_Flying) {
            control.setYaw(pos.x()*YAW_SPEED*controlSensitivity);
        }
    } else {
        control.setYaw(z*ROLL_SPEED*controlSensitivity);
        control.setRoll(pos.x()*YAW_SPEED*controlSensitivity);
    }
}

void MainWindow::keyReleaseEvent ( QKeyEvent * event ) {
    qDebug() << Q_FUNC_INFO << event->text();
    if(event->key()==Qt::Key_Left || event->key()==Qt::Key_Right
            || event->key()==Qt::Key_A|| event->key()==Qt::Key_D) {
        joystick.setX(0);
    } else if(event->key()==Qt::Key_Up || event->key()==Qt::Key_Down
              || event->key()==Qt::Key_W || event->key()==Qt::Key_S) {
        joystick.setY(0);
    }
}


void MainWindow::navDataChanged() {
    // qDebug() << "Alt" << QString::number(control.droneAltitude()) << "Bat " << control.droneVBat();
    // << "pitch" << control.dronePitch() <<
    /*
    qDebug() <<  QString::number(landingMode)
            << QString::number(currentPad)
            << QString::number(targetPad)
            << QString::number(padIsVisible);
*/
    if(!dataReceived)
        firstDataReceived();

    droneAliveTimer.setInterval(5000);
    droneAliveTimer.start();
    ui->batteryProgress->setValue(control.droneVBat());

    if(control.droneVBat() < 20 && !batteryWarned) {
        say("drone battery low", 1);
        batteryWarned = true;
    }

    //ui->targetLabel->setText(QString("%1/%2").arg(currentPad).arg(targetPad));
    //QString statusString = control.decodedStatus();
    //statusString.replace(" ","\n");
    //    ui->stateLabel->setText(statusString);

    if(flightMode == FM_Landing) {
        if(control.droneAltitude() < FINAL_ALT) { // Just land the thing
            setFlightMode(FM_Final);
        } else if(control.droneAltitude() < AUTOLAND_ALT) {
            control.setVVelocity(-VERTICAL_VELOCITY*0.5);
            ui->landingModeLabel->setText("Approach");
        } else { // land if pad visible
            if(gameMode->okForLanding()) {
                control.setVVelocity(-VERTICAL_VELOCITY);
                ui->landingModeLabel->setText("Landing");
            } else {
                if(control.droneAltitude() < gameMode->flightAlt()) {
                    control.setVVelocity(VERTICAL_VELOCITY);
                    ui->landingModeLabel->setText("Not vis");
                } else {
                    control.setVVelocity(0);
                    ui->landingModeLabel->setText("At top");
                }
            }
        }
    } else if(flightMode==FM_Flying) {
        if(control.droneAltitude() < gameMode->flightAlt()) {
            control.setVVelocity(VERTICAL_VELOCITY/2);
        } else if(control.droneAltitude() > gameMode->flightAlt()+50) {
            control.setVVelocity(-VERTICAL_VELOCITY/2);
        } else {
            control.setVVelocity(0);
        }
    }
}

void MainWindow::landed() {
    landTimer.stop();
    setFlightMode(FM_Landed);
}

void MainWindow::panicButton() {
    qDebug() << Q_FUNC_INFO << "Panic!!!";
    emergencyPressed++;
    control.setFly(false);
    if(emergencyPressed>2) {
        control.setEmergency(true);
        say("emergency", 1);
        ui->targetPadLabel->setText("Emergency");
    } else {
        say("panic", 1);
        ui->targetPadLabel->setText("Panic");
    }
    killCurrentGame();
    landed();
    controlsEnabled = true;
    control.setVVelocity(0);
    panicRecoveryTimer.start();
}

void MainWindow::setFlightMode(FlightMode newMode) {
    FlightMode oldMode = flightMode;
    flightMode = newMode;
    switch(flightMode) {
    case FM_Flying:
    {
        controlsEnabled = true;
        ui->modeLabel->setText("Flying");
        ui->landButton->setText("Land");
        control.setEmergency(false);
        control.setFly(true);
        control.setCtrlActive(true);

        break;
    }
    case FM_Landing:
    {
        controlsEnabled = true;
        ui->modeLabel->setText("Landing");
        ui->landButton->setText("Fly");
        break;
    }
    case FM_Final:
    {
        controlsEnabled = false;
        control.setYaw(0);
        control.setRoll(0);
        control.setPitch(0);
        control.setVVelocity(-VERTICAL_VELOCITY*0.6);
        if(!landTimer.isActive()) {
            landTimer.singleShot(1500,this,SLOT(landed()));
        }
        ui->modeLabel->setText("Final");
        break;
    }
    case FM_Landed:
    {
        controlsEnabled = false;
        ui->modeLabel->setText("Landed");
        ui->landButton->setText("Fly");
        control.setFly(false);
        break;
    }
    }
    if(flightMode != FM_Landing && flightMode != FM_Final)
        ui->landingModeLabel->setText("");
    if(gameMode)
        gameMode->flightModeChanged(flightMode);
}

void MainWindow::say(QString text, int voice, int pitch, int speed) {
    QProcess *espeak = new QProcess(this);
    QStringList args;
    if(voice==1) {
        args << "-v" << "!v/f5";
    } else {
        args << "-p" << QString::number(pitch);
        args << "-s" << QString::number(speed);
    }
    args << text;
    espeak->start("espeak", args, QIODevice::NotOpen);
}

void MainWindow::firstDataReceived() {
    dataReceived = true;
    ui->landButton->setEnabled(true);
    ui->panicButton->setEnabled(true);
    connectionProgress.setVisible(false);
    if(!intro)
        if(!gameSettingsDialog)
            gameOver();
}

void MainWindow::startGame(int gm, int time, float sensitivity, int pads, bool videoEnabled) {
    if(gameMode) {
        gameMode->deleteLater();
    }
    video.reset();
    control.reset();

    gameMode = 0;
    controlSensitivity = sensitivity;
    gameSettingsDialog->deleteLater();
    gameSettingsDialog = 0;
    setFlightMode(FM_Landed);
    if(gm < 2) {
        GameModeDroneTaxi *dt = new GameModeDroneTaxi(this);
        dt->initGame(time, pads);
        gameMode = dt;
    } else if(gm >=2 && gm < 4){
        GameModeRace *race = new GameModeRace(this);
        int laps = 3;
        if(gm==3)
            laps = 5;
        race->initGame(laps, pads);
        gameMode = race;
    } else {
        GameModeFreeFlight *ff = new GameModeFreeFlight(this);
        gameMode = ff;
    }
    connect(&video, SIGNAL(padVisible(int)), gameMode, SLOT(padVisible(int)));
    connect(gameMode, SIGNAL(requestFlightMode(FlightMode)), this, SLOT(setFlightMode(FlightMode)));
    connect(gameMode, SIGNAL(say(QString,int,int,int)), this, SLOT(say(QString,int,int,int)));
    connect(gameMode, SIGNAL(setTargetLabelText(QString)), ui->targetPadLabel, SLOT(setText(QString)));
    connect(gameMode, SIGNAL(gameOver()), this, SLOT(gameOver()));
    connect(gameMode, SIGNAL(viewBottomCamera(bool)), this, SLOT(viewBottomCamera(bool)));
    connect(gameMode, SIGNAL(enableControls(bool)), this, SLOT(enableControls(bool)));
    gameMode->begin();
    video.enableVideo(videoEnabled);
    enableControls(true);
}

void MainWindow::gameOver() {
    if(emergencyPressed) return;
    killCurrentGame();
    if(gameSettingsDialog)  {
        qDebug() << Q_FUNC_INFO << "who calls gameover when gamesettingsdialog already exists?";
//        Q_ASSERT(!gameSettingsDialog);
        return;
    }
    gameSettingsDialog = new GameSettingsDialog(this);
    connect(gameSettingsDialog, SIGNAL(startGame(int,int,float,int,bool)),
            this, SLOT(startGame(int,int,float,int,bool)));
    gameSettingsDialog->show();
}

void MainWindow::viewBottomCamera(bool bc) {
    control.viewBottomCamera(bc);
    video.viewBottomCamera(bc);
}

void MainWindow::enableControls(bool enabled) {
    controlsEnabled = enabled;
    if(!enabled) {
        control.setPitch(0);
        control.setRoll(0);
        control.setYaw(0);
    }
    control.setCtrlActive(enabled);
}

void MainWindow::droneHasDied() {
    droneAliveTimer.stop();
    killCurrentGame();
    if(flightMode==FM_Flying)
        setFlightMode(FM_Landing);
    QMessageBox msg(this);
    msg.setText("Connection to drone lost.\nRestart the drone and press ok to reset the application.\n"
                "I hope you didn't break it");
    msg.addButton("Quit", QMessageBox::RejectRole);
    QAbstractButton *reconnectButton = msg.addButton("Reconnect", QMessageBox::AcceptRole);
    msg.exec();
    if(msg.clickedButton()!=reconnectButton) {
        QCoreApplication::quit();
    } else {
        reset();
    }
}

void MainWindow::reset() {
    control.reset();
    video.reset();
    controlsEnabled = false;
    emergencyPressed = 0;
    killCurrentGame();
    dataReceived = false;
    batteryWarned = false;
    control.setConfFullScreen(true);
    control.setCtrlActive(false);
    enableControls(false);
    viewBottomCamera(false);
    setFlightMode(FM_Landed);
    ui->targetPadLabel->setText("Connecting..");
    connectionProgress.setVisible(true);
}

void MainWindow::killCurrentGame() {
    if(gameMode) {
        delete gameMode;
        gameMode = 0;
    }
}

void MainWindow::panicRecovery() {
    QMessageBox msg;
    if(emergencyPressed > 2) {
        msg.setText("Drone is in emergency mode.\nRestart it and close dialog.");
    } else {
        msg.setText("I hope that was a safe landing.\nClose dialog to continue.");
    }
    msg.exec();
    reset();
    gameOver();
}

void MainWindow::introFinished() {
    intro = 0;
    show();
    QSettings settings("com.symbio", "DroneTaxi");
    if(!settings.contains("help_has_been_given")) {
        HelpDialog help(this);
        help.exec();
        settings.setValue("help_has_been_given", true);
    }
    if(dataReceived) {
        showFullScreen();
        firstDataReceived();
    }
    reset();
}
