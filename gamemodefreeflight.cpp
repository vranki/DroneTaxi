#include "gamemodefreeflight.h"

GameModeFreeFlight::GameModeFreeFlight(QObject *parent) :
    GameMode(parent)
{
    hasflown = false;
}

void GameModeFreeFlight::begin() {
    emit setTargetLabelText("Free Flight");
    say("Free Flight, land when finished");
}

void GameModeFreeFlight::flightModeChanged(FlightMode newMode) {
    if(newMode == FM_Flying) {
        hasflown = true;
    } else if(newMode == FM_Landed && hasflown) {
        emit gameOver();
    }
}

