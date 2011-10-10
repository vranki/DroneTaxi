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
#include <QtGui/QApplication>
#include "dronecontrol.h"
#include "video.h"
#include "gaugehorizon.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QUrl *url;
    MainWindow window;
    app.connect(&window, SIGNAL(destroyed()), &app, SLOT(quit()));
   // window.show();
    return app.exec();
}
