#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "MatQueue/SingletonMatQueue.h"
#include "Project_Config.h"
#include "ImageProcess/ImageProcess.h"

/* 2023/11/19
 * 线程池的设计
 *  1.此项目涉及到 图像采集 图像处理 图像展示 数据表格展示
 *      在其中，图像采集和图像展示分别对应队列的enqueue和dequeue，因而选择通过图像队列进行缓存操作
 *  2.设计思路，主线程负责界面维持和子线程的调度任务
 *      子线程(1)负责图像数据的采集，不断地进行enqueue操作。
 *      子线程(2)负责图像数据的处理，在初始图像队列进行dequeue操作，对处理后的图像队列进行enqueue因为耗时长比较长，工作量多，使用多个线程进行维持
 *      子线程(3)负责图像数据的显示，不断的对(2)维持的处理后图像队列进行dequeue操作进行显示。
 *      子线程(4)负责接受图像数据处理后的结果，使用vector动态数组进行pushback存储，并实时显示在图像窗口上。
 *  3.设计细节
 *      1.队列操作需要进行加锁设计，否则会导致ThreadUnsafeProblem
 *      2.子线程(1)入队操作后，队列为按时间排序的有序队列，子线程(2)如果有多个线程进行处理图像算法的话，会导致图像顺序错位，
 *      因而选择针对一张照片的多线程处理办法。
 *      3.子线程(3)负责对图像的显示，因而对(2)输出的图像有可排序、可索引或者为按照时间的正序排列
 *      4.子线程(4)为子线程(3)获得的同步线程操作，因而针对子线程(3)和(4)需要进行线程同步。
 *  4.实现路径
 *      通过QThreadPool单例类，创建多个子处理类，在主线程安排入池逻辑进行Start操作。
 *
 *  2023/11/20
 *  线程池的优化
 *      1.考虑采用C++ priority_queue进行数据排序，在队列操作中采用struct {cv::mat ,QTime}进行数据标识，并通过id进行排序
 *      2.线程池目前测试无任何非期望运行情况，但是任有一下需要优化的细节
 *          2.1 线程同步设计
 *              线程池在运行任务的过程中往往是从队列中获取一个完整的处理任务进行测试，因而不符合提速的基本要求，期望可以拆分处理任务以达到线程高效利用的目的
 *              线程池在运行的过程尽管已经进行了优化设计，但仍然只有单个线程加入池中
 *          2.2 待定....
 * */

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // int Initialization
    qDebug() <<"MainThread" << QThread::currentThreadId();
    m_captureNumber = RefreshCameraNum();
    RefreshCaptureSelect();

    // object Initialization

    threadAxesFreshTask = new QThread;
    threadVideoShowTask = new QThread;
    threadVideoTask = new QThread;
    axesFreshTask = new AxesFreshTask(ui->chartView);
    m_captureTask = new CaptureTask;
    m_captureShowTask = new CaptureShowTask(ui->VideoShow);
    m_captureShowTask->moveToThread(threadVideoShowTask);
    m_captureTask->moveToThread(threadVideoTask);
    axesFreshTask->moveToThread(threadAxesFreshTask);

    axesFreshTask->setCalFunction(ImageProcess::calGrayPercent);
    Ui_Init(ui);
    ui->textBrowser->append(tr("测试\n"));

    SingletonMatQueue::GetInstance();

    //connect
    connect(this, QOverload<int>::of(&MainWindow::thisCapture), m_captureTask,
            &CaptureTask::getCaptureNumber);
    connect(this, &MainWindow::switchCapture, m_captureTask, &CaptureTask::getCaptureStatus);
    connect(this, &MainWindow::switchCapture, m_captureShowTask, &CaptureShowTask::getCaptureStatus, Qt::QueuedConnection);
    connect(this, QOverload<QString>::of(&MainWindow::SendVideoFileName), m_captureTask,
            &CaptureTask::SetVideo);
    connect(this, &MainWindow::SendFpsNumber, m_captureShowTask, &CaptureShowTask::GetFpsNumber, Qt::QueuedConnection);
    connect(m_captureShowTask, &CaptureShowTask::SendMat, axesFreshTask, &AxesFreshTask::axesFresh, Qt::QueuedConnection);

    threadVideoTask->start(QThread::HighestPriority);
    threadAxesFreshTask->start(QThread::LowPriority);
    threadVideoShowTask->start(QThread::HighPriority);
}

