//
// Created by Robinson on 2023/11/17.
//

#ifndef OPENCV_PROJECT_IMAGE_PROCESS_H
#define OPENCV_PROJECT_IMAGE_PROCESS_H

#include <QImage>
#include <opencv2/opencv.hpp>

class ImageProcess {
public:
    static QImage cvMat2QImage(const cv::Mat &mat);
    static double calGrayPercent(const cv::Mat& mat);
    static cv::Mat MatTranslate(const cv::Mat &);
};

#endif //OPENCV_PROJECT_IMAGE_PROCESS_H
