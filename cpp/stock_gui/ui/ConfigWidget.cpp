#include "ConfigWidget.h"

namespace stock_gui {

ConfigWidget::ConfigWidget(QWidget* parent)
    : QWidget(parent)
    , mainLayout_(nullptr)
    , placeholderLabel_(nullptr)
{
    SetupUI();
}

ConfigWidget::~ConfigWidget() = default;

void ConfigWidget::SetupUI() {
    mainLayout_ = new QVBoxLayout(this);

    placeholderLabel_ = new QLabel("配置界面（待实现）\n\n功能:\n- 配置参数显示\n- 配置编辑和保存\n- 数据库连接配置\n- API密钥管理", this);
    placeholderLabel_->setAlignment(Qt::AlignCenter);
    placeholderLabel_->setStyleSheet("QLabel { color: gray; font-size: 14pt; }");

    mainLayout_->addWidget(placeholderLabel_);
}

}  // namespace stock_gui