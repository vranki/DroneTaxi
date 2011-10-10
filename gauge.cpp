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

#include "gauge.h"
#include "qgraphicswidget.h"
#include "QGraphicsItem"
#include <QPainter>
#include <QDebug>

Gauge::Gauge(QGraphicsItem *parent) :
    QGraphicsWidget(parent)
{
    qDebug() << "Gauge::Gauge()";
}
#if 1
void Gauge::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
 {

 }
#endif
void Gauge::setValue(float val_)
{
    m_value=val_;
    update(boundingRect()); // Value updated, schedule redtaw
};
float Gauge::value()
{
    return m_value;
};

QRectF Gauge::boundingRect() const
    {
        return QRectF(0.0,0.0,size().width(),size().height());
    }
