#include "QueryPanel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QStyle>
#include <QDebug>

QueryPanel::QueryPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    setupResultsTable();
}

QueryPanel::~QueryPanel() {
    // 析构时无需特殊处理
}

void QueryPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ========== 股票查询区域 ==========

    QGroupBox* stockQueryGroup = new QGroupBox("股票基本信息查询", this);
    QHBoxLayout* stockQueryLayout = new QHBoxLayout(stockQueryGroup);

    QLabel* stockLabel = new QLabel("搜索关键词:", this);
    m_stockKeywordEdit = new QLineEdit(this);
    m_stockKeywordEdit->setPlaceholderText("输入股票代码（如：600000）或名称（如：浦发银行）");
    m_stockKeywordEdit->setClearButtonEnabled(true);

    m_queryStockBtn = new QPushButton("查询", this);
    m_queryStockBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    m_queryStockBtn->setToolTip("查询股票基本信息");
    m_queryStockBtn->setStyleSheet("background-color: #2196f3; color: white;");
    connect(m_queryStockBtn, &QPushButton::clicked, this, [this]() {
        if (!m_stockKeywordEdit->text().isEmpty()) {
            emit stockQueryRequested(m_stockKeywordEdit->text());
            m_statusLabel->setText("正在查询股票信息...");
        } else {
            QMessageBox::warning(this, "输入错误", "请输入股票代码或名称");
        }
    });

    stockQueryLayout->addWidget(stockLabel);
    stockQueryLayout->addWidget(m_stockKeywordEdit, 3);
    stockQueryLayout->addWidget(m_queryStockBtn);
    mainLayout->addWidget(stockQueryGroup);

    // ========== 价格查询区域 ==========

    QGroupBox* priceQueryGroup = new QGroupBox("价格数据查询", this);
    QVBoxLayout* priceQueryLayout = new QVBoxLayout(priceQueryGroup);

    QHBoxLayout* priceInputLayout = new QHBoxLayout();
    QLabel* tsCodeLabel = new QLabel("股票代码:", this);
    m_priceTsCodeEdit = new QLineEdit(this);
    m_priceTsCodeEdit->setPlaceholderText("例如：600000.SH");
    m_priceTsCodeEdit->setClearButtonEnabled(true);

    priceInputLayout->addWidget(tsCodeLabel);
    priceInputLayout->addWidget(m_priceTsCodeEdit, 2);
    priceQueryLayout->addLayout(priceInputLayout);

    QHBoxLayout* dateRangeLayout = new QHBoxLayout();
    QLabel* dateLabel = new QLabel("日期范围:", this);

    m_priceStartDate = new QDateEdit(this);
    m_priceStartDate->setCalendarPopup(true);
    m_priceStartDate->setDisplayFormat("yyyy-MM-dd");
    m_priceStartDate->setDate(QDate::currentDate().addMonths(-1));  // 默认一个月前

    QLabel* toDateLabel = new QLabel("至", this);

    m_priceEndDate = new QDateEdit(this);
    m_priceEndDate->setCalendarPopup(true);
    m_priceEndDate->setDisplayFormat("yyyy-MM-dd");
    m_priceEndDate->setDate(QDate::currentDate());  // 默认今天

    m_queryPriceBtn = new QPushButton("查询", this);
    m_queryPriceBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    m_queryPriceBtn->setToolTip("查询价格数据");
    m_queryPriceBtn->setStyleSheet("background-color: #2196f3; color: white;");
    connect(m_queryPriceBtn, &QPushButton::clicked, this, [this]() {
        if (!m_priceTsCodeEdit->text().isEmpty()) {
            emit priceQueryRequested(
                m_priceTsCodeEdit->text(),
                m_priceStartDate->dateTime(),
                m_priceEndDate->dateTime()
            );
            m_statusLabel->setText("正在查询价格数据...");
        } else {
            QMessageBox::warning(this, "输入错误", "请输入股票代码");
        }
    });

    dateRangeLayout->addWidget(dateLabel);
    dateRangeLayout->addWidget(m_priceStartDate);
    dateRangeLayout->addWidget(toDateLabel);
    dateRangeLayout->addWidget(m_priceEndDate);
    dateRangeLayout->addWidget(m_queryPriceBtn);
    dateRangeLayout->addStretch();
    priceQueryLayout->addLayout(dateRangeLayout);

    mainLayout->addWidget(priceQueryGroup);

    // ========== 分析结果查询区域 ==========

    QGroupBox* analysisQueryGroup = new QGroupBox("分析结果查询", this);
    QVBoxLayout* analysisQueryLayout = new QVBoxLayout(analysisQueryGroup);

    QHBoxLayout* strategyLayout = new QHBoxLayout();
    QLabel* strategyLabel = new QLabel("策略:", this);
    m_strategyCombo = new QComboBox(this);
    m_strategyCombo->addItem("所有策略", "");
    m_strategyCombo->addItem("EMA17Breakout", "EMA17Breakout");
    m_strategyCombo->addItem("MACD", "MACD");
    m_strategyCombo->addItem("RSI", "RSI");
    m_strategyCombo->addItem("BOLL", "BOLL");
    m_strategyCombo->addItem("KDJ", "KDJ");

    strategyLayout->addWidget(strategyLabel);
    strategyLayout->addWidget(m_strategyCombo);
    strategyLayout->addStretch();
    analysisQueryLayout->addLayout(strategyLayout);

    QHBoxLayout* analysisDateLayout = new QHBoxLayout();
    QLabel* analysisDateLabel = new QLabel("日期范围:", this);

    m_analysisStartDate = new QDateEdit(this);
    m_analysisStartDate->setCalendarPopup(true);
    m_analysisStartDate->setDisplayFormat("yyyy-MM-dd");
    m_analysisStartDate->setDate(QDate::currentDate().addDays(-7));  // 默认一周前

    QLabel* analysisToDateLabel = new QLabel("至", this);

    m_analysisEndDate = new QDateEdit(this);
    m_analysisEndDate->setCalendarPopup(true);
    m_analysisEndDate->setDisplayFormat("yyyy-MM-dd");
    m_analysisEndDate->setDate(QDate::currentDate());  // 默认今天

    m_queryAnalysisBtn = new QPushButton("查询", this);
    m_queryAnalysisBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    m_queryAnalysisBtn->setToolTip("查询分析结果");
    m_queryAnalysisBtn->setStyleSheet("background-color: #2196f3; color: white;");
    connect(m_queryAnalysisBtn, &QPushButton::clicked, this, [this]() {
        emit analysisQueryRequested(
            m_strategyCombo->currentData().toString(),
            m_analysisStartDate->dateTime(),
            m_analysisEndDate->dateTime()
        );
        m_statusLabel->setText("正在查询分析结果...");
    });

    analysisDateLayout->addWidget(analysisDateLabel);
    analysisDateLayout->addWidget(m_analysisStartDate);
    analysisDateLayout->addWidget(analysisToDateLabel);
    analysisDateLayout->addWidget(m_analysisEndDate);
    analysisDateLayout->addWidget(m_queryAnalysisBtn);
    analysisDateLayout->addStretch();
    analysisQueryLayout->addLayout(analysisDateLayout);

    mainLayout->addWidget(analysisQueryGroup);

    // ========== 查询结果区域 ==========

    QLabel* resultsLabel = new QLabel("查询结果:", this);
    resultsLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(resultsLabel);

    m_resultsTable = new QTableView(this);
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setSortingEnabled(true);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);
    m_resultsTable->verticalHeader()->setVisible(false);

    mainLayout->addWidget(m_resultsTable);

    // ========== 状态和导出区域 ==========

    QHBoxLayout* statusLayout = new QHBoxLayout();

    m_statusLabel = new QLabel("就绪", this);
    m_statusLabel->setStyleSheet("color: #666666; font-size: 11px;");

    m_countLabel = new QLabel("查询结果：0 条", this);
    m_countLabel->setStyleSheet("color: #4caf50; font-weight: bold; font-size: 12px;");

    m_exportBtn = new QPushButton("导出结果", this);
    m_exportBtn->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_exportBtn->setToolTip("导出查询结果为 CSV 文件");
    connect(m_exportBtn, &QPushButton::clicked, this, [this]() {
        emit exportDataRequested("csv");
    });

    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_countLabel);
    statusLayout->addWidget(m_exportBtn);
    mainLayout->addLayout(statusLayout);
}

