//
// Created by Robinson on 2023/11/21.
//
#include "../CaptureTask/CaptureTask.h"

#include <QMessageBox>
#include <QThreadPool>
#include <QTime>
#include <QTimer>
#include <opencv2/opencv.hpp>

#include "../ImageProcess/ImageProcess.h"
#include "../MatQueue/SingletonMatQueue.h"

/*   2023/11/23 Bug log
 *   打开视频然后退出软件之后，CaptureTask退出异常
 *   bug fixed... but haven't do anything..
 * */
CaptureTask::CaptureTask() {
    m_timer = nullptr;
    m_capture = new cv::VideoCapture;
    m_mat = new cv::Mat;
    m_captureSelect = 0;
    qDebug() << "CaptureTask Constructor ID:" << QThread::currentThreadId();
}

CaptureTask::~CaptureTask() {
    qDebug() << "CaptureTask Destory ID:" << QThread::currentThreadId();
    if (m_timer != nullptr) delete m_timer;
    delete m_capture;
    delete m_mat;
}

// Bug in there.
void CaptureTask::PushMat() {
    qDebug() << "CaptureTask ID:" << QThread::currentThreadId();
    if (!m_capture->read(*m_mat)) {
        if (m_captureSelect == -1) emit VideoOver();
        getCaptureStatus(false);
    } else {
        SingletonMatQueue::GetInstance()->enqueueNotProcessed(
            *m_mat, QTime::currentTime());
        if (SingletonMatQueue::GetInstance()->checkNotProcessed() > 0) {
            // QThreadPool::globalInstance()->tryStart(new ImageProcessTask);
            Q_Mat q_mat =
                SingletonMatQueue::GetInstance()->dequeueNotProcessed();
            double num =
                ImageProcess::GetInstance().process_function(q_mat.mat);
            SingletonMatQueue::GetInstance()->enqueueProcessedWithArg(
                q_mat.mat, q_mat.time, num);
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
        m_timer->start(m_fps);
        if (m_captureSelect == -1) {
            m_capture->open(m_videoName);
            cv::Mat mat;
            m_capture->read(mat);
            qDebug() << mat.type();
        } else {
            m_capture->open(m_captureSelect);
            cv::Mat mat;
            m_capture->read(mat);
            qDebug() << mat.type();
        }
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
        QThreadPool::globalInstance()->releaseThread();
        SingletonMatQueue::GetInstance()->ClearAllQueue();
    }
}

void CaptureTask::SetVideo(const QString &string) {
    m_videoName = string.toStdString();
    qDebug() << m_videoName;
    m_captureSelect = -1;
    getCaptureStatus(false);
}
