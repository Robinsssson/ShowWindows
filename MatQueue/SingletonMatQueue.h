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

struct Q_Mat {
    Q_Mat(QTime time, const cv::Mat mat) {
        this->mat = mat;
        this->time = time;
    }

    void set_arg(double arg) { this->arg = arg; }
    QTime time;
    cv::Mat mat;
    double arg = 0;
};

struct Q_MatCompare {
    bool operator()(const Q_Mat &lhs, const Q_Mat &rhs) const {
        return lhs.time > rhs.time;  // 按时间排序，时间小的优先
    }
};

class SingletonMatQueue : public QObject {
    Q_OBJECT

public:
    SingletonMatQueue() {}

    ~SingletonMatQueue() override = default;

    SingletonMatQueue(const SingletonMatQueue &) = delete;

    SingletonMatQueue &operator=(const SingletonMatQueue &) = delete;

public:
    static SingletonMatQueue *GetInstance() {
        if (m_instance == NULL) {
            QMutexLocker mutexLocker(&m_mutex);
            if (m_instance == NULL) m_instance = new SingletonMatQueue();
        }
        return m_instance;
    }

    void enqueueNotProcessed(const cv::Mat &mat, const QTime &time) {
        QMutexLocker mutexLocker(&m_mutex);
        m_matQueueNotProcessed.enqueue(Q_Mat(time, mat));
    }

    void enqueueProcessed(const cv::Mat &mat, const QTime &time) {
        QMutexLocker mutexLocker(&m_mutex);
        m_matQueueProcessed.push(Q_Mat(time, mat));
    }

    void enqueueProcessedWithArg(const cv::Mat &mat, const QTime &time, double arg) {
        QMutexLocker mutexLocker(&m_mutex);
        auto q_mat = Q_Mat(time, mat);
        q_mat.set_arg(arg);
        m_matQueueProcessed.push(q_mat);
    }

    Q_Mat dequeueNotProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        if (m_matQueueNotProcessed.isEmpty()) return Q_Mat(QTime(), cv::Mat());
        return m_matQueueNotProcessed.dequeue();
    }

    Q_Mat dequeueProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        if (m_matQueueProcessed.empty()) return Q_Mat(QTime(), cv::Mat());
        Q_Mat top = m_matQueueProcessed.top();
        m_matQueueProcessed.pop();
        return top;
    }

    long long checkNotProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        return m_matQueueNotProcessed.size();
    }

    long long checkProcessed() {
        QMutexLocker mutexLocker(&m_mutex);
        return m_matQueueProcessed.size();
    }

    void ClearAllQueue() {
        QMutexLocker mutexLocker(&m_mutex);
        m_matQueueNotProcessed.clear();
        while (!m_matQueueProcessed.empty()) {
            m_matQueueProcessed.pop();
        }
    }

private:
    static QMutex m_mutex;
    static SingletonMatQueue *m_instance;
    QQueue<Q_Mat> m_matQueueNotProcessed;
    std::priority_queue<Q_Mat, std::vector<Q_Mat>, Q_MatCompare> m_matQueueProcessed;
};

#endif  // OPENCVPROJECT_SINGLETONMATQUEUE_H
