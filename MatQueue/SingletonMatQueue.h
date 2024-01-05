//
// Created by Robinson on 2023/11/22.
//

#ifndef OPENCVPROJECT_SINGLETONMATQUEUE_H
#define OPENCVPROJECT_SINGLETONMATQUEUE_H

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QQueue>
#include <QSharedMemory>
#include <QSharedPointer>
#include <QTime>
#include <opencv2/opencv.hpp>
#include <queue>
#include <utility>

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
        m_matQueueNotProcessed =
            QSharedPointer<QQueue<Q_Mat>>(new QQueue<Q_Mat>);
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
                m_instance =
                    QSharedPointer<SingletonMatQueue>(new SingletonMatQueue);
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

    cv::Mat dequeueProcessed() { return m_matQueueProcessed->dequeue().mat; }

    long long checkNotProcessed() { return m_matQueueNotProcessed->size(); }

    long long checkProcessed() { return m_matQueueProcessed->size(); }

   private:
    static QMutex m_mutex;
    static QSharedPointer<SingletonMatQueue> m_instance;
    QSharedPointer<QQueue<Q_Mat>> m_matQueueNotProcessed;
    QSharedPointer<QQueue<Q_Mat>> m_matQueueProcessed;  // 改成优先队列
};

#endif  // OPENCVPROJECT_SINGLETONMATQUEUE_H
