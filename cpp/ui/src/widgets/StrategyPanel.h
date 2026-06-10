/**
 * @file StrategyPanel.h
 * @brief 策略参数面板控件
 * @author StockLens Team
 * @version 1.0.0
 */

#ifndef STRATEGY_PANEL_H
#define STRATEGY_PANEL_H

#include <QWidget>
#include <QComboBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <vector>
#include <map>
#include <string>

namespace ui {

/**
 * @brief 策略参数结构
 */
struct StrategyParam {
    std::string name;           // 参数名称
    std::string displayName;    // 显示名称
    double defaultValue;        // 默认值
    double minValue;            // 最小值
    double maxValue;            // 最大值
    double step;                // 步进值
    std::string type;           // 类型（slider/spinbox/checkbox/combobox）
};

/**
 * @brief 策略参数面板
 * 
 * 提供可视化的策略参数调整界面，支持：
 * - 策略选择下拉框
 * - 参数滑块/输入框
 * - 实时计算触发
 */
class StrategyPanel : public QWidget {
    Q_OBJECT

public:
    explicit StrategyPanel(QWidget* parent = nullptr);
    ~StrategyPanel() override = default;

    /**
     * @brief 设置可用策略列表
     * @param strategies 策略名称数组
     */
    void setAvailableStrategies(const std::vector<std::string>& strategies);

    /**
     * @brief 设置当前策略的参数配置
     * @param params 参数数组
     */
    void setStrategyParams(const std::vector<StrategyParam>& params);

    /**
     * @brief 获取当前选中的策略名称
     */
    QString currentStrategy() const;

    /**
     * @brief 获取当前参数值
     */
    std::map<std::string, double> currentParams() const;

    /**
     * @brief 设置参数值
     */
    void setParamValue(const std::string& name, double value);

    /**
     * @brief 清除所有参数控件
     */
    void clearParams();

signals:
    /**
     * @brief 策略变更时发出
     * @param strategyName 新策略名称
     */
    void strategyChanged(const QString& strategyName);

    /**
     * @brief 参数变更时发出
     * @param paramName 参数名称
     * @param value 新值
     */
    void paramChanged(const QString& paramName, double value);

    /**
     * @brief 用户点击计算按钮时发出
     */
    void calculateRequested();

private:
    void setupUI();
    void createParamWidget(const StrategyParam& param);
    
    QComboBox* m_strategyCombo = nullptr;
    QGroupBox* m_paramsGroup = nullptr;
    QPushButton* m_calculateBtn = nullptr;
    
    std::map<std::string, QWidget*> m_paramWidgets;
};

} // namespace ui

#endif // STRATEGY_PANEL_H