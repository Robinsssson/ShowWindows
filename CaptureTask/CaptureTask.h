//
// Created by Robinson on 2023/11/21.
//

#ifndef OPENCVPROJECT_CAPTURETASK_H
#define OPENCVPROJECT_CAPTURETASK_H

#include <QEventLoop>
#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
class CaptureTask : public QObject {
    Q_OBJECT

  public:
    CaptureTask();

    ~CaptureTask() override;

  public slots:

    void SetVideo(const QString &string);

    void getCaptureStatus(bool boolean);

    void PushMat(void);

    void getCaptureNumber(int number);

    void GetFpsNumber(int fps) { m_fps = fps; };

    void taskGetRect(cv::Rect *rect) { m_rect = rect; }

  signals:

    void VideoOver();

    void videoFPS(int);

    void SendMat(cv::Mat);

  private:
    int m_fps = 1;
    int m_captureSelect = 0;
    bool m_switchCapture = false;
    QTimer *m_timer;
    cv::VideoCapture *m_capture;
    cv::Mat *m_mat;
    std::string m_videoName;
    cv::Rect *m_rect = nullptr;
};

#endif  // OPENCVPROJECT_CAPTURETASK_H
