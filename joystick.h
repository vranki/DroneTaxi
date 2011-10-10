#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QWidget>
#include <QMouseEvent>

#include "sdljoystick.h"

#define STICK_SIZE 200

class Joystick : public QWidget
{
    Q_OBJECT
public:
    explicit Joystick(QWidget *parent = 0);
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    bool supportsZ();
protected:
    void paintEvent(QPaintEvent *event);

    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );

signals:
    void stickPosChanged(QPointF pos, float z);
    void landButtonPressed();
    void panicButtonPressed();
    void rollChanged(float r);

public slots:
    void sdlAxisEvent(int axis, int value);
    void sdlButtonPress(int button);

private:
    void setStickPos(QPoint pos);
    bool mouseIsPressed;
    float curX, curY, curZ;
    SdlJoystick sixaxis;
    bool stickConnected;
};

#endif // JOYSTICK_H
