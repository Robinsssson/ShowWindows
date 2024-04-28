#include "fftfreshtask.h"

FftFreshTask::FftFreshTask(QChartView *qGraphicsView) {
    m_qChartView = qGraphicsView;
    reader = new io::CSVReader<2>("../Cache/data_cache.csv");
}
