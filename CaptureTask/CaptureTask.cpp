//
// Created by Robinson on 2023/11/21.
//
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <opencv2/opencv.hpp>

#include "../MatQueue/SingletonMatQueue.h"
#include "../CaptureTask/CaptureTask.h"
#include <QThreadPool>
#include "../ImageProcessTask/ImageProcessTask.h"

/*2023/11/23 Bug log
 *   打开视频然后退出软件之后，CaptureTask退出异常
 *   bug fixed... but haven't do anything..
 *
 *
 * */
CaptureTask::CaptureTask() {
    m_timer = new QTimer;
    m_capture = new cv::VideoCapture;
    m_mat = new cv::Mat;
    m_videoName = new std::string;
    m_captureSelect = 0;
    qDebug() << "CaptureTask constructor";
}

CaptureTask::~CaptureTask() {
    qDebug() << "CaptureTask delete";
    delete m_timer;
    delete m_capture;
    delete m_mat;
}

void CaptureTask::run() {
    m_timer->stop();
    connect(m_timer, &QTimer::timeout, this, &CaptureTask::PushMat, Qt::DirectConnection);
    exec();
}

void CaptureTask::PushMat() {
    if (!m_capture->read(*m_mat)) {
        getCaptureStatus(false);
    } else {
        SingletonMatQueue::GetInstance()->enqueueNotProcessed(*m_mat, QTime::currentTime());
        if (SingletonMatQueue::GetInstance()->checkNotProcessed() > 0) {
            if (SingletonMatQueue::GetInstance()->checkProcessed() > 30)
                return;
            QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
            if (SingletonMatQueue::GetInstance()->checkProcessed() < 5 &&
                SingletonMatQueue::GetInstance()->checkNotProcessed() > 5) {
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
            }
            if (SingletonMatQueue::GetInstance()->checkProcessed() < 2 &&
                SingletonMatQueue::GetInstance()->checkNotProcessed() > 10) {
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
                QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
            }
        }
    }
}

void CaptureTask::getCaptureNumber(int number) {
    m_captureSelect = number;
}

void CaptureTask::getCaptureStatus(bool boolean) {
    m_switchCapture = boolean;
    if (m_switchCapture) {
        m_timer->start(1);

        if (m_captureSelect == -1)
            m_capture->open(*m_videoName);
        else
            m_capture->open(m_captureSelect);
        if (!m_capture->isOpened()) {
            QMessageBox::warning(nullptr, tr("My Application"),
                                 tr("The document has been modified.\n"),
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                 QMessageBox::Save);
        }
    } else {
        m_timer->stop();
        m_capture->release();
    }
}

void CaptureTask::SetVideo(const QString &string) {
    *m_videoName = string.toStdString();
    m_captureSelect = -1;
    getCaptureStatus(false);
}
