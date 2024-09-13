#pragma once
#include "common/zarray.h"
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>

extern "C" {
#include <apriltag/apriltag.h>
#include <apriltag/apriltag_pose.h>
#include <apriltag/tagStandard52h13.h>
#include <apriltag/common/getopt.h>
}

using namespace cv;

class RobotVision {
public:
    RobotVision(int argc, char *argv[]);
    ~RobotVision();
    void loop();
    void drawDetections(Mat& frame, zarray_t* detections);
    void logPoses(zarray_t* detections);
private:
    bool enableDraw;
    getopt_t *getopt;
    TickMeter meter;
    VideoCapture cap;
    apriltag_detector_t *td;
    apriltag_family_t *tf;
};
