#include "intro.h"
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QGraphicsTextItem>

Intro::Intro(QWidget *parent) :
        QMainWindow(parent)
{
    bg = QPixmap(":/data/data/dronentaxi-tausta.png");
    drone = QPixmap(":/data/data/dronetaxi.png");
    logo = QPixmap(":/data/data/dronetaxi-title.png");
    Q_ASSERT(bg);
    Q_ASSERT(drone);
    Q_ASSERT(logo);
    view.setScene(&scene);
    setCentralWidget(&view);
    bgItem = new QGraphicsPixmapItem(bg);
    scene.addItem(bgItem);
    droneItem = new QGraphicsPixmapItem(drone);
    scene.addItem(droneItem);
    logoItem = new QGraphicsPixmapItem(logo);
    scene.addItem(logoItem);
    view.setBackgroundBrush(Qt::black);
    bgItem->setPos(0,0);
    bgItem->setZValue(-10);
    logoItem->setZValue(10);
    logoItem->setPos(0,0);
    timer.setInterval(16);
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
    timer.setSingleShot(false);

    endTimer.setInterval(1000);
//    connect(&endTimer, SIGNAL(timeout()), this, SLOT(deleteLater()));
}

void Intro::run() {
    showFullScreen();
    view.show();
    timer.start();
    time.restart();
    view.fitInView(bgItem, Qt::KeepAspectRatio);
}

void Intro::tick() {
    int t = time.elapsed();
    int introTime = 5000;
    if(t > introTime) {
        QFont credsFont = scene.font();
        credsFont.setBold(true);
        credsFont.setPixelSize(10);
        QGraphicsTextItem *creds = new QGraphicsTextItem("By Ville Ranki <ville.ranki@iki.fi>\n"
                                                         "Artwork by Saila Juuti\n\n"
                                                         "Tap screen to continue");
        creds->setPos(bg.width()*0.9-creds->boundingRect().width(), bg.height()*0.1);
        creds->setDefaultTextColor(Qt::white);
        creds->setFont(credsFont);

        scene.addItem(creds);
        endTimer.start();
        timer.stop();
        return;
    }
    float ft = (float) t/(float) introTime;
    logoItem->setOpacity(ft);
    droneItem->setPos(logo.width()*2 - ft*logo.width(), logo.height()/2 + ft*(logo.height()/2));
    droneItem->setScale(ft);
}

void Intro::mousePressEvent ( QMouseEvent * event ){
    hide();
    deleteLater();
}
