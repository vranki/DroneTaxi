#ifndef DRONECONTROL_H
#define DRONECONTROL_H

#include <QGraphicsWidget>
#include <QGraphicsItem>
#include <QPainter>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QSettings>
#undef _GNU_SOURCE // just get rid of error message double definition
#include <navdata.h>

class DroneThread;

class DroneControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float pitch READ pitch WRITE setPitch NOTIFY pitchRollChanged)
    Q_PROPERTY(float roll  READ roll  WRITE setRoll NOTIFY pitchRollChanged)
    Q_PROPERTY(float yaw   READ yaw   WRITE setYaw NOTIFY pitchRollChanged)
    Q_PROPERTY(float altitude  READ altitude WRITE setAltitude)
    Q_PROPERTY(float vVelocity READ vVelocity WRITE setVVelocity NOTIFY pitchRollChanged)
    Q_PROPERTY(int   enabled   READ enabled WRITE setEnabled)
    Q_PROPERTY(bool  fly READ fly WRITE setFly)
    Q_PROPERTY(bool  ctrlActive READ ctrlActive WRITE setCtrlActive)
    Q_PROPERTY(float dronePitch READ dronePitch NOTIFY navDataChanged)
    Q_PROPERTY(float droneRoll READ droneRoll NOTIFY navDataChanged)
    Q_PROPERTY(float droneYaw READ droneYaw NOTIFY navDataChanged)
    Q_PROPERTY(float droneVBat READ droneVBat NOTIFY navDataChanged)
    Q_PROPERTY(float droneAltitude READ droneAltitude NOTIFY navDataChanged)
    Q_PROPERTY(bool  emergency READ emergency WRITE setEmergency)
    Q_PROPERTY(QString decodedStatus READ decodedStatus NOTIFY statusChanged)
    Q_PROPERTY(int pwm_motor1 READ pwm_motor1 NOTIFY navDataChanged)
    Q_PROPERTY(int pwm_motor2 READ pwm_motor1 NOTIFY navDataChanged)
    Q_PROPERTY(int pwm_motor3 READ pwm_motor1 NOTIFY navDataChanged)
    Q_PROPERTY(int pwm_motor4 READ pwm_motor1 NOTIFY navDataChanged)


    // Config variables from QSettings
    Q_PROPERTY(QString  confDroneIp   READ confDroneIp      WRITE setConfDroneIp      NOTIFY configChanged)
    Q_PROPERTY(bool  confShowDebug    READ confShowDebug    WRITE setConfShowDebug    NOTIFY configChanged)
    Q_PROPERTY(bool  confShowHorizon  READ confShowHorizon  WRITE setConfShowHorizon  NOTIFY configChanged)
    Q_PROPERTY(bool  confShowGauges   READ confShowGauges   WRITE setConfShowGauges   NOTIFY configChanged)
    Q_PROPERTY(bool  confUseAccel     READ confUseAccel     WRITE setConfUseAccel     NOTIFY configChanged)
    Q_PROPERTY(bool  confUseJoyStick  READ confUseJoyStick  WRITE setConfUseJoyStick  NOTIFY configChanged)
    Q_PROPERTY(bool  confFullScreen   READ confFullScreen   WRITE setConfFullScreen   NOTIFY configChanged)
    Q_PROPERTY(float confForwardGain  READ confForwardGain  WRITE setConfForwardGain  NOTIFY configChanged)
    Q_PROPERTY(float confBackwardGain READ confBackwardGain WRITE setConfBackwardGain NOTIFY configChanged)
    Q_PROPERTY(float confLeftGain     READ confLeftGain     WRITE setConfLeftGain     NOTIFY configChanged)
    Q_PROPERTY(float confRightGain    READ confRightGain    WRITE setConfRightGain    NOTIFY configChanged)
    Q_PROPERTY(float confCtrlTrsh     READ confCtrlTrsh     WRITE setConfCtrlTrsh     NOTIFY configChanged)

