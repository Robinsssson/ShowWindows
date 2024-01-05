//
// Created by Robinson on 2023/11/21.
//
#include "../CaptureTask/CaptureTask.h"

#include <QMessageBox>
#include <QThreadPool>
#include <QTime>
#include <QTimer>
#include <opencv2/opencv.hpp>

#include "../ImageProcessTask/ImageProcessTask.h"
#include "../MatQueue/SingletonMatQueue.h"

/*2023/11/23 Bug log
 *   打开视频然后退出软件之后，CaptureTask退出异常
 *   bug fixed... but haven't do anything..
 *
 *
 * */
CaptureTask::CaptureTask() {
    m_timer = nullptr;
    m_capture = new cv::VideoCapture;
    m_mat = new cv::Mat;
    m_videoName = new std::string;
    m_captureSelect = 0;
    qDebug() << "CaptureTask Constructor ID:" << QThread::currentThreadId();
}

CaptureTask::~CaptureTask() {
    qDebug() << "CaptureTask Destory ID:" << QThread::currentThreadId();
    if (m_timer != nullptr) delete m_timer;
    delete m_capture;
    delete m_mat;
    delete m_videoName;
}

void CaptureTask::PushMat() {
    qDebug() << "CaptureTask ID:" << QThread::currentThreadId();
    if (!m_capture->read(*m_mat)) {
        getCaptureStatus(false);
    } else {
        SingletonMatQueue::GetInstance()->enqueueNotProcessed(
            *m_mat, QTime::currentTime());
        if (SingletonMatQueue::GetInstance()->checkNotProcessed() > 0) {
            if (SingletonMatQueue::GetInstance()->checkProcessed() > 30) return;
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

void CaptureTask::getCaptureNumber(int number) { m_captureSelect = number; }

void CaptureTask::getCaptureStatus(bool boolean) {
    m_switchCapture = boolean;
    if (m_timer == nullptr) {
        m_timer = new QTimer;
        connect(m_timer, &QTimer::timeout, this, &CaptureTask::PushMat,
                Qt::DirectConnection);
    }
    if (m_switchCapture) {
        m_timer->start(1);

        if (m_captureSelect == -1)
            m_capture->open(*m_videoName);
        else
            m_capture->open(m_captureSelect);
        if (!m_capture->isOpened()) {
            QMessageBox::warning(
                nullptr, tr("My Application"),
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
