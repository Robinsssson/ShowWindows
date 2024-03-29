
// Created by Robinson on 2023/11/17.
//

#ifndef OPENCV_PROJECT_IMAGE_PROCESS_H
#define OPENCV_PROJECT_IMAGE_PROCESS_H

#include <QImage>
#include <opencv2/opencv.hpp>
// using PublicImageProcessFunction = double (*) (cv::Mat &);
class ImageProcess {
   public:
    // QImage cvMat2QImage(const cv::Mat &mat);
    double calGrayPercent(const cv::Mat &mat);
    double MatTranslate(cv::Mat &);
    QImage cvMatToQImage(const cv::Mat &inMat);
    static ImageProcess &GetInstance() {
        static ImageProcess imageProcess = ImageProcess();
        return imageProcess;
    }

   private:
    ImageProcess() {
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        fgbg = cv::createBackgroundSubtractorMOG2();
    }
    // PublicImageProcessFunction function;
    cv::Mat kernel;
    cv::Ptr<cv::BackgroundSubtractorMOG2> fgbg;
};

#endif  // OPENCV_PROJECT_IMAGE_PROCESS_H
