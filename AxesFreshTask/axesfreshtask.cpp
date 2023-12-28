#include "axesfreshtask.h"


/* 2023/12/28
 *  brief：添加了本类用作更新chart对象
 *  bug wait fixed：chart无法做到即时更新，从而实现队列的跟进效果。
 *
 */
AxesFreshTask::AxesFreshTask(QChartView * qGraphicsView, QObject *parent)
    : QObject{parent}
{
    qChart = new QChart;
    qLineSeries = new QLineSeries;
    setGraphicsView(qGraphicsView);
    qLineSeries->setPointsVisible(true);
    qChart->addSeries(qLineSeries);
    qChart->createDefaultAxes();
    qChart->legend()->hide();
    qChart->setTitle(tr("灰度值"));
    qGraphicsView->setChart(qChart);
    qGraphicsView->setRenderHint(QPainter::Antialiasing);
}

void AxesFreshTask::axesFresh(cv::Mat mat)
{

    static int i;
    qLineSeries->append(i ++, m_function(mat));
    qDebug() << m_function(mat);
}

AxesFreshTask::~AxesFreshTask(void)
{
    delete qChart;
    delete qLineSeries;
}
