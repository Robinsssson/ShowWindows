//
// Created by Robinson on 2023/11/17.
//
#include "ImageProcess.h"

#include <QDebug>

QImage ImageProcess::cvMatToQImage(const cv::Mat &inMat) {
    switch (inMat.type()) {
        // 8-bit, 4 channel
        case CV_8UC4: {
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_ARGB32);

            return image;
        }
        // 8-bit, 3 channel
        case CV_8UC3: {
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_RGB888);

            return image.rgbSwapped();
        }
        // 8-bit, 1 channel
        case CV_8UC1: {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_Grayscale8);
#else
            static QVector<QRgb> sColorTable;

            // only create our color table the first time
            if (sColorTable.isEmpty()) {
                sColorTable.resize(256);

                for (int i = 0; i < 256; ++i) {
                    sColorTable[i] = qRgb(i, i, i);
                }
            }

            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_Indexed8);

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
    cv::findContours(fgmask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty()) return 0;
    double max = 0.0;
    int member = 0;
    auto maxCon = std::max_element(contours.begin(), contours.end(), [](const std::vector<cv::Point> &v1, const std::vector<cv::Point> &v2) { return cv::contourArea(v1) < cv::contourArea(v2); });
    auto rect = cv::boundingRect(*maxCon);
    cv::rectangle(mat, rect, cv::Scalar(255, 255, 0), 2);
    return rect.x + rect.width * 0.5;
}

double ImageProcess::calGrayPercent(cv::Mat &mat) {
    cv::Mat out_mat;
    cv::cvtColor(mat, out_mat, cv::COLOR_BGR2GRAY);
    return cv::mean(out_mat)[0];
}

QVector<double> ImageProcess::LKOptricalFlow(cv::Mat &frame, cv::Mat &old_gray, std::vector<cv::Point2f> &p0, cv::Rect *rect = nullptr) {
    cv::Mat new_gray;
    if (rect == nullptr)
        cvtColor(frame, new_gray, cv::COLOR_BGR2GRAY);
    else
        cvtColor(frame(*rect), new_gray, cv::COLOR_BGR2GRAY);
    std::vector<cv::Point2f> p1, good_new, good_old;
    std::vector<uchar> status;
    std::vector<float> err;
    cv::calcOpticalFlowPyrLK(old_gray, new_gray, p0, p1, status, err);
    if (rect == nullptr)
        for (auto &point : p1) qDebug() << point.x << "," << point.y;
    else
        for (auto &point : p1) qDebug() << "in rect :" << point.x << "," << point.y;
    for (int i = 0; i < status.size(); i++) {
        if (status[i] == 1) {
            good_new.push_back(p1[i]);
            good_old.push_back(p0[i]);
        }
    }
    if (good_new.size() == 0) {
        ImageProcess::LKOptricalFlow_first_entry = false;
        return QVector<double>(LKOptricalFlow_times, 0);
    }
    cv::Point max_new(good_new[0].x, good_new[0].y), min_new(good_new[0].x, good_new[0].y);
    for (cv::Point2f point : good_new) {
        max_new.x = std::max(max_new.x, (int)point.x);
        max_new.y = std::max(max_new.y, (int)point.y);
        min_new.x = std::min(min_new.x, (int)point.x);
        min_new.y = std::min(min_new.y, (int)point.y);
    }
    // tag.
    std::vector<std::vector<float>> sum(LKOptricalFlow_times);
    for (int iter = 0; iter < good_old.size(); iter++) {
        cv::Point2f old = good_old[iter], news = good_new[iter];
        for (int i = 0; i < LKOptricalFlow_times; i++) {
            if (((int)news.y - min_new.y) < ((max_new.y - min_new.y) / LKOptricalFlow_times) * (i + 1)) {
                if (((int)news.y - min_new.y) > ((max_new.y - min_new.y) / LKOptricalFlow_times) * (i)) {
                    sum[i].push_back(news.x);
                    if (rect != nullptr) {
                        cv::Point2f absolute_news(rect->x + news.x, rect->y + news.y); /**/
                        cv::Point2f absolute_old(rect->x + old.x, rect->y + old.y);    /**/
                        cv::line(LKOptricalFlow_mask, absolute_news, absolute_old,     /**/
                            LKOptricalFlow_color[i], 2);
                        cv::circle(frame, absolute_news, 5, LKOptricalFlow_color[i], -1); /**/
                    } else {
                        cv::line(LKOptricalFlow_mask, news, old, LKOptricalFlow_color[i], 2);
                        cv::circle(frame, news, 5, LKOptricalFlow_color[i], -1);
                    }
                }
            }
        }
    }
    QVector<double> mean;
    for (auto &sum_i : sum) mean.push_back(std::accumulate(sum_i.begin(), sum_i.end(), 0.0) / (double)sum_i.size());
    LKOptricalFlow_frame_points.push_back(mean);
    cv::add(LKOptricalFlow_mask, frame, frame);
    old_gray = new_gray.clone();
    p0 = good_new;
    return mean;
}

