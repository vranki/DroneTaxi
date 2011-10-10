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
#include "video.h"
#include <QGraphicsView>

DroneVideo::DroneVideo() : videoWidth(FRONT_VIDEO_WIDTH), videoHeight(FRONT_VIDEO_HEIGHT)
{
    droneHost.setAddress("192.168.1.1");
    setPreferredSize(videoWidth,videoHeight);
    padDetection = new PadDetection(this, videoWidth, videoHeight);
    connect(padDetection, SIGNAL(padDetected(int,float,float, float)), this, SLOT(padDetected(int,float,float,float)));
    padFont = font();
    padFont.setPixelSize(PAD_LABEL_SIZE);
    image = 0;
    videoThread = 0;
    videoEnabled = true;
    pad[0] = QPixmap(":/data/data/dt_pad1.png");
    pad[1] = QPixmap(":/data/data/dt_pad2.png");
    pad[2] = QPixmap(":/data/data/dt_pad3.png");
    pad[3] = QPixmap(":/data/data/dt_pad4.png");
    pad[4] = QPixmap(":/data/data/dt_pad5.png");
    Q_ASSERT(!pad[0].isNull());
    reset();
}

void DroneVideo::reset() {
    videoHasBeenReceived = false;
    initialized=false;
    viewBottomCamera(false);
    if(image) {
        delete image;
        image = 0;
    }
    if(videoThread) {
        delete videoThread;
        videoThread = 0;
    }
}

VideoThread::VideoThread(DroneVideo *parentp,QHostAddress host,QImage *_image)
{
    qDebug() << Q_FUNC_INFO;
    image=_image;
    stopped=false;
    parent=parentp;
    videoSock.bind(QHostAddress::Any,5555);
    droneHost=host;
    videoFrameNumber = 0;
    frameSkip = 0;
    start();
};

VideoThread::~VideoThread() {
    videoSock.close();
}

void DroneVideo::paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!initialized) { // We need initialize QImage here because we don't know display depth before
        int depth=0;

        depth=painter->device()->depth();
        //        qDebug() << "depth=" << depth;
        /*
        if(depth==124) {
            image=new QImage(320,240, QImage::Format_RGB32);
            image->fill(0x555555);
        } else {*/
        image=new QImage(videoWidth,videoHeight, QImage::Format_RGB16);
        image->fill(0x5555);
        //}
        QPainter p(image);
        p.drawLine(0,0,image->width(),image->height());
        p.drawLine(image->width(),0,0,image->height());
        p.drawText(boundingRect(), "Waiting for video..");
        videoThread=new VideoThread(this,droneHost,image);
        connect(videoThread, SIGNAL(frameUpdated()), this, SLOT(frameUpdated()), Qt::QueuedConnection);
        viewBottomCamera(false);
        initialized=true;
        update(boundingRect());
    } else {
        if(videoEnabled) {
            QRect sourceRect;
            if(bottomCamera) {
                sourceRect.setRect(0,0,BOTTOM_VIDEO_WIDTH, BOTTOM_VIDEO_HEIGHT);
            } else {
                sourceRect.setRect(0,0,FRONT_VIDEO_WIDTH, FRONT_VIDEO_HEIGHT);
            }
            painter->drawImage(boundingRect(),*image,sourceRect);
            painter->setPen(Qt::white);
            painter->setBrush(Qt::darkGray);
            painter->setFont(padFont);
            if(padNum) {
                if(padNum > 0 && padNum < MAX_NUM_PADS+1) { // Draw the fancy pad
                    painter->drawPixmap(padX - pad[0].width()/2, padY - pad[0].height()/2, pad[padNum-1]);
                } else { // Fallback pad
                    painter->drawRect(padX-PAD_LABEL_SIZE/2, padY+PAD_LABEL_SIZE/2, PAD_LABEL_SIZE*3,-PAD_LABEL_SIZE);
                    painter->drawText(padX-PAD_LABEL_SIZE/2, padY+PAD_LABEL_SIZE/2-5, "Pad " + QString::number(padNum));
                }
            }
        }
    }
};

QRectF DroneVideo::boundingRect() const
{
    return QRectF(0.0,0.0,FRONT_VIDEO_WIDTH, FRONT_VIDEO_HEIGHT);
}

void DroneVideo::viewBottomCamera(bool bottom) {
    bottomCamera = bottom;
    videoWidth = FRONT_VIDEO_WIDTH;
    videoHeight = FRONT_VIDEO_HEIGHT;
    emit padDetected(0,0,0,0);
    if(videoThread) {
        if(bottom) {
            videoThread->setFrameSkip(DETECT_ON_NTH_FRAME);
        } else {
            videoThread->setFrameSkip(0);
        }
    }
    /*
    if(bottom) {
        videoWidth = BOTTOM_VIDEO_WIDTH;
        videoHeight = BOTTOM_VIDEO_HEIGHT;
//        setScale((float)FRONT_VIDEO_WIDTH/(float)BOTTOM_VIDEO_WIDTH);
    } else {
        videoWidth = FRONT_VIDEO_WIDTH;
        videoHeight = FRONT_VIDEO_HEIGHT;
//        setScale(1);
        emit padDetected(0,0,0,0);
    }
    */
    if(scene() && !scene()->views().isEmpty()) {
        //        scene()->views().at(0)->fitInView(this);
        scene()->setSceneRect(0,0,videoWidth, videoHeight);
        qDebug() << Q_FUNC_INFO << videoWidth << videoHeight;
        //        scene()->views().at(0)->
        scene()->views().at(0)->fitInView(0,0,videoWidth, videoHeight);
    }
}

