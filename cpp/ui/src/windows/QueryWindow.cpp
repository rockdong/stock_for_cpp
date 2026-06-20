/**
 * @file QueryWindow.cpp
 * @brief 数据查询独立窗口实现
 */

#include "QueryWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QStatusBar>      // QStatusBar 完整定义
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

QueryWindow::QueryWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("数据查询 - Stock for C++");
    resize(800, 600);
    setupUI();
}

void QueryWindow::setDAOs(data::StockDAO* stockDAO, data::PriceDAO* priceDAO) {
    m_stockDAO = stockDAO;
    m_priceDAO = priceDAO;
}

void QueryWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // ========== 股票查询面板 ==========
    QGroupBox* stockGroup = new QGroupBox("股票查询", this);
    QHBoxLayout* stockLayout = new QHBoxLayout(stockGroup);
    stockLayout->addWidget(new QLabel("关键词:", this));
    m_stockKeywordInput = new QLineEdit(this);
    m_stockKeywordInput->setPlaceholderText("输入股票代码或名称");
    stockLayout->addWidget(m_stockKeywordInput);

    QPushButton* stockQueryBtn = new QPushButton("查询", this);
    connect(stockQueryBtn, &QPushButton::clicked, this, &QueryWindow::onStockQuery);
    stockLayout->addWidget(stockQueryBtn);
    stockLayout->addStretch();
    mainLayout->addWidget(stockGroup);

    // ========== 价格查询面板 ==========
    QGroupBox* priceGroup = new QGroupBox("价格查询", this);
    QHBoxLayout* priceLayout = new QHBoxLayout(priceGroup);
    priceLayout->addWidget(new QLabel("股票代码:", this));
    m_priceTsCodeInput = new QLineEdit(this);
    m_priceTsCodeInput->setPlaceholderText("如: 000001.SZ");
    priceLayout->addWidget(m_priceTsCodeInput);

    priceLayout->addWidget(new QLabel("开始日期:", this));
    m_startDateEdit = new QDateEdit(this);
    m_startDateEdit->setCalendarPopup(true);
    priceLayout->addWidget(m_startDateEdit);

    priceLayout->addWidget(new QLabel("结束日期:", this));
    m_endDateEdit = new QDateEdit(this);
    m_endDateEdit->setCalendarPopup(true);
    priceLayout->addWidget(m_endDateEdit);

    QPushButton* priceQueryBtn = new QPushButton("查询", this);
    connect(priceQueryBtn, &QPushButton::clicked, this, &QueryWindow::onPriceQuery);
    priceLayout->addWidget(priceQueryBtn);
    priceLayout->addStretch();
    mainLayout->addWidget(priceGroup);

    // ========== 查询结果显示 ==========
    QGroupBox* resultsGroup = new QGroupBox("查询结果", this);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsGroup);

    m_resultsTable = new QTableWidget(this);
    m_resultsTable->setColumnCount(7);
    m_resultsTable->setHorizontalHeaderLabels({"股票代码", "交易日期", "开盘价", "收盘价", "最高价", "最低价", "成交量"});
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);
    resultsLayout->addWidget(m_resultsTable);

    QPushButton* exportBtn = new QPushButton("导出结果", this);
    connect(exportBtn, &QPushButton::clicked, this, &QueryWindow::onExportResults);
    resultsLayout->addWidget(exportBtn);

    mainLayout->addWidget(resultsGroup);
    setCentralWidget(centralWidget);
}

void QueryWindow::onStockQuery() {
    QString keyword = m_stockKeywordInput->text();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "查询失败", "请输入股票关键词");
        return;
    }

    // TODO: 调用 m_stockDAO 进行查询
    // 当前实现：显示模拟结果
    m_resultsTable->setRowCount(2);
    m_resultsTable->setItem(0, 0, new QTableWidgetItem(keyword + ".SH"));
    m_resultsTable->setItem(0, 1, new QTableWidgetItem("示例股票A"));
    m_resultsTable->setItem(1, 0, new QTableWidgetItem(keyword + ".SZ"));
    m_resultsTable->setItem(1, 1, new QTableWidgetItem("示例股票B"));

    statusBar()->showMessage("股票查询完成");
}

void QueryWindow::onPriceQuery() {
    QString tsCode = m_priceTsCodeInput->text();
    if (tsCode.isEmpty()) {
        QMessageBox::warning(this, "查询失败", "请输入股票代码");
        return;
    }

    // TODO: 调用 m_priceDAO 进行查询
    // 当前实现：显示模拟结果
    m_resultsTable->setRowCount(5);
    for (int i = 0; i < 5; ++i) {
        m_resultsTable->setItem(i, 0, new QTableWidgetItem(tsCode));
        m_resultsTable->setItem(i, 1, new QTableWidgetItem(m_startDateEdit->date().addDays(i).toString("yyyy-MM-dd")));
        m_resultsTable->setItem(i, 2, new QTableWidgetItem(QString::number(10.0 + i * 0.5, 'f', 2)));
        m_resultsTable->setItem(i, 3, new QTableWidgetItem(QString::number(10.5 + i * 0.5, 'f', 2)));
        m_resultsTable->setItem(i, 4, new QTableWidgetItem(QString::number(11.0 + i * 0.5, 'f', 2)));
        m_resultsTable->setItem(i, 5, new QTableWidgetItem(QString::number(10.0 + i * 0.3, 'f', 2)));
        m_resultsTable->setItem(i, 6, new QTableWidgetItem(QString::number(1000000 + i * 100000)));
    }

    statusBar()->showMessage("价格查询完成");
}

void QueryWindow::onExportResults() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出结果", QDir::homePath() + "/query_results.csv", "CSV文件(*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // 导出表头
        for (int col = 0; col < m_resultsTable->columnCount(); ++col) {
            out << m_resultsTable->horizontalHeaderItem(col)->text();
            if (col < m_resultsTable->columnCount() - 1) out << ",";
        }
        out << "\n";

        // 导出数据
        for (int row = 0; row < m_resultsTable->rowCount(); ++row) {
            for (int col = 0; col < m_resultsTable->columnCount(); ++col) {
                QTableWidgetItem* item = m_resultsTable->item(row, col);
                if (item) out << item->text();
                if (col < m_resultsTable->columnCount() - 1) out << ",";
            }
            out << "\n";
        }
        file.close();
        QMessageBox::information(this, "导出成功", QString("结果已导出到: %1").arg(fileName));
    }
}