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

#include "gaugelabel.h"
#include <QDebug>


GaugeLabel::GaugeLabel(QGraphicsItem *parent) :
    Gauge(parent)
{
}

void GaugeLabel::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    drawNumLabel(painter);
};
//////////////////////////////////////////////////////////////////////////
void GaugeLabel::drawNumLabel(QPainter *painter)
{
  int ld,ldo,ud,lo;
  int split,uw,base;
  char text[20] = {0};
  int h=size().height();
  int w=size().width();
  int val=m_value;
   painter->save();
  qDebug() <<"drawnumlabel=" << m_name << " h=" << h << "w=" << w;
  if(m_style&SCROLL) {
    QFont sansFont("Helvetica [Cronyx]", 12);
    QFontMetrics fm(sansFont);
    painter->setFont(sansFont);

    ld=abs(val%100);
    ud=val/100;
    ldo=ld%10;
    printf("ld=%d ldo=%d lo=%d h=%d\n",ld,ldo,lo,h);
    lo=(ldo*h)/-20;
    drawLabel(painter, text, m_style);
    sprintf(text, "%3d", ud);
    split=w/2;
    base=(h+fm.ascent())/2;
    uw=fm.width(text);

    painter->drawText(split-uw+6, base, text);
    sprintf(text, "%02d", abs((ld-ldo-20)%100));
    painter->drawText(split+ 7, base -h +lo, text);
    sprintf(text, "%02d", abs((ld-ldo-10)%100));
    painter->drawText(split+ 7, base -h/2 +lo, text);
    sprintf(text, "%02d", abs((ld-ldo)%100));
    painter->drawText(split +7, base +lo, text);
    sprintf(text, "%02d", abs((ld-ldo+10)%100));
    painter->drawText(split + 7,base + h/2 +lo , text);
    sprintf(text, "%02d", abs((ld-ldo+20)%100));
    painter->drawText(split + 7,base + h +lo , text);


  } else {
    sprintf(text, "%3d", val);
    drawLabel(painter, text,m_style);
  }
  painter->restore();  // Restore and remove clicping rectabgle
}

///////////////////////////////////////////////////////////////////////////
void GaugeLabel::drawLabel(QPainter *painter,char *text,int style)
{
  int textlen = 0, i = 0;
  int xplace=0;
  int yplace=0;
  int height=size().height();
  int width=size().width();
  int scrbox=(height*10)/30;


  QVector<QPoint> nboxv;

  QPen pen(Qt::white);
  painter->setPen(pen);


  if (style&7 == LEFT) xplace+=5;

  nboxv.push_back(QPoint(xplace,yplace));



  if (style&7 == UP) {
    nboxv.push_back(QPoint(xplace+(width/2)-5,yplace  ));
    nboxv.push_back(QPoint(xplace+(width/2)  ,yplace-5));
    nboxv.push_back(QPoint(xplace+(width/2)+5,yplace  ));
  }

  if (style&SCROLL) {
    nboxv.push_back(QPoint(xplace+width/2,yplace));
    nboxv.push_back(QPoint(xplace+width/2,yplace-scrbox));
    nboxv.push_back(QPoint(xplace+width,yplace-scrbox  ));
  } else
    nboxv.push_back(QPoint(xplace+width,yplace  ));


  if (style&7== RIGHT) {

    nboxv.push_back(QPoint(xplace+width  ,yplace+(height/2)-5));
    nboxv.push_back(QPoint(xplace+width+5,yplace+(height/2)  ));
    nboxv.push_back(QPoint(xplace+width  ,yplace+(height/2)+5));
  }

  if (style&SCROLL) {
    nboxv.push_back(QPoint(xplace+width,yplace+height+scrbox));
    nboxv.push_back(QPoint(xplace+width/2,yplace+height+scrbox));
    nboxv.push_back(QPoint(xplace+width/2,yplace+height));
  } else
    nboxv.push_back(QPoint(xplace+width,yplace+height));

  if (style&7== DOWN) {
    nboxv.push_back(QPoint(xplace+(width/2)-5,yplace+height));
    nboxv.push_back(QPoint(xplace+(width/2)  ,yplace+height+5));
    nboxv.push_back(QPoint(xplace+(width/2)+5,yplace+height));
  }

  nboxv.push_back(QPoint(xplace,yplace+height));

  if (style&7 == LEFT) {
    nboxv.push_back(QPoint(xplace  ,yplace+(height/2)+5));
    nboxv.push_back(QPoint(xplace-5,yplace+(height/2)  ));
    nboxv.push_back(QPoint(xplace  ,yplace+(height/2)-5));
  }

  nboxv.push_back(QPoint(xplace,yplace));

  textlen = strlen(text) + 2;

  //  pdrawable->draw_rectangle(pwidget->get_style()->get_black_gc(),
  //			    true, xplace, yplace, width, height);

  if (style&SCROLL)
    painter->setClipRect(QRect(xplace-5,yplace-scrbox, width+10, height+2*scrbox+1));

  QBrush brush(QColor(0,0,0,255));
  //  QFont tapeFont("Helvetica [Cronyx]", 18);
  //  painter->setFont(tapeFont);
  //  painter->setPen(pen);
  painter->setBrush(brush);

  painter->drawPolygon(nboxv);

  painter->drawLines(nboxv);

  //  painter->setBrush(brush);
  QFont sansFont("Helvetica [Cronyx]", 12);
  QFontMetrics fm(sansFont);
  painter->setFont(sansFont);

  painter->drawText(xplace + 3, yplace + (height+fm.ascent())/2, text);
}

