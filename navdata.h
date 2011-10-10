#ifndef NAVDATA_H
#define NAVDATA_H

#include <QObject>
#undef _GNU_SOURCE // just get rid of error message double definition
#include <navdata_common.h>
#include "config.h"

struct tag_t {
    unsigned int type;
    unsigned int xc;
    unsigned int yc;
    unsigned int width;
    unsigned int height;
    unsigned int dist;
};

class NavData : public QObject
{
    Q_OBJECT
public:
    explicit NavData(QObject *parent = 0);
    void parseRawNavData(char *buf,unsigned int len);
    void parseOption(navdata_option_t *op);
    QString decodeState(unsigned int state,int level);

    unsigned int state,oldState;
    QString decodedState;
    float vbat;
    float pitch;
    float roll;
    float yaw;
    float altitude;
    float vx;
    float vy;
    float vf;
    float vz;
    float current_motor1;
    float current_motor2;
    float current_motor3;
    float current_motor4;
    int pwm_motor1;
    int pwm_motor2;
    int pwm_motor3;
    int pwm_motor4;
    tag_t tags[4];
signals:
    void navDataUpdated();
    void stateUpdated();
public slots:

};

#endif // NAVDATA_H
