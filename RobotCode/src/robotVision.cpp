#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "robotVision.h"

using namespace std;
using namespace cv;

RobotVision::RobotVision(int argc, char *argv[]) {
    getopt = getopt_create();
    cout << "Enabling video capture" << endl;
    
    getopt_add_bool(getopt, 'h', "help", 0, "Show this help");
    getopt_add_int(getopt, 'c', "camera", "0", "camera ID");
    getopt_add_bool(getopt, 'd', "debug", 0, "Enable debugging output (slow)");
    getopt_add_bool(getopt, 'q', "quiet", 0, "Reduce output");
    getopt_add_string(getopt, 'f', "family", "tag36h11", "Tag family to use");
    getopt_add_int(getopt, 't', "threads", "1", "Use this many CPU threads");
    getopt_add_double(getopt, 'x', "decimate", "2.0", "Decimate input image by this factor");
    getopt_add_double(getopt, 'b', "blur", "0.0", "Apply low-pass blur to input");
    getopt_add_bool(getopt, '0', "refine-edges", 1, "Spend more time trying to align edges of tags");

    if (!getopt_parse(getopt, argc, argv, 1) ||
            getopt_get_bool(getopt, "help")) {
        printf("Usage: %s [options]\n", argv[0]);
        getopt_do_usage(getopt);
        exit(0);
    }

    meter.start();

    // Initialize camera
    cap.open(getopt_get_int(getopt, "camera"));
    if (!cap.isOpened()) {
        cerr << "Couldn't open video capture device" << endl;
        return;
    }
    
    // Initialize tag detector with options
    tf = tagStandard52h13_create();

    td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);

    if (errno == ENOMEM) {
        cerr << "Unable to add family to detector due to insufficient memory. Try choosing an alternative tag family.\n";
        exit(-1);
    }

    td->quad_decimate = getopt_get_double(getopt, "decimate");
    td->quad_sigma = getopt_get_double(getopt, "blur");
    td->nthreads = getopt_get_int(getopt, "threads");
    td->debug = getopt_get_bool(getopt, "debug");
    td->refine_edges = getopt_get_bool(getopt, "refine-edges");

    enableDraw = td->debug && !getopt_get_bool(getopt, "quiet");

    meter.stop();
    cout << "Detector for 52h13 initialized in "
        << std::fixed << std::setprecision(3) << meter.getTimeSec() << " seconds" << endl;
#if CV_MAJOR_VERSION > 3
    cout << "  " << cap.get(CAP_PROP_FRAME_WIDTH ) << "x" <<
                    cap.get(CAP_PROP_FRAME_HEIGHT ) << " @" <<
                    cap.get(CAP_PROP_FPS) << "FPS" << endl;
#else
    cout << "  " << cap.get(CV_CAP_PROP_FRAME_WIDTH ) << "x" <<
                    cap.get(CV_CAP_PROP_FRAME_HEIGHT ) << " @" <<
                    cap.get(CV_CAP_PROP_FPS) << "FPS" << endl;
#endif
    meter.reset();
}

RobotVision::~RobotVision() {
    apriltag_detector_destroy(td);
    tagStandard52h13_destroy(tf);
    getopt_destroy(getopt);
}

void RobotVision::drawDetections(Mat& frame, zarray_t* detections) {
    if (!enableDraw) {
        return;
    }

    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);
        line(frame, Point(det->p[0][0], det->p[0][1]),
                 Point(det->p[1][0], det->p[1][1]),
                 Scalar(0, 0xff, 0), 2);
        line(frame, Point(det->p[0][0], det->p[0][1]),
                 Point(det->p[3][0], det->p[3][1]),
                 Scalar(0, 0, 0xff), 2);
        line(frame, Point(det->p[1][0], det->p[1][1]),
                 Point(det->p[2][0], det->p[2][1]),
                 Scalar(0xff, 0, 0), 2);
        line(frame, Point(det->p[2][0], det->p[2][1]),
                 Point(det->p[3][0], det->p[3][1]),
                 Scalar(0xff, 0, 0), 2);

        stringstream ss;
        ss << det->id;
        String text = ss.str();
        int fontface = FONT_HERSHEY_SCRIPT_SIMPLEX;
        double fontscale = 1.0;
        int baseline;
        Size textsize = getTextSize(text, fontface, fontscale, 2, &baseline);
        putText(frame, text, Point(det->c[0] - textsize.width / 2,
                                   det->c[1] + textsize.height / 2),
                fontface, fontscale, Scalar(0xff, 0x99, 0), 2);
    }
}

void RobotVision::loop() {
    Mat frame, gray;
    
    while (true) {
        errno = 0;
        cap >> frame;
        if (frame.empty()) {
            cerr << "Failed to capture image" << endl;
            break;
        }

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Make an image_u8_t header for the Mat data
        image_u8_t im = {gray.cols, gray.rows, gray.cols, gray.data};

        zarray_t *detections = apriltag_detector_detect(td, &im);

        if (errno == EAGAIN) {
            cerr << "Unable to create the " << td->nthreads << " threads requested.\n";
            exit(-1);
        }

        drawDetections(frame, detections);

        apriltag_detections_destroy(detections);

        if (enableDraw) {
            imshow("Tag Detections", frame);
            if (waitKey(30) >= 0)
                break;
        }
    }
}
