#include "settingconfig.h"

#include <QMessageBox>

#include "ImageProcess/ImageProcess.h"
#include "ui_settingconfig.h"
SettingConfig::SettingConfig(QWidget *parent) : QDialog(parent), ui(new Ui::SettingConfig) {
    ui->setupUi(this);
    ui->EditLKOptricalFlowTimes->setText(QString::number(ImageProcess::GetInstance().LKOptricalFlow_times));
}

SettingConfig::~SettingConfig() {
    delete ui;
}
void SettingConfig::on_pushButtonFirm_clicked() {
    auto gets = ui->EditLKOptricalFlowTimes->text();
    bool ok;
    int coverint = gets.toInt(&ok);
    if (ok == false) {
        QMessageBox::warning(this, tr("光流法捕捉点输入错误"), tr("请重试"));
    } else {
        ImageProcess::GetInstance().LKOptricalFlow_times = coverint;
        this->close();
    }
}
void SettingConfig::on_pushButtonCancel_clicked() {
    this->close();
}
