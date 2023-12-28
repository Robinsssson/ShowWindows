#ifndef AXESFRESHTASK_H
#define AXESFRESHTASK_H
#include <QGraphicsView>
#include <QObject>
#include <QLineSeries>
#include <QChart>
#include <QGraphicsScene>

class AxesFreshTask : public QObject
{
    Q_OBJECT
public:
    explicit AxesFreshTask(QObject *parent = nullptr);
    void setGraphicsView(QGraphicsView * qGraphicsView) {this->qGraphicsView = qGraphicsView; }
private:
    QGraphicsView* qGraphicsView;
    QLineSeries* qLineSeries;
    QChart* qChart;
    QGraphicsScene* qScene;
};

#endif // AXESFRESHTASK_H
