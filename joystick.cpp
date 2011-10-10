#include "joystick.h"
#include <QPainter>
#include <QDebug>

Joystick::Joystick(QWidget *parent) : QWidget(parent), mouseIsPressed(false),
    curX(0), curY(0), curZ(0), sixaxis(-1)
{
    resize(STICK_SIZE, STICK_SIZE);
    setMinimumHeight(STICK_SIZE);
    setMinimumWidth(STICK_SIZE);
    setMouseTracking(true);
    stickConnected = false;
    if(sixaxis.attachedJoysticks()) {
        if(sixaxis.joystickName(0)=="Sony PLAYSTATION(R)3 Controller") {
            sixaxis.setJoystick(0);
            qDebug() << Q_FUNC_INFO << "Joystick name:" << sixaxis.joystickName(0);
            connect(&sixaxis, SIGNAL(axisEvent(int,int)), this, SLOT(sdlAxisEvent(int,int)));
            connect(&sixaxis, SIGNAL(buttonPress(int)), this, SLOT(sdlButtonPress(int)));
            stickConnected = true;
        }
    }
}

void Joystick::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setBackground(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,width(),height());
    int handleSize = width()/10;
    if(stickConnected) {
        painter.setBrush(Qt::red);
        painter.drawRect(width()/2 - handleSize/2 + curZ*width()/2,
                         height()/2 - handleSize/2,
                         handleSize,handleSize);
    }
    painter.setBrush(Qt::green);
    painter.drawRect(width()/2 - handleSize/2 + curX*width()/2,
                     height()/2 - handleSize/2 + curY*height()/2,
                     handleSize,handleSize);
}

void Joystick::mousePressEvent ( QMouseEvent * event ) {
    if(event->button()==Qt::LeftButton) {
        mouseIsPressed = true;
        setStickPos(event->pos());
    }
}

void Joystick::mouseReleaseEvent ( QMouseEvent * event ) {
    if(event->button()==Qt::LeftButton) {
        mouseIsPressed = false;
        setStickPos(QPoint(width()/2,height()/2));
    }
}

void Joystick::mouseMoveEvent ( QMouseEvent * event ) {
    if(mouseIsPressed)
        setStickPos(event->pos());
}

void Joystick::setX(float x) {
    Q_ASSERT(x >= -1 && x <= 1);
    curX = x;
    emit stickPosChanged(QPointF(curX, curY), curZ);
    update();
}

void Joystick::setY(float y) {
    Q_ASSERT(y >= -1 && y <= 1);
    curY = y;
    emit stickPosChanged(QPointF(curX, curY), curZ);
    update();
}

void Joystick::setZ(float z) {
    Q_ASSERT(z >= -1 && z <= 1);
    curZ = z;
    emit stickPosChanged(QPointF(curX, curY), curZ);
    update();
}

void Joystick::setStickPos(QPoint pos) {
    float x = ((float)pos.x() - (float)width()/2) / ((float)width()/2);
    float y = ((float)pos.y() - (float)height()/2) / ((float)height()/2);
    qDebug() << Q_FUNC_INFO << pos << x << y;
    x = qMin(x, 1.0f);
    x = qMax(x, -1.0f);
    y = qMin(y, 1.0f);
    y = qMax(y, -1.0f);
    bool changed = (x!=curX || y!=curY);
    if(!changed) return;
    curX = x;
    curY = y;
    emit stickPosChanged(QPointF(curX, curY), curZ);
    update();
}

void Joystick::sdlAxisEvent(int axis, int value) {
    if(axis==0)
        setZ((float)value/32768.0f);
    if(axis==2)
        setX((float)value/32768.0f);
    if(axis==3)
        setY((float)value/32768.0f);
}

void Joystick::sdlButtonPress(int button){
    if(button == 12)
        emit panicButtonPressed();
    if(button == 14)
        emit landButtonPressed();
}

bool Joystick::supportsZ() {
    return stickConnected;
}
