#ifndef INTRO_H
#define INTRO_H

#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QTime>

class Intro : public QMainWindow
{
    Q_OBJECT
public:
    explicit Intro(QWidget *parent = 0);
    void run();
    void mousePressEvent ( QMouseEvent * event );

signals:

public slots:
    void tick();
private:
    QPixmap bg, drone, logo;
    QGraphicsPixmapItem *bgItem, *droneItem, *logoItem;
    QGraphicsView view;
    QGraphicsScene scene;
    QTimer timer, endTimer;
    QTime time;
};

#endif // INTRO_H
