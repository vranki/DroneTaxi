#include "gamemode.h"

GameMode::GameMode(QObject *parent) : QObject(parent)
{
}

void GameMode::flightModeChanged(FlightMode newMode) {

}

int GameMode::flightAlt() {
    return 500;
}

bool GameMode::gameRunning() {
    return true;
}

bool GameMode::okForLanding() {
    return true;
}

void GameMode::begin() {
}

void GameMode::padVisible(int num) {

}
