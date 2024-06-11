#include <Qtime>
#include <map>

#include "fftw3.h"
#pragma comment(lib, "libfftw3-3.lib")
//
#define _CRT_SECURE_NO_WARNINGS
#include "../csv/csv.h"

using namespace std;

std::map<QTime, double> fftFromCSV(io::CSVReader<2>& in) {
    /**/
    std::map<QTime, double> deta;
    in.read_header(io::ignore_extra_column, "times", "arg");
    QTime time;
    double value;

    while (in.read_row(time, value)) {
        deta[time] = value;  // 输入数据
    }

    if (deta.empty()) {
        qDebug() << "数据为空或读取失败";
        return std::map<QTime, double>();  // 返回空map
    }

    int N = deta.size();

    double* inDeta = NULL;
    fftw_complex* outDeta = NULL;
    fftw_plan plan;

    inDeta = (double*)fftw_malloc(sizeof(double) * N);
    outDeta = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    if (inDeta == nullptr || outDeta == nullptr) {
        qDebug() << "内存分配失败 (inDeta/outDeta)";
        return std::map<QTime, double>();  // 返回空map
    }

    int i = 0;  // 考虑time不从0开始
    for (const auto& kv : deta) {
        inDeta[i] = kv.second;
        i++;
    }

    plan = fftw_plan_dft_r2c_1d(N, inDeta, outDeta, FFTW_ESTIMATE);  // 进行傅里叶变换

    fftw_execute(plan);

    std::map<QTime, double> result;
    i = 0;
    for (const auto& kv : deta) {
        // result[kv.first] = std::hypot(outDeta[i][0], outDeta[i][1]);
        result[kv.first] = sqrt(outDeta[i][0] * outDeta[i][0] + outDeta[i][1] * outDeta[i][1]);
        i++;
    }

    fftw_destroy_plan(plan);
    fftw_free(inDeta);
    fftw_free(outDeta);
    return result;
}
