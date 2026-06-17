#include "LogWidget.h"

namespace stock_gui {

LogWidget::LogWidget(QWidget* parent)
    : QWidget(parent)
    , mainLayout_(nullptr)
    , logTextEdit_(nullptr)
    , clearButton_(nullptr)
    , placeholderLabel_(nullptr)
{
    SetupUI();
}

LogWidget::~LogWidget() = default;

void LogWidget::SetupUI() {
    mainLayout_ = new QVBoxLayout(this);

    // 日志文本框
    logTextEdit_ = new QTextEdit(this);
    logTextEdit_->setReadOnly(true);
    logTextEdit_->setPlaceholderText("实时日志显示区域（待实现）");

    mainLayout_->addWidget(logTextEdit_);

    // 清空按钮
    clearButton_ = new QPushButton("清空日志", this);
    connect(clearButton_, &QPushButton::clicked, [this]() {
        logTextEdit_->clear();
    });

    mainLayout_->addWidget(clearButton_);
}

}  // namespace stock_gui