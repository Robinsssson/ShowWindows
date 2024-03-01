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

struct Q_Mat {
    Q_Mat(QTime time, const cv::Mat &mat) {
        this->mat = mat;
        this->time = time;
    }

    void set_arg(double arg) { this->arg = arg; }
    QTime time;
    cv::Mat mat;
    double arg = 0;
};

class SingletonMatQueue : public QObject {
    Q_OBJECT

   public:
    SingletonMatQueue() {
    }

    ~SingletonMatQueue() override = default;

    SingletonMatQueue(const SingletonMatQueue &) = delete;

    SingletonMatQueue &operator=(const SingletonMatQueue &) = delete;

   public:
    static SingletonMatQueue *GetInstance() {
        if (m_instance == NULL) {
            QMutexLocker mutexLocker(&m_mutex);
            if (m_instance == NULL)
                m_instance = new SingletonMatQueue();
        }
        return m_instance;
    }

    void enqueueNotProcessed(const cv::Mat &mat, const QTime &time) {
        QMutexLocker mutexLocker(&m_mutex);
        m_matQueueNotProcessed.enqueue(Q_Mat(time, mat));
    }

    void enqueueProcessed(const cv::Mat &mat, const QTime &time) {
        QMutexLocker mutexLocker(&m_mutex);
        m_matQueueProcessed.enqueue(Q_Mat(time, mat));
    }

    void enqueueProcessedWithArg(const cv::Mat &mat, const QTime &time,
                                 double arg) {
        QMutexLocker mutexLocker(&m_mutex);
        auto q_mat = Q_Mat(time, mat);
        q_mat.set_arg(arg);
        m_matQueueProcessed.enqueue(q_mat);
    }

    Q_Mat dequeueNotProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        if (m_matQueueNotProcessed.isEmpty())
            return m_matQueueNotProcessed.head();
        return m_matQueueNotProcessed.dequeue();
    }

    Q_Mat dequeueProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        if (m_matQueueProcessed.isEmpty())
            return m_matQueueProcessed.head();
        return m_matQueueProcessed.dequeue();
    }

    long long checkNotProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        return m_matQueueNotProcessed.size(); }

    long long checkProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        return m_matQueueProcessed.size();
    }

    void ClearAllQueue() {
        QMutexLocker mutexLocker(&m_mutex);
        m_matQueueNotProcessed.clear();
        m_matQueueProcessed.clear();
    }

   private:
    static QMutex m_mutex;
    static SingletonMatQueue* m_instance;
    QQueue<Q_Mat> m_matQueueNotProcessed;
    QQueue<Q_Mat> m_matQueueProcessed;  // 改成优先队列
};

#endif  // OPENCVPROJECT_SINGLETONMATQUEUE_H
