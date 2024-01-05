//
// Created by Robinson on 2023/11/23.
//

#include "CaptureShowTask.h"

#include <QImage>
#include <QPixmap>
#include <QThread>

#include "../ImageProcess/ImageProcess.h"
#include "../MatQueue/SingletonMatQueue.h"

/*
 * 2023/11/23 bug wait fix log
 *  视频摄像头开启过早，会导致黑屏无法显示，急需解决。
 *  bug fixed log: in label 1 fix this bug? waiting test.
 *
 * 2023/12/21 bug log
 *  when drag the page pic geting error
 *
 * 2024/1/2 bug log
 *  视频开启时，会导致绘图页面跟不上
 *  bug how to fix
 *  视频模式可以把视频单独列出来
 * */
CaptureShowTask::CaptureShowTask(QLabel *label) {
    m_timer = nullptr;
    m_mat = new cv::Mat;
    m_label = label;
    m_fps = 1;  // label 1
    qDebug() << "CaptureShowTask Constructed ID:" << QThread::currentThreadId();
}

CaptureShowTask::~CaptureShowTask() {
    if (m_timer != nullptr) delete m_timer;
    delete m_mat;
    qDebug() << "CaptureShowTask Destroyed ID:" << QThread::currentThreadId();
}

void CaptureShowTask::getCaptureStatus(bool boolean) {
    m_videoShowFlag = boolean;
    if (m_timer == nullptr) {
        m_timer = new QTimer;
        connect(m_timer, &QTimer::timeout, this, &CaptureShowTask::CaptureShow,
                Qt::DirectConnection);
    }
    if (m_videoShowFlag) {
        m_timer->start(m_fps);
    } else {
        m_timer->stop();
    }
}

void CaptureShowTask::CaptureShow() {
    if (SingletonMatQueue::GetInstance()->checkProcessed() == 0) {
        return;
    }
    qDebug() << "CaptureShowTask ID:" << QThread::currentThreadId();
    *m_mat = SingletonMatQueue::GetInstance()->dequeueProcessed();
    emit SendMat(*m_mat);
    auto qImage = ImageProcess::GetInstance().cvMat2QImage(*m_mat);
    if (qImage.height() > m_label->maximumHeight() ||
        qImage.width() > m_label->maximumWidth())
        qImage = qImage.scaled(m_label->maximumSize());
    m_label->setFixedSize(qImage.width(), qImage.height());
    m_label->setPixmap(QPixmap::fromImage(qImage));
}