void DroneVideo::frameUpdated() {
    //    static QImage convertedImage;
    if(bottomCamera) {
        //        convertedImage = videoThread->imageData()->convertToFormat(QImage::Format_RGB888);
        //        padDetection->detectPads(convertedImage.bits());

        padDetection->detectPads(videoThread->imageData()->bits());
    }
    update();
    if(!videoHasBeenReceived) {
        emit videoReceived();
        videoHasBeenReceived = true;
    }
}

void DroneVideo::padDetected(int num, float x, float y, float rot) {
    padNum = num;
    padX = (FRONT_VIDEO_WIDTH/2 + x*FRONT_VIDEO_WIDTH*2.5f);
    padY = (FRONT_VIDEO_HEIGHT/2 + y*FRONT_VIDEO_HEIGHT*3.0f);
    padX *=(FRONT_VIDEO_WIDTH/BOTTOM_VIDEO_WIDTH);
    padY *= (FRONT_VIDEO_HEIGHT/BOTTOM_VIDEO_HEIGHT);
    emit padVisible(num);
}

void DroneVideo::enableVideo(bool enabled) {
    videoEnabled = enabled;
    if(!enabled && image)
        image->fill(0);
}

void VideoThread::run()
{
#define ACQ_WIDTH     320
#define ACQ_HEIGHT    240
#undef memset
    memset(&controller,0,sizeof(controller));
    memset(&picture,0,sizeof(picture));
    pictureWidth= image->width();
    pictureHeight=image->height();
    int codec_type=UVLC_CODEC;
    connect(&stateTimer,SIGNAL(timeout()),this,SLOT(timer()));
    connect(&videoSock,SIGNAL(readyRead()),this,SLOT(videoDataReady()));
    luma_only=FALSE;
    num_picture_decoded=0;
    /// Picture configuration
    picture.format        = PIX_FMT_YUV420P;
    picture.width         = pictureWidth;
    picture.height        = pictureHeight;
    picture.framerate     = 30;
    picture.y_buf         = (uint8_t*)(void*)vp_os_malloc((size_t) pictureWidth*pictureHeight );
    picture.cr_buf        = (uint8_t*)vp_os_malloc( pictureWidth*pictureHeight/4 );
    picture.cb_buf        = (uint8_t*)vp_os_malloc( pictureWidth*pictureHeight/4 );
    picture.y_line_size   = pictureWidth;
    picture.cb_line_size  = pictureWidth / 2;
    picture.cr_line_size  = pictureWidth / 2;
    picture.y_pad         = 0;
    picture.c_pad         = 0;
    video_codec_open(&controller, (codec_type_t)UVLC_CODEC);
    //stateTimer->start(1000);
    sendVideoPort("AT");
    while(!stopped) {
        exec();
    }
}

void VideoThread::timer()
{
    //  qDebug() << "thread Timer";

}

void VideoThread::sendVideoPort(QString cmd)
{
    QByteArray dgram;
    dgram=cmd.toLatin1();
    qDebug() << "videoThread::sendCmd= " << cmd+"\n" << "to " << droneHost ;
    videoSock.writeDatagram(dgram.data(),dgram.size(),droneHost,5555);
}

void VideoThread::videoDataReady()
{
    qint64 l;
    QByteArray videoData;

    QHostAddress host;
    quint16 port;
    videoData.resize(videoSock.pendingDatagramSize());
    l=videoSock.readDatagram(videoData.data(),videoData.size(),&host,&port);
    // qDebug() << "videoThread::videoDataReady" <<" l=" << l << "from"  << host ;
    decodeTransform(videoData);
}

void VideoThread::decodeTransform(QByteArray &videoData)
{
//    qDebug() << Q_FUNC_INFO;
    videoFrameNumber++;
    if(videoFrameNumber < frameSkip) return;
    videoFrameNumber = 0;

    controller.in_stream.bytes   = (uint32_t*)videoData.data();
    controller.in_stream.used    = videoData.size();
    controller.in_stream.size    = videoData.size();
    controller.in_stream.index   = 0;
    controller.in_stream.length  = 32;
    controller.in_stream.code    = 0;

    bool_t got_image = FALSE;
    video_decode_blockline( &controller, &picture, &got_image );
    //qDebug() <<"VideoThread::decodeTransform 2";
    // video_decode_picture( &controller, &picture, &controller.in_stream, &got_image );
    if( got_image )
    {
        //        qDebug() <<"VideoThread::decodeTransform got image" << picture.width << picture.height << image->byteCount() << image->bytesPerLine();
        // we got one picture
        // out->size = 1;
        picture.complete = 1;
        num_picture_decoded++;
        //        memcpy(picture.)
        vp_stages_YUV420P_to_RGB565(NULL,&picture,image->bits(),image->bytesPerLine());
        emit frameUpdated();

        //   qDebug() << "pic " << num_picture_decoded;
    }
};


QImage *VideoThread::imageData() {
    return image;
}

void VideoThread::setFrameSkip(int f){
    frameSkip = f;
}