void MainWindow::Ui_Init(Ui::MainWindow *_ui)
{
    _ui->VideoShow->setMinimumSize(480, 400);
    QImage image(480, 400, QImage::Format_RGB888);
    image.fill(QColor(Qt::black));
    _ui->VideoShow->setPixmap(QPixmap::fromImage(image));
    _ui->chartView->setMinimumSize(480, 400);
}

MainWindow::~MainWindow() {
    threadVideoShowTask->quit();
    threadVideoShowTask->wait();
    threadVideoShowTask->deleteLater();

    threadVideoTask->quit();
    threadVideoTask->wait();
    threadVideoTask->deleteLater();

    threadAxesFreshTask->quit();
    threadAxesFreshTask->wait();
    threadAxesFreshTask->deleteLater();


    delete ui;
    delete axesFreshTask;
    delete m_captureShowTask;
    delete m_captureTask;
}

void MainWindow::on_pushButtonOpen_clicked() {
    if (m_captureOpenFlag)
        return;
    m_captureOpenFlag = true;
    emit switchCapture(m_captureOpenFlag);
    ui->textBrowser->append(tr("摄像头开启\n"));

}

void MainWindow::on_pushButtonClose_clicked() {
    if (!m_captureOpenFlag)
        return;
    m_captureOpenFlag = false;
    emit switchCapture(m_captureOpenFlag);
    m_videoFileFlag = false;
    QThreadPool::globalInstance();
    QImage image(480, 400, QImage::Format_RGB888);
    image.fill(QColor(Qt::black));
    ui->VideoShow->setFixedSize(480, 400);
    ui->VideoShow->setPixmap(QPixmap::fromImage(image));
    ui->textBrowser->append(tr("摄像头关闭\n"));
}

void MainWindow::on_actionFPS_triggered() {
    bool checkConfig = false;
    m_fpsConfig = QInputDialog::getInt(this, tr("输入FPS"), tr("输入Int"), m_fpsConfig, 1, 1000, 1, &checkConfig);
    if (!checkConfig)
        QMessageBox::warning(this, tr("非法输入"), tr("输入值无效！"));
    ui->textBrowser->append("FPS:" + QString::number(m_fpsConfig) + '\n');
    emit SendFpsNumber(m_fpsConfig);
}


void MainWindow::on_actionImportVideos_triggered() {
    m_videoFileName = QFileDialog::getOpenFileName(this, tr("打开视频文件"), "C:/",
                                                   tr("video files(*.avi *mp4);;All files(*.*)"));
    if (m_videoFileName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Failed to find");
        return;
    }
    emit SendVideoFileName(m_videoFileName);
    m_videoFileFlag = true;
}

void MainWindow::on_pushButtonCreateChart_clicked() {
    if (!m_chart_windows_status) {
    } else {

    }
}

std::vector<int> MainWindow::RefreshCameraNum() {
    auto *tmp_capture = new cv::VideoCapture;
    int _count;
    std::vector<int> __v;
    for (_count = 0; _count < MAX_CAPTURE_NUM; _count++) {
        tmp_capture->open(_count);
        if (!tmp_capture->isOpened())
            continue;
        __v.push_back(_count);
        tmp_capture->release();//一定要释放 否则程序进程不能完全退出
    }
    delete tmp_capture;
    return __v;
}

void MainWindow::RefreshCaptureSelect() {
    for (int _index : m_captureNumber) {
        auto *CAPTURE_ACTION_CREATE(_index) = new QAction(QString::number(_index), ui->menuCaptureSelect);
        ui->menuCaptureSelect->addAction(CAPTURE_ACTION_CREATE(_index));
        connect(CAPTURE_ACTION_CREATE(_index), QOverload<bool>::of(&::QAction::triggered),
                [=](bool) {
                    MainWindow::m_selectedCapture = _index;
                    ui->textBrowser->append(tr("摄像头") + QString::number(m_selectedCapture) + tr("启用\n"));
                    emit thisCapture(m_selectedCapture);
                });
    }
}
