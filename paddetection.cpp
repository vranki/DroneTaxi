#include "paddetection.h"

PadDetection::PadDetection(QObject *parent, int width, int height) :
    QObject(parent)
{
    // create a tracker that does:
    //  - 6x6 sized marker images
    //  - samples at a maximum of 6x6
    //  - works with luminance (gray) images
    //  - can load a maximum of 1 pattern
    //  - can detect a maximum of 8 patterns in one image
    tracker = new ARToolKitPlus::TrackerSingleMarker(width, height);
    if(!tracker->init("data/no_distortion.cal", 1.0f, 1000.0f)) {
        qDebug() << Q_FUNC_INFO << "Tracker init failed";
        delete tracker;
        tracker = 0;
        return;
    }
    tracker->getCamera()->printSettings();

//    tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_RGB);
    tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_RGB565);
    // define size of the marker
    tracker->setPatternWidth(2);

    // the marker in the BCH test image has a thin border...
    tracker->setBorderWidth(0.250f);
//    tracker->setBorderWidth(0.1250f);

    // set a threshold. alternatively we could also activate automatic thresholding
  //  tracker->setThreshold(150);
    tracker->activateAutoThreshold(true);

    // let's use lookup-table undistortion for high-speed
    // note: LUT only works with images up to 1024x1024
    tracker->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);

    // RPP is more robust than ARToolKit's standard pose estimator
   // tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);

    // switch to simple ID based markers
    // use the tool in tools/IdPatGen to generate markers
    tracker->setMarkerMode(ARToolKitPlus::MARKER_ID_SIMPLE);
    // tracker->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);

    // tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL)
}
PadDetection::~PadDetection() {
    if(tracker)
        delete tracker;
}

void PadDetection::detectPads(uchar * data) {
    if(!tracker) return;
    std::vector<int> markerIds = tracker->calc(data);
    tracker->selectBestMarkerByCf();
    float conf = tracker->getConfidence();
    if(conf > 0) {
       // qDebug() << Q_FUNC_INFO << "Confidence " << conf << "marker " << markerIds[0];
        /*
        qDebug() << "MVM " << tracker->getModelViewMatrix()[12] << tracker->getModelViewMatrix()[13] << tracker->getModelViewMatrix()[14];
        qDebug() << "PM " << tracker->getProjectionMatrix()[0] << tracker->getProjectionMatrix()[1] << tracker->getProjectionMatrix()[2] << tracker->getProjectionMatrix()[3] << "\n"
                 << tracker->getProjectionMatrix()[4] << tracker->getProjectionMatrix()[5] << tracker->getProjectionMatrix()[6] << tracker->getProjectionMatrix()[7] << "\n"
                 << tracker->getProjectionMatrix()[8];
                 */
        float x, y, z;
        x = tracker->getModelViewMatrix()[12];
        y = tracker->getModelViewMatrix()[13];
        z = tracker->getModelViewMatrix()[14];
        emit padDetected(markerIds[0], x/z, y/z, 0);
    } else {
        emit padDetected(0, 0,0,0);
    }
}
