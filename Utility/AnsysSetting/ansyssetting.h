#ifndef ANSYSSETTING_H
#define ANSYSSETTING_H
#include <QChart>
#include <QChartView>
#include <QFile>
#include <QJsonObject>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QtWidgets>

#include "../../AxesFreshTask/axesfreshtask.h"
class AnsysSetting : public QObject {
    Q_OBJECT
public:
    explicit AnsysSetting(QFile &file, QObject *parent = nullptr);
    ~AnsysSetting();
    void ansys(QWidget *parent);
    QVector<int> get_config(QVector<QString> &strs);
    QPushButton *m_button;
    QMap<QString, QVector<QPair<QChartView *, AxesFreshTask *>>> *chart_map;
    QWidget *global_aw, *global_tw, *m_parent;

public slots:
    void reDraw(bool);

private:
    void createTextWidgets(QWidget *parent, QVBoxLayout *layout, const QJsonObject &textObject);
    void createAxesWidgets(QWidget *parent, QVBoxLayout *parent_vbox, const QJsonObject &axesObject);
    void addButton(QWidget *parent, QVBoxLayout *layout, QWidget *connectParent);

    QJsonObject m_json_object;
    QFile *m_config_file;
    QMap<QString, QLineEdit *> qmap;
};

#endif  // ANSYSSETTING_H
