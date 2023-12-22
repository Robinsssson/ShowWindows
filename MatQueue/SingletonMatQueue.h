//
// Created by Robinson on 2023/11/22.
//

#ifndef OPENCVPROJECT_SINGLETONMATQUEUE_H
#define OPENCVPROJECT_SINGLETONMATQUEUE_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedMemory>
#include <QSharedPointer>
#include <QQueue>
#include <QTime>
#include <opencv2/opencv.hpp>
#include <utility>
#include <queue>

struct Q_Mat {
    Q_Mat(QTime time, const cv::Mat &mat) {
        this->mat = mat;
        this->time = time;
    }

    QTime time;
    cv::Mat mat;
};

class SingletonMatQueue : public QObject {
Q_OBJECT

public:

    SingletonMatQueue() {
        m_matQueueNotProcessed = QSharedPointer<QQueue<Q_Mat>>(new QQueue<Q_Mat>);
        m_matQueueProcessed = QSharedPointer<QQueue<Q_Mat>>(new QQueue<Q_Mat>);
    }

    ~SingletonMatQueue() override = default;

    SingletonMatQueue(const SingletonMatQueue &) = delete;

    SingletonMatQueue &operator=(const SingletonMatQueue &) = delete;


public:
    static QSharedPointer<SingletonMatQueue> &GetInstance() {
        if (m_instance.isNull()) {
            QMutexLocker mutexLocker(&m_mutex);
            if (m_instance.isNull())
                m_instance = QSharedPointer<SingletonMatQueue>(new SingletonMatQueue);
        }
        return m_instance;
    }

    void enqueueNotProcessed(const cv::Mat &mat, const QTime &time) {
        m_matQueueNotProcessed->enqueue(Q_Mat(time, mat));
    }

    void enqueueProcessed(const cv::Mat &mat, const QTime &time) {
        m_matQueueProcessed->enqueue(Q_Mat(time, mat));
    }

    Q_Mat dequeueNotProcessed() {
        if (m_matQueueNotProcessed->isEmpty())
            return m_matQueueNotProcessed->head();
        return m_matQueueNotProcessed->dequeue();
    }

    cv::Mat dequeueProcessed() {
        return m_matQueueProcessed->dequeue().mat;
    }

    long long checkNotProcessed() {
        return m_matQueueNotProcessed->size();
    }

    long long checkProcessed() {
        return m_matQueueProcessed->size();
    }

private:
    static QMutex m_mutex;
    static QSharedPointer<SingletonMatQueue> m_instance;
    QSharedPointer<QQueue<Q_Mat>> m_matQueueNotProcessed;
    QSharedPointer<QQueue<Q_Mat>> m_matQueueProcessed; //改成优先队列
};


#endif //OPENCVPROJECT_SINGLETONMATQUEUE_H
