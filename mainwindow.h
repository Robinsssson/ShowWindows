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

    void Ui_Init(Ui::MainWindow *_ui);

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
    bool m_captureOpenFlag = false;
    bool m_chart_windows_status = false;
    QString m_videoFileName;
    bool m_videoFileFlag = false;
    int m_fpsConfig = 1;
    int m_captureNumber;
    CaptureTask *m_captureTask;
    int m_selectedCapture = 0;
    CaptureShowTask *m_captureShowTask;
    QThread *threadVideoShowTask;
    QThread *threadVideoTask;
};
#endif // MAINWINDOW_H
