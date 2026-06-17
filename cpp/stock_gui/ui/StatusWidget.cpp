#include "StatusWidget.h"

namespace stock_gui {

StatusWidget::StatusWidget(QWidget* parent)
    : QWidget(parent)
    , mainLayout_(nullptr)
    , systemStatusGroup_(nullptr)
    , databaseStatusGroup_(nullptr)
    , apiStatusGroup_(nullptr)
    , placeholderLabel_(nullptr)
{
    SetupUI();
}

StatusWidget::~StatusWidget() = default;

void StatusWidget::SetupUI() {
    mainLayout_ = new QVBoxLayout(this);

    // 系统状态组
    systemStatusGroup_ = new QGroupBox("系统状态", this);
    QVBoxLayout* systemLayout = new QVBoxLayout(systemStatusGroup_);
    QLabel* systemLabel = new QLabel("系统运行正常", this);
    systemLayout->addWidget(systemLabel);
    mainLayout_->addWidget(systemStatusGroup_);

    // 数据库状态组
    databaseStatusGroup_ = new QGroupBox("数据库状态", this);
    QVBoxLayout* dbLayout = new QVBoxLayout(databaseStatusGroup_);
    QLabel* dbLabel = new QLabel("数据库连接: 正常", this);
    dbLayout->addWidget(dbLabel);
    mainLayout_->addWidget(databaseStatusGroup_);

    // API状态组
    apiStatusGroup_ = new QGroupBox("API状态", this);
    QVBoxLayout* apiLayout = new QVBoxLayout(apiStatusGroup_);
    QLabel* apiLabel = new QLabel("API密钥: 已配置", this);
    apiLayout->addWidget(apiLabel);
    mainLayout_->addWidget(apiStatusGroup_);

    // 占位标签
    placeholderLabel_ = new QLabel("状态界面（基础版本）", this);
    placeholderLabel_->setAlignment(Qt::AlignCenter);
    placeholderLabel_->setStyleSheet("QLabel { color: gray; font-size: 12pt; }");
    mainLayout_->addWidget(placeholderLabel_);
}

}  // namespace stock_gui