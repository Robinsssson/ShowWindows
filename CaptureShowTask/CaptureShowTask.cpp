//
// Created by Robinson on 2023/11/23.
//

#include "CaptureShowTask.h"
#include "../MatQueue/SingletonMatQueue.h"
#include "../ImageProcess/ImageProcess.h"
#include <QImage>
#include <QPixmap>

#include <QThread>

/*
 * 2023/11/23 bug wait fix log
 *  视频摄像头开启过早，会导致黑屏无法显示，急需解决。
 *  bug fixed log: in label 1 fix this bug? waiting test.
 *
 * 2023/12/21 bug log
 *  when drag the page pic geting error
 *
 * */
CaptureShowTask::CaptureShowTask(QChartView * qGraphicsView, QLabel *label) {
    m_timer = nullptr;
    m_mat = new cv::Mat;
    m_label = label;
    m_fps = 1; //label 1
    qDebug() << "CaptureShowTask Constructed ID:" << QThread::currentThreadId();
    axesFreshTask = new AxesFreshTask(qGraphicsView);
    threadAxesFreshTask = new QThread;
}

CaptureShowTask::~CaptureShowTask() {
    threadAxesFreshTask->quit();
    threadAxesFreshTask->wait();
    threadAxesFreshTask->deleteLater();

    delete axesFreshTask;
    delete m_timer;
    delete m_mat;
    qDebug() << "CaptureShowTask Destroyed ID:" << QThread::currentThreadId();
}

void CaptureShowTask::getCaptureStatus(bool boolean) {
    m_videoShowFlag = boolean;
    if(m_timer == nullptr)
    {
        m_timer = new QTimer;
        axesFreshTask->setCalFunction(ImageProcess::calGrayPercent);
        connect(m_timer, &QTimer::timeout, this, &CaptureShowTask::CaptureShow, Qt::DirectConnection);
        connect(this, &CaptureShowTask::SendMat, axesFreshTask, &AxesFreshTask::axesFresh, Qt::AutoConnection);
        axesFreshTask->moveToThread(threadAxesFreshTask);
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
    qDebug() <<"CaptureShowTask ID:" << QThread::currentThreadId();
    *m_mat = SingletonMatQueue::GetInstance()->dequeueProcessed();
    emit SendMat(*m_mat);
    auto qImage = ImageProcess::cvMat2QImage(*m_mat);
    m_label->setFixedSize(qImage.size().width(), qImage.size().height());
    m_label->setPixmap(QPixmap::fromImage(qImage));
}
