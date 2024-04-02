#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QtCharts/QLineSeries>
#include <QtCharts/QtCharts>

#include "settingconfig.h"
#include "AxesFreshTask/axesfreshtask.h"
#include "CaptureShowTask/CaptureShowTask.h"
#include "CaptureTask/CaptureTask.h"

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

   private:
    static std::vector<int> RefreshCameraNum();

    void RefreshCaptureSelect();

    void Ui_Init(Ui::MainWindow *_ui);

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

    QThread *threadAxesFreshTask;
    QThread *threadFftFreshTask;
    QThread *threadVideoShowTask;
    QThread *threadVideoTask;
    AxesFreshTask *axesFreshTask;
    CaptureShowTask *m_captureShowTask;
    CaptureTask *m_captureTask;
    SettingConfig *m_settingDialog;
};
#endif  // MAINWINDOW_H
