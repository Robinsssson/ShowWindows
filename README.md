# show windows

## Tool Chains

- Platform: Windows11/10

- Toolchains: `Qt6.5.3`, `opencv 4.8.1`, `gcc-qt`

- 3rdparty:[`fftw3-3`](https://www.fftw.org/download.html), [`csv`](https://github.com/ben-strasser/fast-cpp-csv-parser)

- License: GPL

- How to build it? add all relay file and toolchains in your computer. use `cmake --build`to compiler the project.

## development docs [中文]

本项目通过`mainwindows.cpp`来组织所有文件。在构造函数中创建任务线程。

### 文件树

```shell
ShowWindows
    ├─AxesFreshTask  		# Axes坐标更新
    ├─CaptureShowTask		# 摄像数据展示
    ├─CaptureTask			# 摄像数据采集
    ├─Docs					# 文档
    ├─FftFreshTask			# [弃用]
    ├─ImageProcess			# 图像处理算法
    ├─ImageProcessTask		# 图像处理线程
    ├─MatQueue				# 图像FIFO
    └─Utility				# 其他组件
        ├─AnsysSetting      # Axes Text 的Json解析
        ├─csv				# csv库
        ├─fft				# fft使用
        └─fftw-3.3.5-dll64	# fftw调度
```

### 线程调度

- 线程一：`CaptureTask`获取设备的摄像头数据交付给处理算法，主要负责摄像图片的获取，通过`cv::VideoCapture`类的封装来实现。

- 线程二：`CaptureShowTask`获取**线程一**的经过处理过后的算法在Label上展示。

- 线程三：`AxesFreshTask`维护了一个基于`.config\setting.json`创建的Axes类。

  注：`setting.json`字段解析`Utility\AnsysSetting\AnsysSetting.cpp`：

  - "Text"字段描述了显示在左上方文字区的`TextLine`展示。
  - "Axes"字段描述了坐标轴的行为



  
