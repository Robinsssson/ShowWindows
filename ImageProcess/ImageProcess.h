
// Created by Robinson on 2023/11/17.
//

#ifndef OPENCV_PROJECT_IMAGE_PROCESS_H
#define OPENCV_PROJECT_IMAGE_PROCESS_H

#include <QDebug>
#include <QImage>
#include <QString>
#include <QtAlgorithms>
#include <QtMath>
#include <opencv2/opencv.hpp>
using PublicImageProcessFunction = double (*)(cv::Mat &);
class ImageProcess {
public:
    // QImage cvMat2QImage(const cv::Mat &mat);
    int LKOptricalFlow_times = 3;
    bool LKOptricalFlow_first_entry = true;

    double calGrayPercent(cv::Mat &mat);
    double MatTranslate(cv::Mat &);
    QVector<double> LKOptricalFlow(cv::Mat &frame, cv::Mat &old_gray, std::vector<cv::Point2f> &p0, cv::Rect *rect);
    QVector<QString> ImageProcessList = {"灰度计算", "光流法", "无目标定位", "像素级别有目标"};

    int getProcessIndex(const QString &str) const { return ImageProcessList.indexOf(str); }
    QString getProcessName(int index) const { return ImageProcessList.at(index); }

    QImage cvMatToQImage(const cv::Mat &inMat);
    static ImageProcess &GetInstance() {
        static ImageProcess imageProcess = ImageProcess();
        return imageProcess;
    }
    void setfunction(int index) {
        m_index = index;
        resetFunctionConfig();
    }
    int getIndex(void) const { return m_index; }
    auto getName() const { return ImageProcessList[m_index]; }

    void resetFunctionConfig() {
        LKOptricalFlow_first_entry = true;
        m_pixelregistration_first_entry = true;
    }

    std::vector<double> pixelregistration(cv::Mat &frame, cv::Mat &old_gray, cv::Rect *rect);

    double process_function(cv::Mat &mat, cv::Rect *rect = nullptr) {
        if (m_index == 0) return calGrayPercent(mat);
        if (m_index == 1) {
            if (LKOptricalFlow_first_entry == true) {
                if (rect == nullptr)
                    cvtColor(mat, LKOptricalFlow_old_gray, cv::COLOR_BGR2GRAY);
                else
                    cvtColor(mat(*rect), LKOptricalFlow_old_gray, cv::COLOR_BGR2GRAY); /**/
                cv::goodFeaturesToTrack(LKOptricalFlow_old_gray, LKOptricalFlow_p0, 100, 0.3, 20);
                LKOptricalFlow_mask = cv::Mat::zeros(mat.size(), mat.type());
                LKOptricalFlow_color = QVector<cv::Scalar>(LKOptricalFlow_times);
                for (auto &i : LKOptricalFlow_color) i = cv::Scalar(std::rand() % 255, std::rand() % 255, std::rand() % 255);
                LKOptricalFlow_first_entry = false;
                return 0.;
            }
            auto ret = LKOptricalFlow(mat, LKOptricalFlow_old_gray, LKOptricalFlow_p0, rect);
            return cvIsNaN(ret[0]) ? 0.0 : ret[0];
        }
        if (m_index == 2) return MatTranslate(mat);
        if (m_index == 3) {
            if (m_pixelregistration_first_entry == true) {
                if (rect == nullptr) {
                    cvtColor(mat, pixelregistration_old_gray, cv::COLOR_BGR2GRAY);
                } else {
                    cvtColor(mat(*rect), pixelregistration_old_gray, cv::COLOR_BGR2GRAY);
                }
                m_pixelregistration_first_entry = false;
            }
            auto ret = pixelregistration(mat, pixelregistration_old_gray, rect);
            return cvIsNaN(ret[0]) ? 0.0 : ret[0];
        }
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
    bool m_pixelregistration_first_entry = true;
    cv::Mat pixelregistration_old_gray;
    cv::Mat LKOptricalFlow_old_gray;
    std::vector<cv::Point2f> LKOptricalFlow_p0;
    cv::Mat LKOptricalFlow_mask;
    QVector<cv::Scalar> LKOptricalFlow_color;
    QVector<QVector<double>> LKOptricalFlow_frame_points;
};

#endif  // OPENCV_PROJECT_IMAGE_PROCESS_H
