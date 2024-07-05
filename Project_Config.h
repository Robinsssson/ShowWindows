//
// Created by Robinson on 2023/11/19.
//

#ifndef OPENCV_PROJECT_PROJECT_CONFIG_H
#define OPENCV_PROJECT_PROJECT_CONFIG_H
#include <QString>

const int MAX_CAPTURE_NUM = 10;
#define CAPTURE_ACTION_CREATE(n) capture_action##n
#define DEBUG_MODE

#ifndef DEBUG_MODE
const QString CONFIG_PATH = ".config\\";
#else
const QString CONFIG_PATH = "C:\\Projects\\Qt\\OpencvDevelop\\ShowWindows\\.config\\";
#endif

constexpr int AXES_H = 300;
constexpr int AXES_W = 600;
#endif  // OPENCV_PROJECT_PROJECT_CONFIG_H
