#include <QApplication>

#include "mainwindow.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("显示工具软件V1.1");
    w.show();
    return a.exec();
}
