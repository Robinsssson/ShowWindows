#include "axesfreshtask.h"

#include <QApplication>
#include <QChart>
#include <QJsonArray>
#include <QThread>
#include <QTime>

#include "../ImageProcess/ImageProcess.h"
#include "../Project_Config.h"
#include "../mainwindow.h"
/******************************************************************
 *  2023/12/28
 *  brief：添加了本类用作更新chart对象
 *  --bug wait fixed: chart无法做到即时更新，从而实现队列的跟进效果。
 *  bug fixed: using replot and clear to do that
 *  wait add feature: 添加方法让横坐标轴改为QTime
 ******************************************************************/
AxesFreshTask::AxesFreshTask(QChartView *qChartView, QJsonObject json, QObject *parent) : QObject{parent} {
    LazyInit(qChartView, std::move(json));
}
void AxesFreshTask::LazyInit(QChartView *qChartView, QJsonObject json) {
    qDebug() << "Enter LazyInit";
    qDebug() << "AxesFreshTask Constructored ID:" << QThread::currentThreadId();
    qChart = new QChart;
    qLineSeries = new QLineSeries;
    xBottomAxis = new QValueAxis;
    yLeftAxis = new QValueAxis;
    qList = new QList<QPointF>;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (MainWindow *mainWindow = qobject_cast<MainWindow *>(widget)) {
            mainWin = mainWindow;
            break;
        }
    }
    qDebug() << "MainWindows" << mainWin;
    auto alg_name = json["file"].toString();
    qDebug() << "name : " << alg_name;
    csv_file = new QFile(alg_name + ".csv");
    setqChartView(qChartView);
    ansysAttribute(this, json);

    qChart->addSeries(qLineSeries);
    qChart->addAxis(yLeftAxis, Qt::AlignLeft);
    qChart->addAxis(xBottomAxis, Qt::AlignBottom);
    qChart->legend()->hide();

    qLineSeries->attachAxis(xBottomAxis);
    qLineSeries->attachAxis(yLeftAxis);

    qChartView->setChart(qChart);
    qChartView->setRenderHint(QPainter::Antialiasing);
    qChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    qChartView->setFixedSize(AXES_W, AXES_H);

    if (csv_file->open(QIODevice::WriteOnly)) {
        QString str;
        QTextStream(&str) << "times,arg" << Qt::endl;
        csv_file->write(str.toUtf8());
    }
    csv_file->close();
}

void AxesFreshTask::ansysAttribute(AxesFreshTask *obj, QJsonObject json) {
    QMap<QString, std::function<void(QString)>> map;
    map.insert("x_label", [&](QString attr) {
        auto tmp = json[attr];
        this->xBottomAxis->setTitleText(tmp.toString());
    });
    map.insert("y_label", [&](QString attr) {
        auto tmp = json[attr];
        this->yLeftAxis->setTitleText(tmp.toString());
    });
    map.insert("y_lim", [&](QString attr) {
        auto tmp = json[attr];
        auto arr = tmp.toArray();
        this->yLeftAxis->setRange(arr[0].toDouble(), arr[1].toDouble());
    });
    map.insert("title", [&](QString attr) {
        auto tmp = json[attr];
        this->qChart->setTitle(tmp.toString());
    });
    map.insert("on-time", [&](QString attr) {
        onTime = json[attr].toBool();
        if (!onTime) return;
        if (mainWin == nullptr) throw std::runtime_error("error occupid in there " + std::to_string(__LINE__) + " " + __FILE__);
        connect(mainWin->get_captureShowTask(), &CaptureShowTask::EmitDoubleArgAndTime, this, &AxesFreshTask::axesFreshByDoubleAndTime, Qt::QueuedConnection);
    });
    map.insert("file", [&](QString attr) {});
    map.insert("interface", [&](QString attr) {});
    for (const auto &attr : json.keys()) map[attr](attr);
}

void AxesFreshTask::axesFreshByDouble(double arg) {
    if (qList->size() > 100) {
        qList->pop_front();
        int n = std::ceil(qList->first().x());  // ?
        xBottomAxis->setRange(n, 100 + n);
    }
    qList->append(QPointF(times++, arg));
    qLineSeries->clear();
    qLineSeries->append(*qList);
    if (csv_file->open(QIODevice::Append)) {
        QString str;
        QTextStream(&str) << times << "," << arg << Qt::endl;
        csv_file->write(str.toUtf8());
    }
    csv_file->close();
}

void AxesFreshTask::axesFreshByDoubleAndTime(double arg, QTime time) {
    qDebug() << "AxesFreshTask:"<< QThread::currentThreadId();
    if (qList->size() > 100) {
        qList->pop_front();
        int n = std::ceil(qList->first().x());  // ?
        xBottomAxis->setRange(n, 100 + n);
    }
    qList->append(QPointF(times++, arg));
    qLineSeries->clear();
    qLineSeries->append(*qList);
    if (csv_file->open(QIODevice::Append)) {
        QString str;
        QTextStream(&str) << times << "," << arg << Qt::endl;
        csv_file->write(str.toUtf8());
    }
    csv_file->close();
}

AxesFreshTask::~AxesFreshTask(void) {
    csv_file->close();
    delete csv_file;
}

void AxesFreshTask::changeAxesWithAlgSlot(int index) {
    auto alg_name = ImageProcess::GetInstance().ImageProcessList[index];
    qChart->setTitle(tr(alg_name.toUtf8()));
    qList->clear();
    qLineSeries->clear();
    xBottomAxis->setRange(0, 100);
    delete csv_file;
    csv_file = new QFile(alg_name + ".csv");
    if (csv_file->open(QIODevice::WriteOnly)) {
        QString str;
        QTextStream(&str) << "times,arg" << Qt::endl;
        csv_file->write(str.toUtf8());
    }
    times = 0;
    csv_file->close();
}