void QueryPanel::setupResultsTable() {
    m_resultsModel = new QStandardItemModel(this);
    m_resultsTable->setModel(m_resultsModel);

    // 设置默认列（示例数据）
    QStringList headers;
    headers << "股票代码" << "股票名称" << "交易日期" << "收盘价" << "成交量";
    m_resultsModel->setHorizontalHeaderLabels(headers);

    // 添加示例数据（模拟）
    for (int i = 0; i < 5; ++i) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString("60000%1.SH").arg(i));
        row << new QStandardItem(QString("示例股票%1").arg(i));
        row << new QStandardItem(QDate::currentDate().toString("yyyy-MM-dd"));
        row << new QStandardItem(QString("%1.00").arg(10 + i));
        row << new QStandardItem(QString("%1万").arg(100 + i * 10));
        m_resultsModel->appendRow(row);
    }

    m_countLabel->setText(QString("查询结果：%1 条").arg(m_resultsModel->rowCount()));
}

void QueryPanel::setQueryResults(const QStringList& headers, const QList<QStringList>& data) {
    m_resultsModel->clear();
    m_resultsModel->setHorizontalHeaderLabels(headers);

    for (const QStringList& row : data) {
        QList<QStandardItem*> items;
        for (const QString& value : row) {
            items << new QStandardItem(value);
        }
        m_resultsModel->appendRow(items);
    }

    m_countLabel->setText(QString("查询结果：%1 条").arg(m_resultsModel->rowCount()));
    m_statusLabel->setText("查询完成");
}

void QueryPanel::clearResults() {
    m_resultsModel->clear();
    m_countLabel->setText("查询结果：0 条");
    m_statusLabel->setText("已清空");
}

bool QueryPanel::validateQueryInput() {
    // 验证逻辑（如有需要）
    return true;
}