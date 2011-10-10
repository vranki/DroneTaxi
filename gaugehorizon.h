#ifndef GAUGELABEL_H
#define GAUGELABEL_H
#include "gauge.h"

class GaugeHorizon : public Gauge
{
    Q_OBJECT
public:
    explicit GaugeHorizon(QGraphicsItem *parent = 0);
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,
                               QWidget *widget);
    void drawHorizon(QPainter *painter,float pitch,float roll);
signals:
};

#endif // GAUGELABEL_H
