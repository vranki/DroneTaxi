/*==================================================================
  !
  !  mardrone application AR-Drone for MeeGo

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
#include <QDebug>
#include "navdata.h"


NavData::NavData(QObject *parent) :
    QObject(parent)
{
    vx=0;vy=0;vz=0;pwm_motor1=0;pwm_motor2=0;pwm_motor3=0;pwm_motor4=0;
    vbat=0.0;pitch=0.0;roll=0.0;yaw=0.0;altitude=0.0;
}

void NavData::parseRawNavData(char *buf,unsigned int len)
{
//    qDebug() << Q_FUNC_INFO;
    int opt=0;
    navdata_option_t *op;
    _navdata_t *nd=(_navdata_t *)buf;
    state=nd->ardrone_state;
    if(state!=oldState) {
        oldState=state;
        decodedState=decodeState(state,0);
        emit stateUpdated();
    };
#if 0
    qDebug("parseRawNavData hdr=%8x state=%08x seq=%06d opt[0].tag=%d size=%d",
           nd->header,nd->ardrone_state,nd->sequence,nd->options[0].tag,nd->options[0].size);
#endif
    qDebug() << decodedState;
    op=&(nd->options[0]);
    while((((unsigned int)op-(unsigned int)buf)<len) && (op->size>0))
    {
        parseOption(op);
        op=(navdata_option_t *)((unsigned int)op+op->size);
    }
};

void NavData::parseOption(navdata_option_t *op)
{
    //  qDebug("parseOption tag=%d size=%d",op->tag,op->size);
    switch(op->tag) {
    case NAVDATA_DEMO_TAG:
    {
        _navdata_demo_t *ndemo=(_navdata_demo_t*)op;
        vbat=ndemo->vbat_flying_percentage;
        pitch=ndemo->theta/1000.0;
        roll=ndemo->phi/1000.0;
        yaw=ndemo->psi/1000.0;
        altitude=ndemo->altitude;
        vx=ndemo->vx;
        vy=ndemo->vy;
        vz=ndemo->vz;
        emit navDataUpdated();
        // qDebug("pitch=%2.1f roll=%2.1f yaw=%2.1f alt=%2.1f v(%2.1f,%2.1f,%2.1f bat=%2.1f",pitch,roll,yaw,altitude,vx,vy,vz,vbat);
    }
        break;
    case NAVDATA_TIME_TAG:
    {
        _navdata_time_t *ntdime=(_navdata_time_t*)op;
    }
        break;
    case NAVDATA_RAW_MEASURES_TAG:
    {
        _navdata_raw_measures_t *nraw=(_navdata_raw_measures_t*)op;
    }
        break;
    case NAVDATA_PHYS_MEASURES_TAG:
    {
        _navdata_phys_measures_t *nphys=(_navdata_phys_measures_t*)op;
    }
        break;
    case NAVDATA_GYROS_OFFSETS_TAG:
        break;
    case NAVDATA_EULER_ANGLES_TAG:
        break;
    case NAVDATA_REFERENCES_TAG:
        break;
    case NAVDATA_TRIMS_TAG: // 7
    {
        _navdata_trims_t *ntrim=( _navdata_trims_t*)op;
    }
        break;
    case NAVDATA_RC_REFERENCES_TAG: //8
        break;
    case NAVDATA_PWM_TAG:
    {
        _navdata_pwm_t *npwm=( _navdata_pwm_t*)op;
        current_motor1=npwm->current_motor1;
        pwm_motor1=npwm->motor1;
        current_motor2=npwm->current_motor2;
        pwm_motor2=npwm->motor2;
        current_motor3=npwm->current_motor3;
        pwm_motor3=npwm->motor3;
        current_motor4=npwm->current_motor4;
        pwm_motor4=npwm->motor4;
        emit navDataUpdated();
        //       qDebug("pwm1=%3d I1=%4f pwm2=%3d I2=%4f pwm3=%3d I3=%4f pwm4=%3d I4=%4f",
        //              pwm_motor1,current_motor1,pwm_motor2,current_motor2,pwm_motor3,current_motor3,pwm_motor4,current_motor4);
    }
        break;
    case NAVDATA_ALTITUDE_TAG: //10
        break;
    case NAVDATA_VISION_RAW_TAG:
        break;
    case NAVDATA_VISION_OF_TAG: //12
        break;
    case NAVDATA_VISION_TAG:
        break;
    case NAVDATA_VISION_PERF_TAG:
        break;
    case NAVDATA_TRACKERS_SEND_TAG:
        break;
    case NAVDATA_VISION_DETECT_TAG: //16
    {
        _navdata_vision_detect_t *nvisd=(_navdata_vision_detect_t*)op;
        //qDebug("nb_detected=%d type=%d xc=%d yc=%d width=%d height=%d dist=%d",nvisd->nb_detected,nvisd->type[0],nvisd->xc[0],nvisd->yc[0],
        //      nvisd->width[0],nvisd->height[0],nvisd->dist[0]);
        for(int i=0;i<4;i++) {
            tags[i].type=nvisd->type[i];
            tags[i].xc=nvisd->xc[i];
            tags[i].yc=nvisd->yc[i];
            tags[i].height=nvisd->height[i];
            tags[i].width=nvisd->width[i];
            tags[i].dist=nvisd->dist[i];
        }
    }
        break;
    case NAVDATA_WATCHDOG_TAG:
        break;
    case NAVDATA_ADC_DATA_FRAME_TAG:
        break;
    case NAVDATA_CKS_TAG: //65535
        break;
    }
};

QString NavData::decodeState(unsigned int state,int level)
{
    QString s="%1 ";
    s=s.arg(state,8,16);
    s+=(state & ARDRONE_FLY_MASK)==0 ? "landed ":"flying ";
    if(level>1) s+=(state & ARDRONE_VIDEO_MASK)==0 ?"video disable ":"video ena ";
    if(level>1)s+=(state & ARDRONE_VISION_MASK)==0 ? "vision disable ":"vision ena ";
    if(level>1)s+=(state & ARDRONE_CONTROL_MASK)==0 ?  "euler angles ":"angular speed ";
    if(level>1)s+=(state & ARDRONE_ALTITUDE_MASK)==0 ? "alt ctrl inact ":"alt ctrl act ";
    //s+=(state & ARDRONE_USER_FEEDBACK_START)==0 ?  /*!< USER feedback : Start button state */
    s+=(state & ARDRONE_COMMAND_MASK)==0 ?  "CMD NAK ":"CMD ACK ";
    //  s+=(state & ARDRONE_FW_FILE_MASK)==0 ?  /* Firmware file is good;
    //  s+=(state & ARDRONE_FW_VER_MASK )==0 ?  /* Firmware update is newer;
    //  ARDRONE_FW_UPD_MASK         = 1 << 9,  /* Firmware update is ongoing (1;
    s+=(state & ARDRONE_NAVDATA_DEMO_MASK )==0 ? "All navdata ":"navdata demo ";
    s+=(state & ARDRONE_NAVDATA_BOOTSTRAP)==0 ? "":"Navdata bootstrap ";
    //  s+=(state & ARDRONE_MOTORS_MASK)==0 ? /*!< Motors status : (0) Ok, (1) Motors problem */
    s+=(state & ARDRONE_COM_LOST_MASK)? "Com Lost ":"Com ok ";
    if(level>1)s+=(state & ARDRONE_VBAT_LOW) ? "VBat low ":"Vbat Ok ";
    if(level>1)s+=(state & ARDRONE_USER_EL) ? "User EL ":"";
    s+=(state & ARDRONE_TIMER_ELAPSED) ? "Timer elapsed ":"";
    s+=(state & ARDRONE_ANGLES_OUT_OF_RANGE) ? "Angles out of range ":"";
    s+=(state & ARDRONE_ULTRASOUND_MASK) ? "Ultrasonic sensor deaf ":"";
    s+=(state & ARDRONE_CUTOUT_MASK) ? "Cutout system detection detected":"";
    //   s+=(state & ARDRONE_PIC_VERSION_MASK)==0 ? /*!< PIC Version number OK : (0) a bad version number, (1) version number is OK */
    //   s+=(state & ARDRONE_ATCODEC_THREAD_ON)==0 ? /*!< ATCodec thread ON : (0) thread OFF (1) thread ON */
    if(level>2)s+=(state & ARDRONE_NAVDATA_THREAD_ON)==0 ? "Navdata thread OFF ":"Navdata thread ON ";
    if(level>2)s+=(state & ARDRONE_VIDEO_THREAD_ON )==0 ? "Video thread OFF ":"Video thread ON ";
    //   s+=+(state & ARDRONE_ACQ_THREAD_ON)==0 ?  "Acquisition thread OFF ":"Acquisition thread ON ";
    s+=(state & ARDRONE_CTRL_WATCHDOG_MASK) ? "CTRL watchdog ":"";
    s+=(state & ARDRONE_ADC_WATCHDOG_MASK) ? "ADC Watchdog ":"";
    s+=(state & ARDRONE_COM_WATCHDOG_MASK) ? "Comm Watchdog ":"";
    s+=(state & ARDRONE_EMERGENCY_MASK)?  "Emg landing":"";
    return s;
}
