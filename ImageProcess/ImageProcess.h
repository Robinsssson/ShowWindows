
// Created by Robinson on 2023/11/17.
//

#ifndef OPENCV_PROJECT_IMAGE_PROCESS_H
#define OPENCV_PROJECT_IMAGE_PROCESS_H

#include <QDebug>
#include <QImage>
#include <QString>
#include <opencv2/opencv.hpp>
using PublicImageProcessFunction = double (*)(cv::Mat &);
class ImageProcess {
   public:
    // QImage cvMat2QImage(const cv::Mat &mat);
    int LKOptricalFlow_times = 3;
    bool LKOptricalFlow_first_entry = false;

    double calGrayPercent(cv::Mat &mat);
    double MatTranslate(cv::Mat &);
    QVector<double> LKOptricalFlow(cv::Mat &frame, cv::Mat &old_gray,
                                   std::vector<cv::Point2f> &p0);
    QVector<QString> ImageProcessList = {"灰度计算", "光流法", "无目标定位"};
    QImage cvMatToQImage(const cv::Mat &inMat);
    static ImageProcess &GetInstance() {
        static ImageProcess imageProcess = ImageProcess();
        return imageProcess;
    }
    void setfunction(int index) {
        m_index = index;
        if (m_index == 1) resetLKOptricalFlow();
    }
    int getfunction(void) { return m_index; }
    void resetLKOptricalFlow() { LKOptricalFlow_first_entry = false; }
    double process_function(cv::Mat &mat) {
        if (m_index == 0) return calGrayPercent(mat);
        if (m_index == 1) {
            if (LKOptricalFlow_first_entry == false) {
                cvtColor(mat, LKOptricalFlow_old_gray, cv::COLOR_BGR2GRAY);
                cv::goodFeaturesToTrack(LKOptricalFlow_old_gray,
                                        LKOptricalFlow_p0, 100, 0.3, 20);
                LKOptricalFlow_mask = cv::Mat::zeros(mat.size(), mat.type());
                LKOptricalFlow_color =
                    QVector<cv::Scalar>(LKOptricalFlow_times);
                for (auto &i : LKOptricalFlow_color)
                    i = cv::Scalar(std::rand() % 255, std::rand() % 255,
                                   std::rand() % 255);
                LKOptricalFlow_first_entry = true;
                return 0.;
            }
            auto val =
                LKOptricalFlow(mat, LKOptricalFlow_old_gray, LKOptricalFlow_p0);
            if (cvIsNaN(val[0])) return 0.;
            return val[0];
        }
        if (m_index == 2) return MatTranslate(mat);
        return 0.0;
    }

   private:
    ImageProcess() {
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        fgbg = cv::createBackgroundSubtractorMOG2();
    }
    PublicImageProcessFunction function;
    int m_index = 0;
    cv::Mat kernel;
    cv::Ptr<cv::BackgroundSubtractorMOG2> fgbg;

    cv::Mat LKOptricalFlow_old_gray;
    std::vector<cv::Point2f> LKOptricalFlow_p0;
    cv::Mat LKOptricalFlow_mask;
    QVector<cv::Scalar> LKOptricalFlow_color;
    QVector<QVector<double>> LKOptricalFlow_frame_points;
};

#endif  // OPENCV_PROJECT_IMAGE_PROCESS_H
