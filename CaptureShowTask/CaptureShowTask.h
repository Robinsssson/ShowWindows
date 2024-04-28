//
// Created by Robinson on 2023/11/23.
//

#ifndef OPENCVPROJECT_CAPTURESHOWTASK_H
#define OPENCVPROJECT_CAPTURESHOWTASK_H

#include <QLabel>
#include <QTimer>
#include <QtCore/QThread>
#include <QTime>
#include <opencv2/opencv.hpp>

class CaptureShowTask : public QObject {
    Q_OBJECT
   public:
    CaptureShowTask(QLabel *label);
    ~CaptureShowTask() override;
    void CaptureShow();
   public slots:
    void getCaptureStatus(bool);
    void GetFpsNumber(int fps) { m_fps = fps; };
    void algChanged(QString);
   signals:
    void EmitDoubleArg(double);
    void EmitDoubleArgAndTime(double, QTime);
    void SingletonMatError();
    void changeAxesWithAlg(int);

   private:
    bool m_videoShowFlag{};
    int m_fps{};
    QTimer *m_timer;
    QLabel *m_label;
};

#endif  // OPENCVPROJECT_CAPTURESHOWTASK_H
