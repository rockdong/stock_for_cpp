#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>

namespace stock_gui {

/**
 * @brief 日志界面 - 显示实时日志
 *
 * 功能（占位，待实现）:
 * - 日志文本显示
 * - 日志滚动和搜索
 * - 日志级别过滤
 * - 日志导出
 */
class LogWidget : public QWidget {
    Q_OBJECT

public:
    explicit LogWidget(QWidget* parent = nullptr);
    ~LogWidget() override;

private:
    void SetupUI();

    QVBoxLayout* mainLayout_;
    QTextEdit* logTextEdit_;
    QPushButton* clearButton_;
    QLabel* placeholderLabel_;
};

}  // namespace stock_gui