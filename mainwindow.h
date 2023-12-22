#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCharts/QtCharts>
#include <QtCharts/QLineSeries>
#include <QGraphicsScene>
#include <QVector>
#include "CaptureTask/CaptureTask.h"
#include "CaptureShowTask/CaptureShowTask.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    static int RefreshCameraNum();

    void RefreshCaptureSelect();


public slots:

    void on_pushButtonOpen_clicked();

    void on_pushButtonClose_clicked();

    void on_pushButtonCreateChart_clicked();

    void on_actionImportVideos_triggered();

    void on_actionFPS_triggered();

signals:

    void thisCapture(int number);

    void switchCapture(bool);

    void SendVideoFileName(QString);

    void SendFpsNumber(int);

public:
    Ui::MainWindow *ui;
private:
    bool m_captureOpenFlag;
    bool m_chart_windows_status;
    QGraphicsScene *m_scene;
    QLineSeries *m_lineSeries;
    QChart *m_chart;
    QString m_videoFileName;
    bool m_videoFileFlag;
    int m_fpsConfig;
    int m_captureNumber;
    CaptureTask *m_captureTask;
    int m_selectedCapture;
    CaptureShowTask *m_captureShowTask;
    QThread *threadVideoShowTask;
    QThread *threadVideoTask;
};
#endif // MAINWINDOW_H
