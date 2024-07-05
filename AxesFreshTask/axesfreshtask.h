#ifndef AXESFRESHTASK_H
#define AXESFRESHTASK_H
#include <QChart>
#include <QChartView>
#include <QFile>
#include <QFunctionPointer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QJsonObject>
#include <QLineSeries>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QTime>
#include <QValueAxis>
#include <opencv2/opencv.hpp>
class MainWindow;
// extern MainWindow* mainWin;

class AxesFreshTask : public QObject {
    Q_OBJECT
public:
    AxesFreshTask(QChartView *qChartView, QJsonObject json, QObject *parent = nullptr);
    AxesFreshTask(QObject *parent = nullptr) : QObject{parent} {}
    ~AxesFreshTask(void);
    void LazyInit(QChartView *qChartView, QJsonObject json);
    void setqChartView(QChartView *qChartView) { this->qChartView = qChartView; }
    void ansysAttribute(AxesFreshTask *, QJsonObject);
    bool getOnTime() const { return onTime; }
public slots:
    void axesFreshByFile();
    void changeAxesWithAlgSlot(int);
    void axesFreshByDoubleAndTime(double arg, QTime time);
    void is_open_video(bool);

private:
    QChartView *qChartView = nullptr;
    QLineSeries *qLineSeries = nullptr;
    QChart *qChart = nullptr;
    QAbstractAxis *xBottomAxis = nullptr;
    QValueAxis *yLeftAxis = nullptr;
    QList<QPointF> *qList = nullptr;
    QFile *csv_file = nullptr;
    int times = 0;
    bool onTime;
    bool first_entry = true;
    MainWindow *mainWin = nullptr;
};

#endif  // AXESFRESHTASK_H
