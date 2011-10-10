#ifndef GAMEMODEFREEFLIGHT_H
#define GAMEMODEFREEFLIGHT_H

#include <QObject>
#include <gamemode.h>

class GameModeFreeFlight : public GameMode
{
    Q_OBJECT
public:
    explicit GameModeFreeFlight(QObject *parent = 0);
    virtual void begin();
    virtual void flightModeChanged(FlightMode newMode);

signals:

public slots:
private:
    bool hasflown;
};

#endif // GAMEMODEFREEFLIGHT_H
