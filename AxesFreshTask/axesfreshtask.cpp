#include "axesfreshtask.h"

#include <QApplication>
#include <QChart>
#include <QDateTimeAxis>
#include <QJsonArray>
#include <QThread>
#include <QTime>
#include <QValueAxis>
#include <QtAlgorithms>
#include "../ImageProcess/ImageProcess.h"
#include "../Project_Config.h"
#include "../mainwindow.h"
#include "../Utility/fft/fft_from_csv.h"

/******************************************************************
 *  2023/12/28
 *  brief：添加了本类用作更新chart对象
 *  --bug wait fixed: chart无法做到即时更新，从而实现队列的跟进效果。
 *  bug fixed: using replot and clear to do that
 *  wait add feature: 添加方法让横坐标轴改为QTime
 ******************************************************************/
AxesFreshTask::AxesFreshTask(QChartView *qChartView, QJsonObject json, QObject *parent) : QObject{parent} {
    csv_file = nullptr;
    LazyInit(qChartView, json);
}

void AxesFreshTask::LazyInit(QChartView *qChartView, QJsonObject json) {
    qDebug() << "Enter LazyInit";
    qDebug() << "AxesFreshTask Constructored ID:" << QThread::currentThreadId();
    qChart = new QChart;
    qLineSeries = new QLineSeries;
    yLeftAxis = new QValueAxis;
    qList = new QList<QPointF>(0);
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (MainWindow *mainWindow = qobject_cast<MainWindow *>(widget)) {
            mainWin = mainWindow;
            break;
        }
    }
    qDebug() << "MainWindows" << mainWin;
    auto alg_name = ImageProcess::GetInstance().getName();
    qDebug() << "name : " << alg_name;
    setqChartView(qChartView);
    ansysAttribute(this, json);

    qChart->addSeries(qLineSeries);
    qChart->addAxis(yLeftAxis, Qt::AlignLeft);
    qChart->legend()->hide();

    if (onTime) {
        xBottomAxis = new QDateTimeAxis;
        qChart->addAxis(qobject_cast<QDateTimeAxis *>(xBottomAxis), Qt::AlignBottom);
        qobject_cast<QDateTimeAxis *>(xBottomAxis)->setFormat("mm:ss.zzz");
        connect(mainWin->get_captureShowTask(), &CaptureShowTask::EmitDoubleArgAndTime, this, &AxesFreshTask::axesFreshByDoubleAndTime, Qt::QueuedConnection);
        connect(mainWin, &MainWindow::is_open, this, &AxesFreshTask::is_open_video,Qt::QueuedConnection);
    } else {
        xBottomAxis = new QValueAxis;
        qChart->addAxis(qobject_cast<QValueAxis *>(xBottomAxis), Qt::AlignBottom);
        connect(mainWin, &MainWindow::ShowFFT, this, &AxesFreshTask::axesFreshByFile, Qt::QueuedConnection);
    }

    qLineSeries->attachAxis(xBottomAxis);
    qLineSeries->attachAxis(yLeftAxis);
    qChartView->setChart(qChart);
    qChartView->setRenderHint(QPainter::Antialiasing);
    qChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    qChartView->setFixedSize(AXES_W, AXES_H);
}
#define tmp_process(str) (str.toString())
void AxesFreshTask::ansysAttribute(AxesFreshTask *obj, QJsonObject json) {
    QMap<QString, std::function<void(QString)>> map;
    map.insert("x_label", [&](QString attr) {
        auto tmp = json[attr];
        // this->xBottomAxis->setTitleText(tmp_process(tmp));
    });
    map.insert("y_label", [&](QString attr) {
        auto tmp = json[attr];
        this->yLeftAxis->setTitleText(tmp_process(tmp));
    });
    map.insert("y_lim", [&](QString attr) {
        auto tmp = json[attr];
        auto arr = tmp.toArray();
        this->yLeftAxis->setRange(arr[0].toDouble(), arr[1].toDouble());
    });
    map.insert("title", [&](QString attr) {
        auto tmp = json[attr];
        this->qChart->setTitle(tmp_process(tmp));
    });
    map.insert("on-time", [&](QString attr) { onTime = json[attr].toBool(); });
    map.insert("file", [&](QString attr) {});
    map.insert("interface", [&](QString attr) {});
    for (const auto &attr : json.keys()) map[attr](attr);
}

