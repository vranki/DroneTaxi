/*==================================================================
  ! Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
  ! All rights reserved.
  !
  !  Author:Kate Alhola  kate.alhola@nokia.com
  !
  ! GNU Lesser General Public License Usage
  ! This file may be used under the terms of the GNU Lesser
  ! General Public License version 2.1 as published by the Free Software
  ! Foundation and appearing in the file LICENSE.LGPL included in the
  ! packaging of this file.  Please review the following information to
  ! ensure the GNU Lesser General Public License version 2.1 requirements
  ! will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
  !
  !
  !
  *===================================================================*/
#include "dronecontrol.h"
#include <QDebug>

DroneControl::DroneControl():QObject()
{
    //    ctlSock=new QTcpSocket();
    //    ctlSock->bind(QHostAddress::Any,5559);
    //    navSock=new QUdpSocket();
    //    navSock->bind(QHostAddress::Any,5554)
    //connect(navSocket,SIGNAL(readyRead()),SLOT(navDataReady()));
    droneHost.setAddress("192.168.1.1");
    droneSettings=new QSettings("symbio.com","dronetaxi");
    droneThread = 0;
    reset();
}

void DroneControl::reset() {
    if(droneThread) {
        delete droneThread;
        droneThread = 0;
    }
    droneThread = new DroneThread(this,droneHost);
    connect(droneThread->navData(),SIGNAL(navDataUpdated()),this,SLOT(navDataUpdated()));
    connect(droneThread->navData(),SIGNAL(stateUpdated()),this,SLOT(statusUpdated()));
    m_ctrlActive=false;
    m_pitch=0;
    m_roll=0;
    m_yaw=0;
    m_vv=0;
    m_fly = 0;
    m_emergency = 0;
    m_enabled = 0;

    m_useAccel=droneSettings->value("useAccel",false).toBool();
    m_ctrlTrsh=droneSettings->value("ctrlTreshold",10.0).toFloat();
    droneThread->setDroneGain(confForwardGain(),confBackwardGain(),confLeftGain(),confRightGain());

//    setEmergency(false);
    setEnabled(true);
    setFly(false);

    viewBottomCamera(false);

}

void DroneControl::navDataUpdated()
{
    emit navDataChanged();
}

void DroneControl::statusUpdated()
{
    emit statusChanged();
}

void DroneControl::setPitch(float val_)
{
    m_pitch=val_;
    droneThread->setDroneControl(m_pitch,m_roll,m_yaw,m_vv);
};
float DroneControl::pitch() {return m_pitch;};

void DroneControl::setRoll(float val_)
{
    m_roll=val_;
    //    qDebug() << "setRoll=" << val_;
    droneThread->setDroneControl(m_pitch,m_roll,m_yaw,m_vv);
};
float DroneControl::roll() {return m_roll;};
void DroneControl::setYaw(float val) {
    m_yaw=val;
    //   qDebug() << "setYaw=" << val;
    if(m_useAccel && m_ctrlActive) m_yaw=(fabs(m_yaw)<(m_ctrlTrsh*2) ) ? 0:(m_yaw>0 ? m_yaw-(m_ctrlTrsh*2):m_yaw+(m_ctrlTrsh*2));
    droneThread->setDroneControl(m_pitch,m_roll,m_yaw,m_vv);
};
float DroneControl::yaw() {return m_yaw;};
void DroneControl::setVVelocity(float val) {
    m_vv=val;
    if(m_useAccel && m_ctrlActive) m_vv=(fabs(m_vv)<(m_ctrlTrsh*2) ) ? 0:(m_vv>0? m_vv-(m_ctrlTrsh*2):m_vv+(m_ctrlTrsh*2));
    //    qDebug() << "setVv=" << val_;
    droneThread->setDroneControl(m_pitch,m_roll,m_yaw,m_vv);
};
float DroneControl::vVelocity() {return m_vv;};
void DroneControl::setAltitude(float val_) {
    m_altitude=val_;
};
float DroneControl::altitude() {return m_altitude;};
void DroneControl::setEnabled(int val_) { m_enabled=val_;};
int DroneControl::enabled() {return m_enabled;};

