#include "gamemodedronetaxi.h"

GameModeDroneTaxi::GameModeDroneTaxi(QObject *parent) : GameMode(parent), customers(0), currentPad(0), targetPad(0), customerVoicePitch(10),
    customerVoiceSpeed(160), flightMode(FM_Landed)
{
    connect(&timeoutTimer, SIGNAL(timeout()), this, SLOT(outOfTime()));
}

void GameModeDroneTaxi::initGame(int time, int pads) {
    gameLength = time;
    customers = 0;
    timeoutTimer.setInterval(time);
    timeoutTimer.start();
    QTimer::singleShot(gameLength - 60*1000, this, SLOT(minuteLeft()));
    QTimer::singleShot(gameLength - 30*1000, this, SLOT(thirtySecsLeft()));
    targetPad = 0;
    numPads = pads;
}

void GameModeDroneTaxi::targetPadReached() {
    Q_ASSERT(flightMode == FM_Landed);
    if(targetPad)
        customers++;
    do {
        targetPad = qrand() % (numPads) + 1;
    } while(targetPad == currentPad);
    qDebug() << Q_FUNC_INFO << "Target pad reached, new target: " << targetPad;
    emit setTargetLabelText("");
//    ui->modeLabel->setText("Landed");
    QTimer::singleShot(2000, this, SLOT(askNextPad()));
}

void GameModeDroneTaxi::askNextPad() {
    customerVoicePitch = qrand() % 90 + 5;
    customerVoiceSpeed = qrand() % 150 + 100;
    emit setTargetLabelText("Pad " + QString::number(targetPad));
    QString request = "Pad " + QString::number(targetPad) + ",\nplease!";
    say(request);
}

void GameModeDroneTaxi::outOfTime() {
    targetPad = 0;
    timeoutTimer.stop();
    if(flightMode == FM_Landed) {
        showGameEnd();
    } else {
        emit enableControls(false);
        emit requestFlightMode(FM_Landing);
    }
}

void GameModeDroneTaxi::minuteLeft() {
    say("one minute left", 1);
}

void GameModeDroneTaxi::thirtySecsLeft() {
    say("30 seconds left", 1);
}

bool GameModeDroneTaxi::gameRunning() {
    return timeoutTimer.isActive();
}

void GameModeDroneTaxi::flightModeChanged(FlightMode newMode) {
    FlightMode oldMode = flightMode;
    flightMode = newMode;
    if(newMode==FM_Landed) {
        if(oldMode != FM_Landed && timeoutTimer.isActive() && targetPad==currentPad && targetPad) {
            say("thanks");
            targetPadReached();
        } else if(!timeoutTimer.isActive()){
            showGameEnd();
        }
        emit viewBottomCamera(false);
    } else if(newMode==FM_Flying) {
        currentPad = 0;
        if(oldMode==FM_Landed) {
            emit say("takeoff", 1);
        } else {
            emit say("flight mode", 1);
        }
        emit viewBottomCamera(false);
    } else if(newMode== FM_Landing) {
        if(gameRunning()) {
            say("landing", 1);
        } else {
            say("game over, landing", 1);
        }

        emit viewBottomCamera(true);
    }
}

void GameModeDroneTaxi::showGameEnd() {
    QString result = QString::number(customers) + " customers transported";
    say(result, 1);
    QMessageBox *msgBox = new QMessageBox(0);
    msgBox->setText(result + "\n\nClose dialog for new game");
    connect(msgBox, SIGNAL(finished(int)), this, SIGNAL(gameOver()));
    msgBox->show();
}

void GameModeDroneTaxi::padVisible(int num) {
    if(num > 0) {
        currentPad = num;
    }
    padIsVisible = num != 0;
}

bool GameModeDroneTaxi::okForLanding() {
    return (!targetPad || (padIsVisible && currentPad == targetPad));
}

void GameModeDroneTaxi::begin() {
    targetPadReached();
}
