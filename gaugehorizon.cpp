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

#include "gaugehorizon.h"
#include <QDebug>
#include <math.h>

GaugeHorizon::GaugeHorizon(QGraphicsItem *parent) :
    Gauge(parent)
{
}



void GaugeHorizon::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    drawHorizon(painter,-1.0*m_value,5*m_value2);
};
//////////////////////////////////////////////////////////////////////////
void GaugeHorizon::drawHorizon(QPainter *painter, float roll, float pitch)
    {
      QRect update_rect;
      QPoint points[10];
      //QPoint plane[10];
      QVector<QLine> plane;
      QVector<QLine>  fixo;
      int tx, ctx, xd, i, j ;
      int xo, yo, x1, x2, y1, y2;

      xo=size().width()/2;
      yo=size().height()/2;

      plane+=QLine(xo-50,yo,xo-20,yo);
      plane+=QLine(xo-20,yo,xo-10,yo+10);
      plane+=QLine(xo-10,yo+10,xo,yo);
      plane+=QLine(xo,yo,xo+10,yo+10);
      plane+=QLine(xo+20,yo,xo+10,yo+10);
      plane+=QLine(xo+50,yo,xo+20,yo);


      static float sintable[]={sin(15*3.141592653/180.0),
                               sin(30*3.141592653/180.0),
                               sin(45*3.141592653/180.0),
                               sin(60*3.141592653/180.0)};

      static float costable[]={cos(15*3.141592653/180.0),
                               cos(30*3.141592653/180.0),
                               cos(45*3.141592653/180.0),
                               cos(60*3.141592653/180.0)};

      i = 0;
      int arc0=130;
      int arc1=150;
      fixo+=QLine(xo,yo-arc0,xo,yo-arc1);
      for (i = 0; i < 4; i++) {
        fixo+=QLine(xo+arc0*costable[i],yo-arc0*sintable[i],xo+arc1*costable[i],yo-arc1*sintable[i]);
        fixo+=QLine(xo-arc0*costable[i],yo-arc0*sintable[i],xo-arc1*costable[i],yo-arc1*sintable[i]);
      };


      tx = (int)(xo * tan(roll * 3.141592653/180.0));
      xd = yo + pitch * 4;

      QTransform savematrix= painter->transform ();
     // painter->setClipRect(QRect(0,0, size().width(),size().height()/2));

      painter->setTransform(QTransform().translate(xo+scenePos().x(), yo+scenePos().y()+pitch).rotate(roll));

      QBrush whitebrush(Qt::green);
      painter->setBrush(whitebrush);
      QPen whitepen(Qt::green);
      whitepen.setWidth(2);
      painter->setPen(whitepen);
      xo=0;yo=0;

      painter->drawLine(50+xo,    0+yo,-50+xo,   0+yo);
      painter->drawLine(40+xo,   50+yo,-40+xo,  50+yo);
      painter->drawLine(40+xo,  100+yo,-40+xo, 100+yo);
      painter->drawLine(40+xo,  -50+yo,-40+xo, -50+yo);
      painter->drawLine(40+xo, -100+yo,-40+xo,-100+yo);

      painter->drawText( 43+xo,-40+yo,"10");
      painter->drawText(-63+xo,-40+yo,"10");
      painter->drawText( 43+xo,-90+yo,"20");
      painter->drawText(-63+xo,-90+yo,"20");
      painter->drawText( 43+xo, 40+yo,"10");
      painter->drawText(-63+xo, 40+yo,"10");
      painter->drawText( 43+xo, 90+yo,"20");
      painter->drawText(-63+xo, 90+yo,"20");


      //  painter->rotate(0);
      painter->setTransform(savematrix);
      QPen bluepen(Qt::blue);
      bluepen.setWidth(2);
      painter->setPen(bluepen);
      painter->drawLines(fixo);
      painter->drawLines(plane);



}

