#ifndef GAMEMODEDRONETAXI_H
#define GAMEMODEDRONETAXI_H

#include "gamemode.h"
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

class GameModeDroneTaxi : public GameMode
{
    Q_OBJECT
public:
    explicit GameModeDroneTaxi(QObject *parent = 0);
    void initGame(int time, int pads);
    virtual bool gameRunning();
    virtual void flightModeChanged(FlightMode newMode);
    virtual bool okForLanding();
    virtual void begin();
public slots:
    virtual void padVisible(int num);
private slots:
    void outOfTime();
    void minuteLeft();
    void thirtySecsLeft();
    void askNextPad();
private:
    void targetPadReached();
    void showGameEnd();
    QTimer timeoutTimer;
    int gameLength; // msec
    int customers;
    int currentPad, targetPad;
    int customerVoicePitch; // 1-99
    int customerVoiceSpeed; // 100-250 wpm
    FlightMode flightMode;

    bool padIsVisible;
};

#endif // GAMEMODEDRONETAXI_H
