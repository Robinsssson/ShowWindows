#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QStringList>
#include <fftw3.h>
#include <iostream>
#include <cmath>
#include "fft_from_csv.h"
#include <QtSystemDetection>
#include <cstdlib>
#include <QProcess>
#include <QDir>

QVector<double> fft_from_csv(const QString &name) {
    QVector<double> lst;

    // 读取 CSV 文件
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Cannot open file!" << std::endl;
        return {};
    }

    QTextStream in(&file);
    QString line;
    int line_num = 0;

    while (!in.atEnd()) {
        line = in.readLine();
        line_num++;
        if (line_num == 1 ) {
            continue;
        }
           QStringList fields = line.split(',');
        if (fields.size() > 1) {
            lst.append(fields[1].toDouble());
        }
    }
    file.close();

    int length = lst.size();
    // 将 lst 转换为 fftw 格式
    double* in_data = (double*) fftw_malloc(sizeof(double) * length);
    fftw_complex* out_data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * length);

    for (int i = 0; i < length; i++) {
        in_data[i] = lst[i];
    }

    fftw_plan plan = fftw_plan_dft_r2c_1d(length, in_data, out_data, FFTW_ESTIMATE);
    fftw_execute(plan);

    QVector<double> magnitude(length);
    for (int i = 0; i < length; i++) {
        magnitude[i] = sqrt(out_data[i][0] * out_data[i][0] + out_data[i][1] * out_data[i][1]);
    }

    fftw_destroy_plan(plan);
    fftw_free(in_data);
    fftw_free(out_data);

    return magnitude;
}

QVector<double> fft_from_python(const QString &name) {
    QFile file("contrast.py");
    if(!file.exists())
        exit(100);
    QString command = "C:\\Users\\Robinson\\AppData\\Local\\Programs\\Python\\Python312\\python.exe "+file.fileName()+" --file=" + name;
    QProcess process;
    process.start(command);
    process.waitForFinished();

    QString output(process.readAllStandardOutput());
    QStringList outputLines = output.split('\n', Qt::SkipEmptyParts);

    QVector<double> result;
    for (const QString &line : outputLines) {
        result.append(line.toDouble());
    }
    return result;
}
