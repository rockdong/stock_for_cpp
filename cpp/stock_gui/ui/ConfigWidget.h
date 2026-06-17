#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

namespace stock_gui {

/**
 * @brief 配置界面 - 显示和编辑系统配置
 *
 * 功能（占位，待实现）:
 * - 配置参数显示
 * - 配置编辑和保存
 * - 数据库连接配置
 * - API密钥管理
 */
class ConfigWidget : public QWidget {
    Q_OBJECT

public:
    explicit ConfigWidget(QWidget* parent = nullptr);
    ~ConfigWidget() override;

private:
    void SetupUI();

    QVBoxLayout* mainLayout_;
    QLabel* placeholderLabel_;
};

}  // namespace stock_gui