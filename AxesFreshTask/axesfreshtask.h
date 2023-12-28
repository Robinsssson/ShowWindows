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

using valueCalFunction = double(*)(const cv::Mat&);
class AxesFreshTask : public QObject
{
    Q_OBJECT
public:
    explicit AxesFreshTask(QChartView * qGraphicsView, QObject *parent = nullptr);
    ~AxesFreshTask(void);
    void setGraphicsView(QChartView * qGraphicsView) { this->qGraphicsView = qGraphicsView; }
    void setCalFunction(valueCalFunction function) { m_function = function; }

public slots:
    void axesFresh(cv::Mat);

private:
    QChartView* qGraphicsView;
    QLineSeries* qLineSeries;
    QChart* qChart;
    valueCalFunction m_function;
};

#endif // AXESFRESHTASK_H
