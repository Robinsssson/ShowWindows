#include "../AnsysSetting/ansyssetting.h"

#include <QChartView>
#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QtCharts>

#include "../../ImageProcess/ImageProcess.h"
#include "../../Project_Config.h"

AnsysSetting::AnsysSetting(QFile &file, QObject *parent) : QObject(parent), m_config_file(new QFile(CONFIG_PATH + "config.json")) {
    qDebug() << file.fileName();
    if (!file.exists()) qDebug() << "no file found";
    QJsonParseError json_error;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto json = QJsonDocument::fromJson(file.readAll(), &json_error);
    file.close();
    if (json_error.error != QJsonParseError::NoError) qDebug() << "Json Format Error";
    m_json_object = json.object();
    chart_map = new QMap<QString, QVector<QPair<QChartView *, AxesFreshTask *>>>();
}

AnsysSetting::~AnsysSetting() {
    m_config_file->remove();
    delete m_config_file;
    delete chart_map;
}

void AnsysSetting::ansys(QWidget *parent) {
    QVBoxLayout *parent_vbox = new QVBoxLayout(parent);
    auto *global_tw = new QWidget(parent);
    auto *global_aw = new QWidget(parent);
    parent_vbox->addWidget(global_tw);
    QSpacerItem *vspacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
    parent_vbox->addSpacerItem(vspacer);
    parent_vbox->addWidget(global_aw);
    QVBoxLayout *gtw_vbox = new QVBoxLayout(global_tw);

    for (const auto &obj : m_json_object.keys()) {
        if (obj == "Text")
            createTextWidgets(global_tw, gtw_vbox, m_json_object[obj].toObject());
        else if (obj == "Axes")
            createAxesWidgets(global_aw, parent_vbox, m_json_object[obj].toObject());
    }

    addButton(global_tw, gtw_vbox, global_aw);
}

void AnsysSetting::createTextWidgets(QWidget *parent, QVBoxLayout *layout, const QJsonObject &textObject) {
    auto *text_widget = new QWidget(parent);
    layout->addWidget(text_widget);
    for (const auto &wid_text : textObject.keys()) {
        auto *tw_vbox = new QVBoxLayout(text_widget);
        auto *title = new QLabel(wid_text, text_widget);
        tw_vbox->addWidget(title);

        const auto arrs = textObject[wid_text].toArray();
        for (const auto &str : arrs) {
            QHBoxLayout *tw_hbox = new QHBoxLayout(text_widget);
            QLabel *label = new QLabel(str.toString(), text_widget);
            QLineEdit *edit = new QLineEdit(text_widget);

            label->setMinimumWidth(50);
            edit->setMaximumHeight(20);
            qmap.insert(str.toString(), edit);

            tw_hbox->addWidget(label);
            tw_hbox->addWidget(edit);
            tw_vbox->addLayout(tw_hbox);
        }
    }
}

QWidget* TopLevelParentWidget (QWidget* widget)
{
    while (widget -> parentWidget() != Q_NULLPTR) widget = widget -> parentWidget() ;
    return widget ;
}

void AnsysSetting::createAxesWidgets(QWidget *parent, QVBoxLayout *parent_vbox, const QJsonObject &axesObject) {
    auto *axes_widget = new QWidget(parent);
    parent_vbox->addWidget(axes_widget);
    QGridLayout *grid = new QGridLayout(axes_widget);
    parent_vbox->addLayout(grid);

    int local ,col, row, num;
    auto str = ImageProcess::GetInstance().getName();
    auto axes_arr = axesObject[str].toArray();
    qDebug() <<"axes "<< axes_arr;
    local = axes_arr[0].toObject()["local"].toInt();
    row = local / 100, col = (local - row * 100) / 10;
    chart_map->insert(str, QVector<QPair<QChartView *, AxesFreshTask *>>(row * col, QPair<QChartView *, AxesFreshTask *>(new QChartView(axes_widget), new AxesFreshTask())));
    for (const auto& axes : axes_arr){
        local = axes.toObject()["local"].toInt();
        num = local % 10;
        for(const auto &viewPair : chart_map->value(str)) {
            auto &view = viewPair.first; auto &axes_obj = viewPair.second;
            grid->addWidget(view, floor((num - 1) / col), num - floor((num - 1) / col) * col - 1);
            axes_obj->LazyInit(view, axes.toObject()["attribute"].toObject());
        }
    }
}

void AnsysSetting::addButton(QWidget *parent, QVBoxLayout *layout, QWidget *connectParent) {
    m_button = new QPushButton("Submit", parent);
    QSpacerItem *v_spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout->addSpacerItem(v_spacer);
    layout->addWidget(m_button);
    QObject::connect(m_button, &QPushButton::clicked, connectParent, [this]() {
        if (!m_config_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to open config file";
            return;
        }
        QJsonObject obj;
        QJsonDocument docs;
        for (auto i = qmap.cbegin(), end = qmap.cend(); i != end; ++i) {
            obj.insert(i.key(), QJsonValue::fromVariant(i.value()->text()));
        }
        docs.setObject(obj);
        m_config_file->write(docs.toJson());
        m_config_file->close();
    });
}

QVector<int> AnsysSetting::get_config(QVector<QString> &strs) {
    m_config_file->open(QIODevice::ReadOnly | QIODevice::Text);
    if (!m_config_file->isOpen()) {
        qDebug() << "error";
        return QVector<int>();
    }
    QVector<int> vecs;
    QJsonParseError json_error;
    auto json = QJsonDocument::fromJson(m_config_file->readAll(), &json_error);
    m_config_file->close();
    if (json_error.error != QJsonParseError::NoError) qDebug() << "Json Format Error";
    auto config = json.object();
    for (auto &str : strs) {
        int ret = config[str].toVariant().toInt();
        qDebug() << "key:" << str << ret;
        vecs.push_back(ret);
    }
    return vecs;
}
