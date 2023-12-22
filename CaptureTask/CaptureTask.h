//
// Created by Robinson on 2023/11/21.
//

#ifndef OPENCVPROJECT_CAPTURETASK_H
#define OPENCVPROJECT_CAPTURETASK_H

#include <QObject>
#include <QRunnable>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QThread>
#include <QEventLoop>

class CaptureTask : public QThread {
Q_OBJECT

public:
    CaptureTask();

    ~CaptureTask() override;

protected:
    void run() override;

public slots:

    void SetVideo(const QString &string);

    void getCaptureStatus(bool boolean);

    void PushMat(void);

    void getCaptureNumber(int number);

signals:

    void SendMat(cv::Mat);

private:
    int m_captureSelect = 0;
    bool m_switchCapture = false;
    QTimer *m_timer;
    cv::VideoCapture *m_capture;
    cv::Mat *m_mat;
    std::string *m_videoName;

};


#endif //OPENCVPROJECT_CAPTURETASK_H
