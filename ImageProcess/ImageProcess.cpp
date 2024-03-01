//
// Created by Robinson on 2023/11/17.
//
#include "ImageProcess.h"

#include <QDebug>

QImage ImageProcess::cvMatToQImage(const cv::Mat &inMat) {
    switch (inMat.type()) {
        // 8-bit, 4 channel
        case CV_8UC4: {
            QImage image(inMat.data, inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step), QImage::Format_ARGB32);

            return image;
        }
        // 8-bit, 3 channel
        case CV_8UC3: {
            QImage image(inMat.data, inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step), QImage::Format_RGB888);

            return image.rgbSwapped();
        }
        // 8-bit, 1 channel
        case CV_8UC1: {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image(inMat.data, inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step),
                         QImage::Format_Grayscale8);
#else
            static QVector<QRgb> sColorTable;

            // only create our color table the first time
            if (sColorTable.isEmpty()) {
                sColorTable.resize(256);

                for (int i = 0; i < 256; ++i) {
                    sColorTable[i] = qRgb(i, i, i);
                }
            }

            QImage image(inMat.data, inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step), QImage::Format_Indexed8);

            image.setColorTable(sColorTable);
#endif
            return image;
        }
        default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not "
                          "handled in switch:"
                       << inMat.type();
            break;
    }
    return QImage();
}

double ImageProcess::MatTranslate(cv::Mat &mat) {
    cv::Mat fgmask;
    fgbg->apply(mat, fgmask);
    cv::morphologyEx(fgmask, fgmask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(fgmask, fgmask, cv::MORPH_CLOSE, kernel);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(fgmask, contours, hierarchy, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty()) return 0;
    double max = 0.0;
    int member = 0;
    auto maxCon = std::max_element(
        contours.begin(), contours.end(),
        [](const std::vector<cv::Point> &v1, const std::vector<cv::Point> &v2) {
            return cv::contourArea(v1) < cv::contourArea(v2);
        });
    auto rect = cv::boundingRect(*maxCon);
    cv::rectangle(mat, rect, cv::Scalar(255, 255, 0), 2);
    return rect.x + rect.width * 0.5;
}

double ImageProcess::calGrayPercent(const cv::Mat &mat) {
    cv::Mat out_mat;
    cv::cvtColor(mat, out_mat, cv::COLOR_BGR2GRAY);
    return cv::mean(out_mat)[0];
}
