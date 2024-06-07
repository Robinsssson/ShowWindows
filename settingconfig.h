#ifndef SETTINGCONFIG_H
#define SETTINGCONFIG_H

#include <QDialog>
#include <QWidget>

namespace Ui {
    class SettingConfig;
}

class SettingConfig : public QDialog {
    Q_OBJECT

  public:
    explicit SettingConfig(QWidget *parent = nullptr);
    ~SettingConfig();
  public slots:
    void on_pushButtonFirm_clicked();
    void on_pushButtonCancel_clicked();

  private:
    Ui::SettingConfig *ui;
};

#endif  // SETTINGCONFIG_H
