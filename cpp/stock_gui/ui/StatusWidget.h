#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

namespace stock_gui {

/**
 * @brief 状态界面 - 显示系统状态和统计信息
 *
 * 功能（占位，待实现）:
 * - 系统状态显示
 * - 数据库状态
 * - API状态
 * - 性能统计
 */
class StatusWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatusWidget(QWidget* parent = nullptr);
    ~StatusWidget() override;

private:
    void SetupUI();

    QVBoxLayout* mainLayout_;
    QGroupBox* systemStatusGroup_;
    QGroupBox* databaseStatusGroup_;
    QGroupBox* apiStatusGroup_;
    QLabel* placeholderLabel_;
};

}  // namespace stock_gui