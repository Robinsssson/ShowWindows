#include "axesfreshtask.h"
#include <QThread>
#include <QTime>

/***************************************************************
 *  2023/12/28
 *  brief：添加了本类用作更新chart对象
 *  --bug wait fixed: chart无法做到即时更新，从而实现队列的跟进效果。
 *  bug fixed: using replot and clear to do that
 *  wait add feature: 添加方法让横坐标轴改为QTime
 ***************************************************************/
AxesFreshTask::AxesFreshTask(QChartView *qChartView, QObject *parent)
    : QObject{parent} {
    qDebug() << "AxesFreshTask Constructored ID:" << QThread::currentThreadId();
    qChart = new QChart;
    qLineSeries = new QLineSeries;
    xBottomAxis = new QValueAxis;
    yLeftAxis = new QValueAxis;
    qList = new QList<QPointF>;
    xBottomAxis->setRange(0, 100);
    yLeftAxis->setRange(0, 1000);
    csv_file = new QFile("data_cache.csv");
    setqChartView(qChartView);
    qChart->addSeries(qLineSeries);
    qChart->addAxis(yLeftAxis, Qt::AlignLeft);
    qChart->addAxis(xBottomAxis, Qt::AlignBottom);
    qLineSeries->attachAxis(xBottomAxis);
    qLineSeries->attachAxis(yLeftAxis);
    qChart->legend()->hide();
    qChart->setTitle(tr("x0变化值"));
    qChartView->setChart(qChart);
    qChartView->setRenderHint(QPainter::Antialiasing);
    qChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if(csv_file->open(QIODevice::WriteOnly)){
        QString str;
        QTextStream(&str) << "times" << "," << "arg" << Qt::endl;
        csv_file->write(str.toUtf8());
    }
    csv_file->close();
}
// not use
// void AxesFreshTask::axesFreshByMat(cv::Mat mat) {
//     qDebug() << "axesFresh ID:" << QThread::currentThreadId();
//     static int times = 0;
//     if (qList->size() > 100) {
//         qList->pop_front();
//         int n = std::ceil(qList->first().x());
//         xBottomAxis->setRange(n, 100 + n);
//     }
//     qList->append(QPointF(times++, m_function(mat)));
//     qLineSeries->clear();
//     qLineSeries->append(*qList);
// }

void AxesFreshTask::axesFreshByDouble(double arg) {
    static int times = 0;
    if (qList->size() > 100) {
        qList->pop_front();
        int n = std::ceil(qList->first().x()); // ?
        xBottomAxis->setRange(n, 100 + n);
    }
    qList->append(QPointF(times++, arg));
    qLineSeries->clear();
    qLineSeries->append(*qList);
    if(csv_file->open(QIODevice::Append)){
        QString str;
        QTextStream(&str) << times << "," << arg << Qt::endl;
        csv_file->write(str.toUtf8());
    }
    csv_file->close();
}

AxesFreshTask::~AxesFreshTask(void) {
    qLineSeries->clear();
    delete xBottomAxis;
    delete yLeftAxis;
    delete qList;
    delete qChart;
    csv_file->close();
    delete csv_file;
}
