#ifndef AXESFRESHTASK_H
#define AXESFRESHTASK_H
#include <QChart>
#include <QChartView>
#include <QFile>
#include <QFunctionPointer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineSeries>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QValueAxis>
#include <opencv2/opencv.hpp>
#include <QTime>

class AxesFreshTask : public QObject {
    Q_OBJECT
   public:
    explicit AxesFreshTask(QChartView *qGraphicsView,
                           QObject *parent = nullptr);
    ~AxesFreshTask(void);

    void setqChartView(QChartView *qChartView) {
        this->qChartView = qChartView;
    }

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
};

#endif  // AXESFRESHTASK_H
