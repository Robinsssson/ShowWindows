//
// Created by Robinson on 2023/11/25.
//
#include "ImageProcessTask.h"
#include "../ImageProcess/ImageProcess.h"
#include <opencv2/opencv.hpp>
#include "../MatQueue/SingletonMatQueue.h"
#include <QThread>

void ImageProcessTask::run() {
    qDebug() <<"ImageProcessTask ID:" << QThread::currentThreadId();
    if (SingletonMatQueue::GetInstance()->checkNotProcessed() > 0) {
        Q_Mat q_mat = SingletonMatQueue::GetInstance()->dequeueNotProcessed();
        SingletonMatQueue::GetInstance()->enqueueProcessed(ImageProcess::MatTranslate(q_mat.mat), q_mat.time);
    }
}
