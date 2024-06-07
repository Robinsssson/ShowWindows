#ifndef FFTFRESHTASK_H
#define FFTFRESHTASK_H

#include <QChartView>
#include <QObject>

#include "../Utility/csv/csv.h"

class FftFreshTask : public QObject {
    Q_OBJECT
  public:
    FftFreshTask(QChartView *qGraphicsView);

  private:
    QChartView *m_qChartView;
    io::CSVReader<2> *reader;
};

#endif  // FFTFRESHTASK_H
