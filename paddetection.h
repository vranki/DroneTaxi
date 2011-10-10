#ifndef PADDETECTION_H
#define PADDETECTION_H

#include <ARToolKitPlus/ARToolKitPlus.h>
#include <ARToolKitPlus/TrackerSingleMarker.h>
#include <QObject>
#include <QDebug>

class PadDetection : public QObject
{
    Q_OBJECT
public:
    explicit PadDetection(QObject *parent, int width, int height);
    ~PadDetection();
    void detectPads(uchar * data);
signals:
    void padDetected(int num, float x, float y, float rot);
public slots:
private:
    ARToolKitPlus::TrackerSingleMarker *tracker;
};

#endif // PADDETECTION_H
