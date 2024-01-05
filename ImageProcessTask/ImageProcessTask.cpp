//
// Created by Robinson on 2023/11/25.
//
#include "ImageProcessTask.h"

#include <QThread>
#include <opencv2/opencv.hpp>

#include "../ImageProcess/ImageProcess.h"
#include "../MatQueue/SingletonMatQueue.h"

void ImageProcessTask::run() {
    qDebug() << "ImageProcessTask ID:" << QThread::currentThreadId();
    if (SingletonMatQueue::GetInstance()->checkNotProcessed() > 0) {
        Q_Mat q_mat = SingletonMatQueue::GetInstance()->dequeueNotProcessed();
        SingletonMatQueue::GetInstance()->enqueueProcessed(
            ImageProcess::GetInstance().MatTranslate(q_mat.mat), q_mat.time);
    }
}
