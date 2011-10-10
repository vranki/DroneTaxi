#ifndef VIDEO_H
#define VIDEO_H

#include <QGraphicsWidget>
#include <QGraphicsItem>
#include <QPainter>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include "dronecontrol.h"
#include "paddetection.h"


#undef _GNU_SOURCE // just get rid of error message double definition
// #define TARGET_CPU_ARM 1
#include <navdata.h>
#include <VP_Os/vp_os_malloc.h>
#include <VP_Os/vp_os_print.h>


extern "C"
{
#include <VLIB/Stages/vlib_stage_decode.h>
void vp_stages_YUV420P_to_RGB565(void *cfg, vp_api_picture_t *picture, uint8_t *dst, uint32_t dst_rbytes);

}

#define FRONT_VIDEO_WIDTH 320.0f
#define FRONT_VIDEO_HEIGHT 240.0f
#define BOTTOM_VIDEO_WIDTH 176.0f
#define BOTTOM_VIDEO_HEIGHT 144.0f

#define DETECT_ON_NTH_FRAME 3

#define PAD_LABEL_SIZE 32

#define MAX_NUM_PADS 5

class VideoThread;

class DroneVideo : public QGraphicsWidget
{
    Q_OBJECT
public:
    DroneVideo();
    void reset();
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
public slots:
    void frameUpdated();
    void padDetected(int num, float x, float y, float rot);
    void viewBottomCamera(bool);
    void enableVideo(bool enabled);
signals:
    void padVisible(int num);
    void videoReceived();
private:
    QHostAddress droneHost;  // Ip address of the drone
    VideoThread *videoThread;
    QImage *image;
    PadDetection *padDetection;
    int padX, padY, padNum;
    int videoWidth, videoHeight;
    QFont padFont;
    QPixmap pad[MAX_NUM_PADS];
public: // for testing
    bool initialized, bottomCamera, videoHasBeenReceived;
    bool videoEnabled;
};

class VideoThread:public QThread {
    Q_OBJECT
public:
    VideoThread(DroneVideo *parentp,QHostAddress host,QImage *_image);
    ~VideoThread();
    void run();
    void sendVideoPort(QString cmd);
    void decodeTransform(QByteArray &videoData);
    QImage *imageData();
    void setFrameSkip(int f);
public slots:
    void videoDataReady();
    void timer();

signals:
    void frameUpdated();

private:
    QImage *image;
    video_controller_t controller;
    vp_api_picture_t picture;
    int pictureWidth;
    int pictureHeight;
    bool luma_only;
    unsigned int num_picture_decoded;
    QHostAddress droneHost;  // Ip address of the drone
    QTimer stateTimer;
    volatile bool stopped;
    DroneVideo *parent;
    QUdpSocket videoSock;  // Navigation data receive socket port 5554
    int videoFrameNumber;
    int frameSkip;
};

#endif // VIDEO_H
