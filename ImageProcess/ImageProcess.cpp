//
// Created by Robinson on 2023/11/17.
//
#include "ImageProcess.h"

#include <QDebug>

QImage ImageProcess::cvMat2QImage(const cv::Mat &mat) {
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (mat.type() == CV_8UC1) {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for (int i = 0; i < 256; i++) {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++) {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3) {
        // Copy input Mat
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        const auto *pSrc = (const uchar *)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, (qsizetype)mat.step,
                     QImage::Format_RGB888);
        // image = image.rgbSwapped();
        return image.copy();
    } else if (mat.type() == CV_8UC4) {
        const auto *pSrc = (const uchar *)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, (qsizetype)mat.step,
                     QImage::Format_ARGB32);
        return image.copy();
    } else {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        QImage image;
        return image.copy();
    }
}

cv::Mat ImageProcess::MatTranslate(cv::Mat &mat) {
    cv::Mat fgmask;
    fgbg->apply(mat, fgmask);
    cv::morphologyEx(fgmask, fgmask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(fgmask, fgmask, cv::MORPH_CLOSE, kernel);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(fgmask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    double max = 0.0;
    int member = 0;
    auto maxCon = std::max_element(contours.begin(), contours.end(), [](const std::vector<cv::Point>& v1, const std::vector<cv::Point>& v2){
        return cv::contourArea(v1) < cv::contourArea(v2);
    });
    cv::rectangle(mat, cv::boundingRect(*maxCon), cv::Scalar(255, 255, 0), 2);
    return mat;
}

double ImageProcess::calGrayPercent(const cv::Mat &mat) {
    cv::Mat out_mat;
    cv::cvtColor(mat, out_mat, cv::COLOR_BGR2GRAY);
    return cv::mean(out_mat)[0];
}
