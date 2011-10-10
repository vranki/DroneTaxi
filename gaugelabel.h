#ifndef GAUGELABEL_H
#define GAUGELABEL_H
#include "gauge.h"

class GaugeLabel : public Gauge
{
    Q_OBJECT
public:
    explicit GaugeLabel(QGraphicsItem *parent = 0);
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,
                               QWidget *widget);
    void drawNumLabel(QPainter *painter);
    void drawLabel(QPainter *painter,char *text,int style);
signals:
};

#endif // GAUGELABEL_H
