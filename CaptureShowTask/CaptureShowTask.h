//
// Created by Robinson on 2023/11/23.
//

#ifndef OPENCVPROJECT_CAPTURESHOWTASK_H
#define OPENCVPROJECT_CAPTURESHOWTASK_H

#include <QtCore/QThread>
#include <QTimer>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include "../AxesFreshTask/axesfreshtask.h"

class CaptureShowTask : public QObject {
Q_OBJECT
public:
    CaptureShowTask(QChartView * qGraphicsView, QLabel *label);
    ~CaptureShowTask() override;
    void CaptureShow();

    AxesFreshTask* axesFreshTask;

public slots:

    void getCaptureStatus(bool);

    void GetFpsNumber(int fps) { m_fps = fps; };

signals:
    void SendMat(cv::Mat);

private:
    bool m_videoShowFlag{};
    int m_fps{};
    QTimer *m_timer;
    cv::Mat *m_mat;
    QLabel *m_label;
    QThread* threadAxesFreshTask;
};


#endif //OPENCVPROJECT_CAPTURESHOWTASK_H