using namespace cv;
using namespace std;

vector<double> ImageProcess::pixelregistration(cv::Mat &frame, cv::Mat &old_gray, cv::Rect *rect) {
    if (frame.empty() || old_gray.empty()) {
        cout << "ERROR: Could not open or find the images!" << endl;
        return vector<double>();
    }
    cv::Mat new_gray;
    cvtColor(frame(*rect), new_gray, cv::COLOR_BGR2GRAY);
    // Ensure the images have the same size
    if (new_gray.size() != old_gray.size()) {
        cout << "ERROR: Images must have the same size!" << endl;
        return vector<double>();
    }
    int m = new_gray.rows;
    int n = new_gray.cols;

    // Convert input images to float32
    cv::Mat new_gray_float32, old_gray_float32;
    new_gray.convertTo(new_gray_float32, CV_32F);
    old_gray.convertTo(old_gray_float32, CV_32F);

    // Perform DFT
    cv::Mat dft1, dft2;
    dft(new_gray_float32, dft1, cv::DFT_COMPLEX_OUTPUT);
    dft(old_gray_float32, dft2, cv::DFT_COMPLEX_OUTPUT);

    // Shift the zero-frequency component to the center of the spectrum
    dft1 = dft1(cv::Rect(0, 0, dft1.cols & -2, dft1.rows & -2));
    dft2 = dft2(cv::Rect(0, 0, dft2.cols & -2, dft2.rows & -2));
    int cx = dft1.cols / 2;
    int cy = dft1.rows / 2;

    cv::Mat q0(dft1, cv::Rect(0, 0, cx, cy));    // Top-Left - Create a ROI per quadrant
    cv::Mat q1(dft1, cv::Rect(cx, 0, cx, cy));   // Top-Right
    cv::Mat q2(dft1, cv::Rect(0, cy, cx, cy));   // Bottom-Left
    cv::Mat q3(dft1, cv::Rect(cx, cy, cx, cy));  // Bottom-Right

    cv::Mat tmp;  // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);  // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    q0 = cv::Mat(dft2, cv::Rect(0, 0, cx, cy));    // Top-Left - Create a ROI per quadrant
    q1 = cv::Mat(dft2, cv::Rect(cx, 0, cx, cy));   // Top-Right
    q2 = cv::Mat(dft2, cv::Rect(0, cy, cx, cy));   // Bottom-Left
    q3 = cv::Mat(dft2, cv::Rect(cx, cy, cx, cy));  // Bottom-Right

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    // Compute correlation in frequency domain
    cv::Mat corr;
    mulSpectrums(dft1, dft2, corr, 0, true);

    // Perform inverse DFT
    cv::Mat res;
    dft(corr, res, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

    // Normalize the result
    normalize(res, res, 0, 1, cv::NORM_MINMAX);

    // Find the peak location
    cv::Point maxLoc;
    minMaxLoc(res, NULL, NULL, NULL, &maxLoc);

    int x = maxLoc.y;
    int y = maxLoc.x;

    if (x > m / 2) x = x - m;
    if (y > n / 2) y = y - n;
    old_gray = new_gray.clone();
    vector<double> result;
    result.push_back(x);
    result.push_back(y);
    return result;
}
