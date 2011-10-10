#ifndef GAMEMODERACE_H
#define GAMEMODERACE_H

#include "gamemode.h"
#include <QTime>
#include <QString>
#include <QMessageBox>
#include <QDebug>

class GameModeRace : public GameMode
{
    Q_OBJECT
public:
    explicit GameModeRace(QObject *parent = 0);
    void initGame(int laps, int pads);
    virtual bool gameRunning();
    virtual void flightModeChanged(FlightMode newMode);
    virtual bool okForLanding();
    virtual int flightAlt();
    virtual void begin();

signals:

public slots:
    virtual void padVisible(int num);

private:
    void showGameEnd();
    QTime raceTimer;
    int lapNumber, numLaps;
    int lastPad;
    int totalTime;
    bool raceIsOn;
};

#endif // GAMEMODERACE_H
