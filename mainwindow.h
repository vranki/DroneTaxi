#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "video.h"
#include "dronecontrol.h"
#include "joystick.h"
#include "gamesettingsdialog.h"
#include "gamemodedronetaxi.h"
#include "gamemoderace.h"
#include "gamemodefreeflight.h"
#include "intro.h"

#define STD_ALTITUDE 1000
#define VERTICAL_VELOCITY 50
#define PITCH_SPEED 250.0f
#define ROLL_SPEED 180.0f
#define YAW_SPEED 200.0f

#define FINAL_ALT 280
#define AUTOLAND_ALT 300

namespace Ui {
    class MainWindow;
}

/**
 This class implements the god object antipattern and handles
 very many things.
  */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void setFlightMode(FlightMode newMode);
    void flyToggled();
    void navDataChanged();
    void landed();
    void panicButton();
    void say(QString text, int voice=0, int pitch=10, int speed=160);
    void gameOver();
    void viewBottomCamera(bool bc);
    void enableControls(bool enabled);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    void changeEvent(QEvent *e);
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void keyReleaseEvent ( QKeyEvent * event );
private slots:
    void joystickPositionChanged(QPointF pos, float z);
    void startGame(int gameMode, int time, float sensitivity, int pads, bool videoEnabled);
    void droneHasDied();
    void panicRecovery();
    void introFinished();
private:
    void firstDataReceived();
    void reset();
    void killCurrentGame();
    Ui::MainWindow *ui;
    QGraphicsScene scene;
    DroneVideo video;
    DroneControl control;
    Joystick joystick;
    bool controlsEnabled;
    int emergencyPressed;
    QTimer landTimer, droneAliveTimer, panicRecoveryTimer;
    FlightMode flightMode; // 0=landed, 1= flying, 2=landing, 3= final landing
    bool dataReceived, batteryWarned;
    GameSettingsDialog *gameSettingsDialog;
    float controlSensitivity;
    GameMode *gameMode;
    QProgressDialog connectionProgress;
    Intro *intro;
};

#endif // MAINWINDOW_H
