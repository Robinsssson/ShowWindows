//
// Created by Robinson on 2023/11/23.
//

#ifndef OPENCVPROJECT_CAPTURESHOWTASK_H
#define OPENCVPROJECT_CAPTURESHOWTASK_H

#include <QLabel>
#include <QTimer>
#include <QtCore/QThread>
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
   signals:
    void EmitDoubleArg(double);
    void SingletonMatError();
   private:
    bool m_videoShowFlag{};
    int m_fps{};
    QTimer *m_timer;
    QLabel *m_label;
};

#endif  // OPENCVPROJECT_CAPTURESHOWTASK_H
