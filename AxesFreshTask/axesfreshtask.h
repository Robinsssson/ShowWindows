#ifndef AXESFRESHTASK_H
#define AXESFRESHTASK_H
#include <QGraphicsView>
#include <QObject>
#include <QLineSeries>
#include <QChart>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <QFunctionPointer>
#include <QChartView>
#include <QValueAxis>
#include <QList>
#include <QPointF>

using valueCalFunction = double(*)(const cv::Mat&);
class AxesFreshTask : public QObject
{
    Q_OBJECT
public:
    explicit AxesFreshTask(QChartView * qGraphicsView, QObject *parent = nullptr);
    ~AxesFreshTask(void);
    void setqChartView(QChartView * qChartView) { this->qChartView = qChartView; }
    void setCalFunction(valueCalFunction function) { m_function = function; }

public slots:
    void axesFresh(cv::Mat);

private:
    QChartView* qChartView;
    QLineSeries* qLineSeries;
    QChart* qChart;
    valueCalFunction m_function;
    QValueAxis* xBottomAxis;
    QValueAxis* yLeftAxis;
    QList<QPointF> *qList;
};

#endif // AXESFRESHTASK_H