public:
    explicit DroneControl();
    void reset();
    float pitch();    void setPitch(float val_);
    float roll() ;    void setRoll(float val_);
    float yaw() ;     void setYaw(float val_);
    float altitude(); void setAltitude(float val_) ;
    float vVelocity();void setVVelocity(float val_) ;
    int   enabled() ; void setEnabled(int val_) ;
    bool  emergency();void setEmergency(bool val_) ;
    bool  fly() ;     void setFly(bool val_) ;
    bool  ctrlActive();void setCtrlActive(bool val_) ;

    // Read only telemetry and drone status
    QString decodedStatus();
    int pwm_motor1();
    int pwm_motor2();
    int pwm_motor3();
    int pwm_motor4();
    float droneAltitude();
    float dronePitch();
    float droneRoll();
    float droneYaw();
    float droneVBat();

    //Config variables
    QString confDroneIp();      void setConfDroneIp(QString ip);
    bool    confShowDebug();    void setConfShowDebug(bool val);
    bool    confShowHorizon();  void setConfShowHorizon(bool val);
    bool    confShowGauges();   void setConfShowGauges(bool val);
    bool    confUseAccel();     void setConfUseAccel(bool val);
    bool    confFullScreen();   void setConfFullScreen(bool val);
    bool    confUseJoyStick();  void setConfUseJoyStick(bool val);
    float   confForwardGain();  void setConfForwardGain(float val);
    float   confBackwardGain(); void setConfBackwardGain(float val);
    float   confLeftGain();     void setConfLeftGain(float val);
    float   confRightGain();    void setConfRightGain(float val);
    float   confCtrlTrsh();     void setConfCtrlTrsh(float val);

signals:
    void navDataChanged();
    void statusChanged();
    void configChanged();
    void pitchRollChanged();
public slots:
    void navDataUpdated();
    void statusUpdated();
    void viewBottomCamera(bool bottom);
private:
    float m_pitch;     // pitch in horizon
    float m_roll;      // Roll in horizon
    float m_yaw;       // low value to display
    float m_vv;        // Verticl velocity
    float m_altitude;  // altitude
    float m_rotRefX;   // Rotation zero reference
    float m_rotRefY;
    float m_rotRefZ;
    int   m_enabled;   // Enable flag
    bool  m_emergency; // Emergency flag
    QString _emgReason; // Reason of emergency
    bool  m_fly;       // Fly flag
    bool  m_ctrlActive;
    bool  m_useAccel;
    float m_ctrlTrsh;  // accel controll treshold

    QTcpSocket *ctlSock;  // TCP port for control/config data
    QUdpSocket *navSock;  // Navigation data receive socket port 5554
    QHostAddress droneHost;  // Ip address of the drone

    DroneThread *droneThread;
    QSettings *droneSettings;
};



class DroneThread:public QThread {
    Q_OBJECT
public:
    DroneThread(DroneControl *parentp,QHostAddress host);

    ~DroneThread ();
    void run();
    void sendNav(QString cmd);
    void sendCmd(QString cmd);
    NavData *navData() { return &nd; };

    enum droneState {
        notInitialized,
        initialized,
        ready,
        flying
    };
public slots:
    void navDataReady();
    void timer();
    void setFly(bool fly);
    void setEmergency(bool emg);
    void setDroneControl(float pitch,float roll,float yaw,float vv);
    void setDroneGain(float fgain,float bgain,float lgain,float rgain);
    void zap(int channel);
    void resendImportantStuff();
private:
    QHostAddress droneHost;  // Ip address of the drone
    QTimer stateTimer;
    QTimer resendImportantStuffTimer;
    volatile bool stopped;
    DroneControl *parent;
    QUdpSocket navSock;  // Navigation data receive socket port 5554
    QUdpSocket cmdSock; // Ay command socket port 5556
    int state;
    int seq;           // Drone command seq number
    int cameraMode;
    bool m_fly;
    bool m_emergency;
    float m_pitch;
    float m_roll;
    float m_yaw;
    float m_vv;
    float m_fgain;
    float m_bgain;
    float m_rgain;
    float m_lgain;
    NavData nd;
};

#endif // DRONECONTROL_H
