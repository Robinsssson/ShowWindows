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
// extern MainWindow *mainWin;
/**
 * ImageProcess Function序列化，序列化的实现
 * 1. on-time 标签
 *  其决定是否在运行时进行坐标轴绘制操作，其通常是与一个emit绑定。
 * 2. 绑定的序列化
 *
 *  connect(m_emit, &emit::function, m_axes, fresh())
 */

AnsysSetting::AnsysSetting(QFile &file, QObject *parent) : QObject(parent), m_config_file(new QFile(CONFIG_PATH + "config.json")), global_aw(nullptr), global_tw(nullptr) {
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
void AnsysSetting::reDraw(bool ok) {
    qDebug() << "enter reDraw function";

    if (global_aw != nullptr) {
        delete global_aw;  // 删除旧的 global_aw
        global_aw = nullptr;
    }

    global_aw = new QWidget(this->m_parent);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->m_parent->layout());

    if (layout) {
        layout->addWidget(global_aw);
        createAxesWidgets(global_aw, layout, m_json_object["Axes"].toObject());
    } else {
        qWarning() << "Parent layout is null";
    }
}
void AnsysSetting::ansys(QWidget *parent) {
    this->m_parent = parent;
    QVBoxLayout *parent_vbox = new QVBoxLayout(parent);
    global_tw = new QWidget(parent);
    global_aw = new QWidget(parent);
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
        else {
        }
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

QWidget *TopLevelParentWidget(QWidget *widget) {
    while (widget->parentWidget() != Q_NULLPTR) widget = widget->parentWidget();
    return widget;
}

void AnsysSetting::createAxesWidgets(QWidget *parent, QVBoxLayout *parent_vbox, const QJsonObject &axesObject) {
    // Clear existing widgets from the layout

    // auto *axes_widget = new QWidget(parent);
    auto &axes_widget = parent;
    parent_vbox->addWidget(axes_widget);
    QGridLayout *grid = new QGridLayout(axes_widget);

    int local, col, row, num;
    auto str = ImageProcess::GetInstance().getName();

    auto axes_arr = axesObject[str].toArray();
    qDebug() << "axes " << axes_arr;

    local = axes_arr[0].toObject()["local"].toInt();
    row = local / 100;
    col = (local - row * 100) / 10;
    if (row <= 0 || col <= 0) throw "error created because of row or col <= 0";
    // if (!chart_map->contains(str)) {
    QVector<QPair<QChartView *, AxesFreshTask *>> chartVector(row * col);
    for (int i = 0; i < row * col; ++i) {
        auto *chartView = new QChartView(axes_widget);
        auto *axesTask = new AxesFreshTask(axes_widget);
        chartVector[i] = QPair<QChartView *, AxesFreshTask *>(chartView, axesTask);
    }
    chart_map->insert(str, chartVector);
    // }

    for (int i = 0; i < row * col; ++i) {
        local = axes_arr[i].toObject()["local"].toInt();
        num = local % 10;
        auto vec = chart_map->value(str);
        auto viewPair = vec[i];
        auto &view = viewPair.first;
        auto &axes_obj = viewPair.second;
        int m = floor((num - 1) / col);
        int n = num - floor((num - 1) / col) * col - 1;
        qDebug() << "number" << m << n;
        grid->addWidget(view, m + 1, n + 1);
        axes_obj->LazyInit(view, axes_arr[i].toObject()["attribute"].toObject());
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
