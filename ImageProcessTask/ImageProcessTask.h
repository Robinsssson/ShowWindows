//
// Created by Robinson on 2023/11/25.
//

#ifndef OPENCVPROJECT_IMAGEPROCESSTASK_H
#define OPENCVPROJECT_IMAGEPROCESSTASK_H

#include <QRunnable>
#include <QObject>

class ImageProcessTask : public QRunnable {
public:
    ImageProcessTask() : QRunnable() {
        setAutoDelete(true);
    }

    void run() override;
};


#endif //OPENCVPROJECT_IMAGEPROCESSTASK_H
