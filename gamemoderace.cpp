#include "gamemoderace.h"

GameModeRace::GameModeRace(QObject *parent) :
    GameMode(parent), lapNumber(0), lastPad(0), raceIsOn(false)
{
}

bool GameModeRace::gameRunning() {
    return lapNumber <= numLaps;
}

void GameModeRace::flightModeChanged(FlightMode newMode) {
    if(newMode==FM_Landed) {
        emit viewBottomCamera(false);
        if(!raceIsOn && lapNumber >= numLaps)
            showGameEnd();
    } else if(newMode==FM_Flying) {
        if(!raceIsOn) {
            emit setTargetLabelText("Pad 1");
        }
        emit viewBottomCamera(true);
    }
}

bool GameModeRace::okForLanding() {
    return true;// !raceIsOn || lapNumber == 0;
}

void GameModeRace::padVisible(int num) {
    bool lapComplete = false;
    if(lastPad == numPads && num == 1) {
        lastPad=1;
        lapComplete = true;
        lapNumber++;
        if(lapNumber>numLaps) {
            emit say("race finished", 1);
            emit viewBottomCamera(false);
            totalTime = raceTimer.elapsed();
            qDebug() << Q_FUNC_INFO << "Timer stopped, total time" << totalTime;
            raceIsOn = false;
            emit enableControls(false);
            emit requestFlightMode(FM_Landing);
        } else {
            if(lapNumber==numLaps) {
                emit say("final lap", 1);
            } else {
                emit say("lap " + QString::number(lapNumber) + " of " + QString::number(numLaps), 1);
            }
        }
    } else if(num==lastPad + 1) {
        lastPad = num;
        if(lapNumber==0 && lastPad == 1) {
            qDebug() << Q_FUNC_INFO << "Timer started";
            raceTimer.start();
            raceIsOn = true;
            lapNumber = 1;
        }
        int nextPad = lastPad+1;
        if(nextPad > numPads) nextPad = 1;
        emit setTargetLabelText("Pad " + QString::number(nextPad));
        emit say("next " + QString::number(nextPad), 1);
    }
}

void GameModeRace::initGame(int laps, int pads) {
    qDebug() << Q_FUNC_INFO;
    numLaps = laps;
    numPads = pads;
    lastPad = 0;
    lapNumber = 0;
    raceIsOn = false;
}

void GameModeRace::showGameEnd() {
    QTime t;
    t = t.addMSecs(totalTime);

    QString result = QString("time %1 minutes %2 seconds").arg(t.minute()).arg(t.second());
    say(result, 1);
    result = QString("Race time: %1:%2:%3").arg(t.minute()).arg(t.second()).arg(t.msec()).arg(totalTime);
    QMessageBox *msgBox = new QMessageBox(0);
    msgBox->setText(result);
    connect(msgBox, SIGNAL(finished(int)), this, SIGNAL(gameOver()));
    msgBox->show();
}

int GameModeRace::flightAlt() {
    return 800;
}


void GameModeRace::begin() {
    emit setTargetLabelText("Take off");
    emit say("fly to pad 1 to begin race", 1);
}