bool DroneControl::ctrlActive()
{
    return m_ctrlActive;
};
void DroneControl::setCtrlActive(bool val_)
{
    m_ctrlActive=val_;
    if(!m_ctrlActive) { m_pitch=0;m_roll=0; };
    droneThread->setDroneControl(m_pitch,m_roll,m_yaw,m_vv);
    emit pitchRollChanged();
};


void DroneControl::setFly(bool val_)
{
    m_fly=val_;
    qDebug() << "setFly=" << val_;
    droneThread->setFly(m_fly);
};
bool DroneControl::fly() {return m_fly;};
void DroneControl::setEmergency(bool val_)
{
    m_emergency=val_;
    qDebug() << "setEmergency=" << val_;
    droneThread->setEmergency(m_emergency);
};
bool DroneControl::emergency() {return m_emergency;};


// Getters to drone actual valyes sent by drone
float DroneControl::droneAltitude()
{
    return droneThread->navData()->altitude;
};
float DroneControl::dronePitch()
{
    return droneThread->navData()->pitch;
};
float DroneControl::droneRoll()
{
    return droneThread->navData()->roll;
};
float DroneControl::droneYaw()
{
    return droneThread->navData()->yaw;
};
float DroneControl::droneVBat()
{
    return droneThread->navData()->vbat;
};
QString DroneControl::decodedStatus()
{
    return droneThread->navData()->decodedState;
};
int DroneControl::pwm_motor1()
{
    return droneThread->navData()->pwm_motor1;
};
int DroneControl::pwm_motor2()
{
    return droneThread->navData()->pwm_motor2;
};
int DroneControl::pwm_motor3()
{
    return droneThread->navData()->pwm_motor3;
};
int DroneControl::pwm_motor4()
{
    return droneThread->navData()->pwm_motor4;
};


QString DroneControl::confDroneIp()
{
    qDebug() << "confDroneIp:" << droneSettings->value("droneIp","192.168.1.1").toString();
    return droneSettings->value("droneIp","192.168.1.1").toString();
};
void DroneControl::setConfDroneIp(QString ip)
{
    qDebug() << "setConfDroneIp:" << ip;
    droneSettings->setValue("droneIp",ip);
    emit configChanged();
};
bool DroneControl::confShowDebug()
{
    return droneSettings->value("showDebug",true).toBool();
};
void DroneControl::setConfShowDebug(bool val)
{
    droneSettings->setValue("showDebug",val);
    emit configChanged();
};
bool DroneControl::confShowHorizon()
{
    return droneSettings->value("showHorizon",true).toBool();
};
void DroneControl::setConfShowHorizon(bool val)
{
    droneSettings->setValue("showHorizon",val);
    emit configChanged();
};
bool DroneControl::confShowGauges()
{
    return droneSettings->value("showGuges",true).toBool();
};
void DroneControl::setConfShowGauges(bool val)
{
    droneSettings->setValue("showGauges",val);
    emit configChanged();
};
bool DroneControl::confUseAccel()
{
    //return droneSettings->value("useAccel",false).toBool();
    return m_useAccel; // return cached value
};
void DroneControl::setConfUseAccel(bool val)
{
    droneSettings->setValue("useAccel",val);
    m_useAccel=val;
    emit configChanged();
};
bool DroneControl::confUseJoyStick()
{
    return droneSettings->value("useJouStick",false).toBool();
};
void DroneControl::setConfUseJoyStick(bool val)
{
    droneSettings->setValue("useJoyStick",val);
    emit configChanged();
};

bool DroneControl::confFullScreen()
{
    return droneSettings->value("fullScreen",true).toBool();
};
void DroneControl::setConfFullScreen(bool val)
{
    droneSettings->setValue("fullScreen",val);
    emit configChanged();
};

float DroneControl::confForwardGain()
{
    return droneSettings->value("forwardGain",1.0).toFloat();
};
void DroneControl::setConfForwardGain(float val)
{
    droneSettings->setValue("forwardGain",val);
    droneThread->setDroneGain(confForwardGain(),confBackwardGain(),confLeftGain(),confRightGain());
    emit configChanged();
};
float DroneControl::confCtrlTrsh()
{
    return m_ctrlTrsh;
};
void DroneControl::setConfCtrlTrsh(float val)
{
    droneSettings->setValue("ctrlTreshold",val);
    m_ctrlTrsh=val;
    emit configChanged();
};