void AxesFreshTask::axesFreshByFile() {
    qList->clear();
    auto alg_name = ImageProcess::GetInstance().getName();
    auto out = fft_from_csv(alg_name + ".csv");
    // auto vec = fft_from_python(alg_name + ".csv");
    out.pop_front();
    QVector<double> vec(out.length()/2);
    for (int i = 0; i < out.length()/2; i ++)
        vec[i] = out[i];
    for (int i = 0; i < vec.length(); i ++)
        qList->append(QPointF(i, vec[i]));
    qobject_cast<QValueAxis *>(xBottomAxis)->setRange(0, vec.length());
    double max = *std::max_element(vec.constBegin(), vec.constEnd());
    qobject_cast<QValueAxis *>(yLeftAxis)->setRange(0, max);
    qLineSeries->clear();
    qLineSeries->append(*qList);
}
void AxesFreshTask::is_open_video(bool is_open) {
    auto alg_name = ImageProcess::GetInstance().getName();
    if (is_open) {
        csv_file = new QFile(alg_name + ".csv");
        if (csv_file->open(QIODevice::WriteOnly)) {
            QString str;
            QTextStream(&str) << "times,arg" << Qt::endl;
            csv_file->write(str.toUtf8());
        }
        csv_file->close();
        first_entry = true;
    }else {
        csv_file->copy(alg_name + QTime::currentTime().toString() + ".csv");
        delete csv_file;
        csv_file = nullptr;
    }
}
void AxesFreshTask::axesFreshByDoubleAndTime(double arg, QTime time) {
    qDebug() << "AxesFreshTask:" << QThread::currentThreadId();
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(time.msecsSinceStartOfDay());
    qList->append(QPointF(dateTime.toMSecsSinceEpoch(), arg));
    auto s = qList->size();
    if (s > 200)
    {
        qList->pop_front();
        QDateTime firstTime = QDateTime::fromMSecsSinceEpoch(qList->first().x());
        QDateTime endTime = firstTime.addSecs(10);
        qobject_cast<QDateTimeAxis *>(xBottomAxis)->setRange(firstTime, endTime);
    }
    if (first_entry)
    {
        first_entry = false;
        QDateTime firstTime = QDateTime::fromMSecsSinceEpoch(qList->first().x());
        QDateTime endTime = firstTime.addSecs(10);
        qobject_cast<QDateTimeAxis *>(xBottomAxis)->setRange(firstTime, endTime);
        qList->pop_front();
        return;
    }
    qLineSeries->clear();
    qLineSeries->append(*qList);
    auto max_val = std::max_element(qList->constBegin(), qList->constEnd(), [&](QPointF t1, QPointF t2)->bool{
        return t1.y() < t2.y();
    })->y();
    auto min_val = std::min_element(qList->constBegin(), qList->constEnd(), [&](QPointF t1, QPointF t2)->bool{
        return t1.y() < t2.y();
    })->y();
    assert(max_val >= min_val);
    qobject_cast<QValueAxis *>(yLeftAxis)->setRange(min_val-5, max_val+5);
    if (csv_file->open(QIODevice::Append)) {
        QString str;
        QTextStream(&str) << time.toString("HH:mm:ss.zzz") << "," << arg << Qt::endl;
        csv_file->write(str.toUtf8());
    }
    csv_file->close();
}

AxesFreshTask::~AxesFreshTask(void) {
    if (csv_file == nullptr){
        return;
    }
    csv_file->close();
    delete csv_file;
}

void AxesFreshTask::changeAxesWithAlgSlot(int index) {
    auto alg_name = ImageProcess::GetInstance().ImageProcessList[index];
    qChart->setTitle(tr(alg_name.toUtf8()));
    qList->clear();
    qLineSeries->clear();
    if (onTime) {
        qobject_cast<QDateTimeAxis *>(xBottomAxis)->setRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addSecs(100));
    } else {
        qobject_cast<QValueAxis *>(xBottomAxis)->setRange(0, 100);
    }
    times = 0;
    csv_file->close();
}
