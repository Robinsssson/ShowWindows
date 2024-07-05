#ifndef FFT_FROM_CSV_H
#define FFT_FROM_CSV_H
#include <QVector>
#include <QString>
QVector<double> fft_from_csv(const QString &name);
QVector<double> fft_from_python(const QString &name);
#endif  // FFT_FROM_CSV_H