float DroneControl::confBackwardGain()
{
    return droneSettings->value("backwardGain",1.0).toFloat();
};
void DroneControl::setConfBackwardGain(float val)
{
    droneSettings->setValue("backwardGain",val);
    droneThread->setDroneGain(confForwardGain(),confBackwardGain(),confLeftGain(),confRightGain());
    emit configChanged();
};

float DroneControl::confLeftGain()
{
    return droneSettings->value("leftGain",1.0).toFloat();
};
void DroneControl::setConfLeftGain(float val)
{
    droneSettings->setValue("leftGain",val);
    droneThread->setDroneGain(confForwardGain(),confBackwardGain(),confLeftGain(),confRightGain());
    emit configChanged();
};
float DroneControl::confRightGain()
{
    return droneSettings->value("rightGain",1.0).toFloat();
};
void DroneControl::setConfRightGain(float val)
{
    droneSettings->setValue("rightGain",val);
    droneThread->setDroneGain(confForwardGain(),confBackwardGain(),confLeftGain(),confRightGain());
    emit configChanged();
};

void DroneControl::viewBottomCamera(bool bottom) {
    qDebug() << Q_FUNC_INFO << bottom;
    if(bottom) {
        droneThread->zap(1); // From ardrone_api.h
    } else {
        droneThread->zap(0);
    }
}

/*=================================================

 DroneThread class starts here

==================================================*/

DroneThread::~DroneThread() {
    navSock.close();
    cmdSock.close();
}

void DroneThread::zap(int channel)
{
    cameraMode = channel;
    sendCmd(QString("AT*ZAP=%1,%2\r").arg(seq++).arg(cameraMode));
}

void DroneThread::setFly(bool fly)
{
    qDebug() << Q_FUNC_INFO << fly;
    if(state==ready) {
        m_fly=fly;
        state=flying;
        stateTimer.setInterval(50); // More frequent updates
        sendCmd(QString("AT*FTRIM=%1\r").arg(seq++));
    }
    if(state==flying) {
        if(!fly) {
            stateTimer.setInterval(200); // Less frequent updates
            state=ready;
        }
        m_fly=fly;
    }
    sendCmd(QString("AT*REF=%1,%2\r").arg(seq++).arg((1<<18) + (1<<20) + (1<<22) + (1<<24) +(1<<28) +(state==flying ? 1<<9:0)));
};

void DroneThread::setEmergency(bool emg)
{
    m_emergency=emg;
    sendCmd(QString("AT*REF=%1,%2\r").arg(seq++).arg((1<<18) + (1<<20) + (1<<22) + (1<<24) +(1<<28) + (m_emergency ? 1<<8:0)));
    // if(m_emergency==1)
    state=ready;
}   ;
void DroneThread::setDroneControl(float pitch,float roll,float yaw,float vv)
{
    m_pitch=pitch/200.0*m_fgain;
    m_roll=roll/200.0*m_rgain;
    m_yaw=yaw/200.0;
    m_vv=vv/200.0;
    //    qDebug()  << QString("pitch=%1 roll=%2 yaw=%3 vv=%4\r").arg(m_pitch,3,'F',2).arg(m_roll,3,'F',2).arg(m_yaw,3,'F',2).arg(m_vv,3,'F',2);
};

void DroneThread::setDroneGain(float fgain,float bgain,float lgain,float rgain)
{
    m_fgain=fgain;
    m_bgain=bgain;
    m_rgain=rgain;
    m_lgain=lgain;
};

void DroneThread::sendCmd(QString cmd)
{
    QByteArray dgram;
    QString seqCmd=cmd;
    if(cmd.contains("%")) seqCmd=cmd.arg(seq++);
    dgram=seqCmd.toLatin1();
    cmdSock.writeDatagram(dgram.data(),dgram.size(),droneHost,5556);
    seqCmd.chop(1); // Remove training cr
    if(cmd.contains("PCMD"))
        qDebug() << "DroneThread::sendCmd= " << seqCmd << "to " << droneHost ;
}

