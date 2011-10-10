#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QObject>

enum FlightMode {
    FM_Landed = 0,
    FM_Flying,
    FM_Landing,
    FM_Final
};


class GameMode : public QObject
{
    Q_OBJECT
public:
    GameMode(QObject *parent);
    virtual bool gameRunning();
    virtual void flightModeChanged(FlightMode newMode);
    virtual bool okForLanding();
    virtual int flightAlt();
    virtual void begin();
signals:
    void requestFlightMode(FlightMode newMode);
    void say(QString text, int voice=0, int pitch=10, int speed=160);
    void setTargetLabelText(QString txt);
    void gameOver();
    void viewBottomCamera(bool bc);
    void enableControls(bool enabled);
public slots:
    virtual void padVisible(int num);
protected:
    int numPads; // Pad count
};

#endif // GAMEMODE_H
