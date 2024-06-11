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
    void axesFreshByDouble(double);
    void changeAxesWithAlgSlot(int);
    void axesFreshByDoubleAndTime(double arg, QTime time);

private:
    QChartView *qChartView;
    QLineSeries *qLineSeries;
    QChart *qChart;
    QValueAxis *xBottomAxis;
    QValueAxis *yLeftAxis;
    QList<QPointF> *qList;
    QFile *csv_file;
    int times = 0;
    bool onTime;
    MainWindow *mainWin = nullptr;
};

#endif  // AXESFRESHTASK_H