void DroneThread::navDataReady()
{
    qint64 l;
    char buf[2048];
    QHostAddress host;
    quint16 port;
    while(navSock.hasPendingDatagrams())
        l=navSock.readDatagram(buf,sizeof(buf),&host,&port);
    //   qDebug() << "DroneThread::navDataReady state=" << state <<" l=" << l << "read=" << buf << "from"  << host ;
    nd.parseRawNavData((char *)&buf,l);
    if(nd.state& (ARDRONE_COM_WATCHDOG_MASK ))
        sendCmd(QString("AT*COMWDG=%1\r").arg(seq++));
    switch(state) {
    case notInitialized:
        //        sendCmd("AT*CONFIG=%1,\"general:navdata_demo\",\"FALSE\"\r");
        sendCmd("AT*CONFIG=%1,\"general:navdata_demo\",\"TRUE\"\r");
        state=initialized;
        break;
    case initialized:
        sendCmd("AT*CTRL=0\r");
        sendCmd("AT*CONFIG=%1,\"detect:detect_type\",\"2\"\r");
        state=ready;
        break;
    case ready:    
        break;
    }
}

void DroneThread::sendNav(QString cmd)
{
    QByteArray dgram;
    dgram=cmd.toLatin1();
    qDebug() << "DroneThread::sendNav= " << cmd+"\n" << "to " << droneHost ;
    navSock.writeDatagram(dgram.data(),dgram.size(),droneHost,5554);
}

DroneThread::DroneThread(DroneControl *parentp,QHostAddress host)
{
    qDebug() << "DroneThread::DroneThread";
    stopped=false;
    state=notInitialized;
    parent=parentp;
    navSock.bind(QHostAddress::Any,5554);
    cmdSock.bind(QHostAddress::Any,5556);
    Q_ASSERT(navSock.isValid());
    Q_ASSERT(cmdSock.isValid());
    droneHost=host;
    seq=1;
    m_pitch=0;
    m_roll=0;
    m_yaw=0;
    m_vv=0;
    m_fgain=1.0;
    m_bgain=1.0;
    m_rgain=1.0;
    m_lgain=1.0;
    m_fly = false;
    cameraMode = 0;
    connect(&resendImportantStuffTimer, SIGNAL(timeout()), this, SLOT(resendImportantStuff()));
    resendImportantStuffTimer.setSingleShot(false);
    resendImportantStuffTimer.start(2000);
    start();
};

void DroneThread::run()
{
    qDebug() << "DroneThread::DroneThread";
    connect(&stateTimer,SIGNAL(timeout()),this,SLOT(timer()));
    connect(&navSock,SIGNAL(readyRead()),this,SLOT(navDataReady()));
    stateTimer.start(1000);
    while(!stopped) {
        exec();
    }
}

void DroneThread::timer()
{
    //  qDebug() << "thread Timer";
    switch(state) {
    case notInitialized:
        sendNav("AT");
        break;
    case initialized:
        break;
    case ready:
        sendCmd(QString("AT*REF=%1,%2\r").arg(seq++).arg((1<<18) + (1<<20) + (1<<22) + (1<<24) +(1<<28)));
        break;
    case flying: {
            float_or_int_t _pitch,_roll,_yaw,_vv;
            int r=(m_pitch!=0.0 || m_roll!=0.0)?1:0;


            _pitch.f=m_pitch;
            _roll.f=m_roll;
            _yaw.f=m_yaw;
            _vv.f=m_vv;
            //            qDebug()  << QString("AT*PCMD=%1,0,%2,%3,%4,%5\r").arg(seq).arg(_roll.f,3,'F',2).arg(_pitch.f,3,'F',2).arg(_vv.f,3,'F',2).arg(_yaw.f,3,'F',2);
            //            qDebug() << QString("AT*PCMD=%1,0,%2,%3,%4,%5\r").arg(seq).arg(_roll.i,8,16).arg(_pitch.i,8,16).arg(_vv.i,8,16).arg(_yaw.i,8,16);
            sendCmd(QString("AT*COMWDG=%1\r").arg(seq++));
            sendCmd(QString("AT*PCMD=%1,%2,%3,%4,%5,%6\r").arg(seq++).arg(r).arg(_roll.i).arg(_pitch.i).arg(_vv.i).arg(_yaw.i));

            break;
        }
    }
}

void DroneThread::resendImportantStuff() {
    qDebug() << Q_FUNC_INFO << cameraMode << m_fly;
    zap(cameraMode);
    setFly(m_fly);
    return;
    sendCmd("AT*CONFIG=%1,\"general:navdata_demo\",\"TRUE\"\r");
}
