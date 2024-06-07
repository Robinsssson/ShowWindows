#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QtCharts/QLineSeries>
#include <QtCharts/QtCharts>

#include "CaptureShowTask/CaptureShowTask.h"
#include "CaptureTask/CaptureTask.h"
#include "Utility/AnsysSetting/ansyssetting.h"
#include "settingconfig.h"
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void set_video_show(QSize size, bool is_signal = false);

  private:
    static std::vector<int> RefreshCameraNum(QSize *size);

    void RefreshCaptureSelect();

    void Ui_Init(QSize size);

  public slots:

    void on_pushButtonOpen_clicked();

    void on_pushButtonClose_clicked();

    void on_actionImportVideos_triggered();

    void on_actionFPS_triggered();

    void on_actionSetting_triggered();
  signals:

    void thisCapture(int number);

    void switchCapture(bool);

    void SendVideoFileName(QString);

    void SendFpsNumber(int);

    void alg_selected(QString);

    void send_rect(cv::Rect *);

  public:
    Ui::MainWindow *ui;
  private:
    bool m_captureOpenFlag = false;
    bool m_chart_windows_status = false;
    QString m_videoFileName;
    bool m_videoFileFlag = false;
    int m_fpsConfig = 1;
    std::vector<int> m_captureNumber;
    int m_selectedCapture = 0;
    AnsysSetting *ansys_setting;
    QThread *threadVideoShowTask;
    QThread *threadVideoTask;
    CaptureShowTask *m_captureShowTask;
    CaptureTask *m_captureTask;
    SettingConfig *m_settingDialog;
    QSize m_init_size;
    cv::Rect *m_wait_send_rect = nullptr;
};
#endif  // MAINWINDOW_H
