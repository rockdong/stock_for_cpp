/**
 * @file StrategyPanel.cpp
 * @brief 策略参数面板实现
 * @author StockLens Team
 * @version 1.0.0
 */

#include "StrategyPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

namespace ui {

StrategyPanel::StrategyPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void StrategyPanel::setupUI() {
    auto* mainLayout = new QVBoxLayout(static_cast<QWidget*>(this));
    
    // 策略选择
    auto* strategyLayout = new QHBoxLayout();
    auto* strategyLabel = new QLabel("策略:");
    m_strategyCombo = new QComboBox();
    
    strategyLayout->addWidget(strategyLabel);
    strategyLayout->addWidget(m_strategyCombo);
    mainLayout->addLayout(strategyLayout);
    
    // 参数区域（可滚动）
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumHeight(200);
    
    m_paramsGroup = new QGroupBox("策略参数");
    auto* paramsLayout = new QVBoxLayout(m_paramsGroup);
    paramsLayout->addStretch();  // 占位，后续动态添加
    
    scrollArea->setWidget(m_paramsGroup);
    mainLayout->addWidget(scrollArea);
    
    // 计算按钮
    m_calculateBtn = new QPushButton("立即计算");
    m_calculateBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #3b82f6;"
        "  color: white;"
        "  border: none;"
        "  padding: 8px 16px;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2563eb;"
        "}"
    );
    mainLayout->addWidget(m_calculateBtn);
    
    // 连接信号槽
    connect(m_strategyCombo, &QComboBox::currentTextChanged,
            this, &StrategyPanel::strategyChanged);
    connect(m_calculateBtn, &QPushButton::clicked,
            this, &StrategyPanel::calculateRequested);
}

void StrategyPanel::setAvailableStrategies(
    const std::vector<std::string>& strategies) {
    
    m_strategyCombo->clear();
    for (const auto& s : strategies) {
        m_strategyCombo->addItem(QString::fromStdString(s));
    }
}

void StrategyPanel::setStrategyParams(
    const std::vector<StrategyParam>& params) {
    
    clearParams();
    
    auto* layout = m_paramsGroup->findChild<QVBoxLayout*>();
    if (!layout) return;
    
    for (const auto& param : params) {
        createParamWidget(param);
    }
    
    layout->addStretch();
}

void StrategyPanel::createParamWidget(const StrategyParam& param) {
    auto* layout = m_paramsGroup->findChild<QVBoxLayout*>();
    if (!layout) return;
    
    auto* rowLayout = new QHBoxLayout();
    auto* label = new QLabel(QString::fromStdString(param.displayName));
    label->setMinimumWidth(100);
    
    QWidget* valueWidget = nullptr;
    
    if (param.type == "slider") {
        auto* slider = new QSlider(Qt::Horizontal);
        slider->setRange(
            static_cast<int>(param.minValue * 100),
            static_cast<int>(param.maxValue * 100)
        );
        slider->setValue(static_cast<int>(param.defaultValue * 100));
        slider->setTickInterval(static_cast<int>(param.step * 100));
        
        // 显示当前值的标签
        auto* valueLabel = new QLabel(
            QString::number(param.defaultValue, 'f', 2));
        valueLabel->setMinimumWidth(50);
        
        connect(slider, &QSlider::valueChanged,
            [this, param, valueLabel](int value) {
                double dValue = value / 100.0;
                valueLabel->setText(QString::number(dValue, 'f', 2));
                emit paramChanged(QString::fromStdString(param.name), dValue);
            });
        
        rowLayout->addWidget(label);
        rowLayout->addWidget(slider, 1);
        rowLayout->addWidget(valueLabel);
        
        valueWidget = slider;
    } else if (param.type == "spinbox") {
        auto* spinBox = new QDoubleSpinBox();
        spinBox->setRange(param.minValue, param.maxValue);
        spinBox->setValue(param.defaultValue);
        spinBox->setSingleStep(param.step);
        spinBox->setDecimals(2);
        
        connect(spinBox, &QDoubleSpinBox::valueChanged,
            [this, param](double value) {
                emit paramChanged(QString::fromStdString(param.name), value);
            });
        
        rowLayout->addWidget(label);
        rowLayout->addWidget(spinBox);
        
        valueWidget = spinBox;
    } else if (param.type == "checkbox") {
        auto* checkBox = new QCheckBox(QString::fromStdString(param.displayName));
        checkBox->setChecked(param.defaultValue > 0);
        
        connect(checkBox, &QCheckBox::toggled,
            [this, param](bool checked) {
                emit paramChanged(QString::fromStdString(param.name), checked ? 1.0 : 0.0);
            });
        
        rowLayout->addWidget(checkBox);
        
        valueWidget = checkBox;
    }
    
    layout->addLayout(rowLayout);
    
    // 保存控件引用
    m_paramWidgets[param.name] = valueWidget;
}

QString StrategyPanel::currentStrategy() const {
    return m_strategyCombo->currentText();
}

std::map<std::string, double> StrategyPanel::currentParams() const {
    std::map<std::string, double> params;
    
    for (const auto& [name, widget] : m_paramWidgets) {
        if (auto* slider = qobject_cast<QSlider*>(widget)) {
            params[name] = slider->value() / 100.0;
        } else if (auto* spinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
            params[name] = spinBox->value();
        } else if (auto* checkBox = qobject_cast<QCheckBox*>(widget)) {
            params[name] = checkBox->isChecked() ? 1.0 : 0.0;
        }
    }
    
    return params;
}

void StrategyPanel::setParamValue(const std::string& name, double value) {
    auto it = m_paramWidgets.find(name);
    if (it == m_paramWidgets.end()) return;
    
    auto* widget = it->second;
    if (auto* slider = qobject_cast<QSlider*>(widget)) {
        slider->setValue(static_cast<int>(value * 100));
    } else if (auto* spinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
        spinBox->setValue(value);
    } else if (auto* checkBox = qobject_cast<QCheckBox*>(widget)) {
        checkBox->setChecked(value > 0);
    }
}

void StrategyPanel::clearParams() {
    auto* layout = m_paramsGroup->findChild<QVBoxLayout*>();
    if (!layout) return;
    
    // 删除所有参数控件
    for (auto& [name, widget] : m_paramWidgets) {
        if (widget) {
            widget->deleteLater();
        }
    }
    m_paramWidgets.clear();
    
    // 清空布局（保留 stretch）
    while (layout->count() > 1) {
        auto* item = layout->takeAt(0);
        if (item->layout()) {
            // 删除子布局中的所有控件
            while (item->layout()->count() > 0) {
                auto* childItem = item->layout()->takeAt(0);
                if (childItem->widget()) {
                    childItem->widget()->deleteLater();
                }
                delete childItem;
            }
        }
        delete item;
    }
}

} // namespace ui