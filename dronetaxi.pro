#-------------------------------------------------
#
# Project created by QtCreator 2010-10-12T20:45:34
#
#-------------------------------------------------

QT       += core gui network
CONFIG += debug
TARGET = dronetaxi
TEMPLATE = app

maemo5 {
  AR_DRONE_SDK= $$(HOME)/ardrone_sdk
  LIBS += -lARToolkitPlus
} else {
  AR_DRONE_SDK= $$(HOME)/ARDrone_SDK_1_5_Version_20101004
  LIBS += -lARToolKitPlus
}
LIBS += -L$${AR_DRONE_SDK}/lib -lvlib -lsdk -lpc_ardrone

CONFIG += link_pkgconfig
PKGCONFIG += sdl

SOURCES += main.cpp\
    dronecontrol.cpp \
    navdata.cpp \
    video.cpp \
    gauge.cpp \
    gaugelabel.cpp \
    gaugehorizon.cpp \
    mainwindow.cpp \
    paddetection.cpp \
    joystick.cpp \
    gamesettingsdialog.cpp \
    gamemode.cpp \
    gamemodedronetaxi.cpp \
    gamemoderace.cpp \
    gamemodefreeflight.cpp \
    moresettingsdialog.cpp \
    sdljoystick.cpp \
    helpdialog.cpp \
    intro.cpp

INCLUDEPATH += $${AR_DRONE_SDK}/ARDroneLib/Soft/Common \
               $${AR_DRONE_SDK}/ARDroneLib/VP_SDK \
               $${AR_DRONE_SDK}/ARDroneLib/VP_SDK/VP_Os/linux \
               $${AR_DRONE_SDK}/ARDroneLib/VP_SDK/VP_Os \
               $${AR_DRONE_SDK}/ARDroneLib/Soft/Lib \
               $${AR_DRONE_SDK}/ARDroneLib/VLIB \
               $${AR_DRONE_SDK}/ARDroneLib

HEADERS  +=  dronecontrol.h \
    navdata.h \
    video.h \
    gauge.h \
    gaugelabel.h \
    gaugehorizon.h \
    mainwindow.h \
    paddetection.h \
    joystick.h \
    gamesettingsdialog.h \
    gamemode.h \
    gamemodedronetaxi.h \
    gamemoderace.h \
    gamemodefreeflight.h \
    moresettingsdialog.h \
    sdljoystick.h \
    helpdialog.h \
    intro.h

FORMS += mainwindow.ui \
    gamesettingsdialog.ui \
    moresettingsdialog.ui \
    helpdialog.ui

OTHER_FILES += \
    debian/changelog \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/README \
    debian/rules \
    dronetaxi.desktop.maemo5

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}

unix:!symbian {
    desktopfile.files = $${TARGET}.desktop
    maemo5 {
        desktopfile.path = /usr/share/applications/hildon
        icon64.path = /usr/share/icons/hicolor/64x64/apps
        icon64.files = $${TARGET}.png
        INSTALLS += icon64
    } else {
        desktopfile.path = /usr/share/applications
    }
    INSTALLS += desktopfile
}

RESOURCES += \
    dronteaxi_resources.qrc
