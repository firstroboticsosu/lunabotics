#pragma once
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>

extern "C" {
#include <apriltag/apriltag.h>
#include <apriltag/tag36h11.h>
#include <apriltag/tag25h9.h>
#include <apriltag/tag16h5.h>
#include <apriltag/tagCircle21h7.h>
#include <apriltag/tagCircle49h12.h>
#include <apriltag/tagCustom48h12.h>
#include <apriltag/tagStandard41h12.h>
#include <apriltag/tagStandard52h13.h>
#include <apriltag/common/getopt.h>
}

using namespace cv;

class RobotVision {
public:
    RobotVision(int argc, char *argv[]);
    void loop();
    void cleanup();
private:
    getopt_t *getopt;
    TickMeter meter;
    VideoCapture cap;
    apriltag_detector_t *td;
    apriltag_family_t *tf;
    const char *famname;
};
