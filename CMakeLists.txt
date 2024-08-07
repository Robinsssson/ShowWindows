cmake_minimum_required(VERSION 3.5)

project(ShowWindows VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OpenCV_DIR "C:/dev/opencv/opencv_with_qt6_build/install")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/mingw_64/lib/cmake")
    set(CMAKE_C_COMPILER "C:/Qt/Tools/mingw1120_64/bin/gcc.exe")
    set(FFTW3_INCLUDE_DIR "C:/Projects/Qt/OpencvDevelop/ShowWindows/Utility/fftw-3.3.5-dll64/include")
    set(FFTW3_LIB_DIR "C:/Projects/Qt/OpencvDevelop/ShowWindows/Utility/fftw-3.3.5-dll64/lib")
    set(FFTW3_LIBRARIES "${FFTW3_LIB_DIR}/libfftw3-3.dll.a")

    include_directories(${FFTW3_INCLUDE_DIR})

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")
    set(Qt_DIR "/opt/Qt/6.5.3/gcc_64/lib/cmake/Qt6")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)
find_package(OpenCV REQUIRED)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Project_Config.h
        CaptureShowTask/CaptureShowTask.cpp
        CaptureShowTask/CaptureShowTask.h
        CaptureTask/CaptureTask.cpp
        CaptureTask/CaptureTask.h
        ImageProcess/ImageProcess.cpp
        ImageProcess/ImageProcess.h
        ImageProcessTask/ImageProcessTask.cpp
        ImageProcessTask/ImageProcessTask.h
        MatQueue/SingletonMatQueue.cpp
        MatQueue/SingletonMatQueue.h
        AxesFreshTask/axesfreshtask.h
        AxesFreshTask/axesfreshtask.cpp
        Utility/csv/csv.h
        Utility/fftw-3.3.5-dll64/include/fftw3.h
        Utility/fft/fft_from_csv.cpp
        Utility/fft/fft_from_csv.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ShowWindows
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        .clang-format
        clang-format.py
        BugLog.md
        FftFreshTask/fftfreshtask.h FftFreshTask/fftfreshtask.cpp
        settingconfig.h settingconfig.cpp settingconfig.ui
        Utility/AnsysSetting/ansyssetting.h
        Utility/AnsysSetting/ansyssetting.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ShowWindows APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ShowWindows SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ShowWindows
            ${PROJECT_SOURCES}
        )
    endif()
endif()
include_directories(ShowWindows ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ShowWindows PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(ShowWindows PRIVATE ${OpenCV_LIBS} ${FFTW3_LIBRARIES})
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ShowWindows)
endif()
set_target_properties(ShowWindows PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ShowWindows
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ShowWindows)
endif()
