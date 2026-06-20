/**
 * @file ApplicationManager.cpp
 * @brief Qt UI 应用程序管理器实现 - 界面协调器
 * @author StockLens Team
 * @version 2.0.0
 */

#include "ApplicationManager.h"
#include "MainWindow.h"          // 入口窗口(简化版)
#include "windows/TaskWindow.h"      // 任务管理窗口
#include "windows/ConfigWindow.h"    // 配置窗口
#include "windows/QueryWindow.h"     // 查询窗口
#include "windows/SyncWindow.h"      // 同步窗口
#include "windows/ChartWindow.h"     // 图表窗口(可选)

// 核心库集成
#include "../../config/Config.h"
#include "../../data/database/ConnectionFactory.h"
#include "../../data/database/Connection.h"
#include "../../data/database/ConnectionManager.h"
#include "../../data/database/StockDAO.h"
#include "../../data/database/PriceDAO.h"
#include "../../log/Logger.h"

#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QApplication>      // QApplication 完整定义
#include <QDir>
#include <QDebug>

using namespace data;

ApplicationManager::ApplicationManager(QObject* parent)
    : QObject(parent)
{
}

ApplicationManager::~ApplicationManager() {
    // 保存全局设置
    QSettings settings("StockLens", "StockUI");
    settings.setValue("theme", m_currentTheme);

    // 注意: DAO对象已被传递给QueryWindow和ChartWindow
    // 这些窗口设置了 Qt::WA_DeleteOnClose 属性,会在关闭时自动删除自己及其子对象
    // 因此这里不删除DAO对象,避免双重删除
    
    qDebug() << "ApplicationManager 析构完成";
}

void ApplicationManager::initialize() {
    // ========== 1. 初始化日志系统 ==========
    setenv("LOG_FILE_ENABLED", "false", 1);
    setenv("LOG_CONSOLE_ENABLED", "true", 1);
    setenv("LOG_LEVEL", "INFO", 1);

    logger::init();
    qDebug() << "应用程序管理器初始化";

    // ========== 2. 初始化数据库连接 ==========
    if (!initializeDatabase()) {
        QMessageBox::critical(nullptr, "初始化失败",
            "无法连接到数据库，应用程序将退出");
        return;
    }

    // ========== 3. 加载全局设置(主题) ==========
    QSettings settings("StockLens", "StockUI");
    m_currentTheme = settings.value("theme", "light").toString();

    // ========== 4. 创建所有独立窗口 ==========
    createWindows();

    // ========== 5. 应用全局主题 ==========
    applyTheme(m_currentTheme);

    qDebug() << "应用程序管理器初始化完成";
}

bool ApplicationManager::initializeDatabase() {
    // 初始化配置
    auto& config = config::Config::getInstance();
    QString envPath = QCoreApplication::applicationDirPath() + "/../../../.env";

    QString dbPath;

    // 从配置文件获取数据库路径
    if (config.initialize(envPath.toStdString().c_str())) {
        std::string dbName = config.getDbName();
        if (!dbName.empty()) {
            dbPath = dbName.find('/') != std::string::npos ?
                     QString::fromStdString(dbName) :
                     QString::fromStdString(dbName + ".db");
        }
    }

    // 使用默认路径
    if (dbPath.isEmpty()) {
        QStringList possiblePaths = {
            QCoreApplication::applicationDirPath() + "/../../../stock_db.db",
            QCoreApplication::applicationDirPath() + "/../../../stock.db",
            QDir::currentPath() + "/stock_db.db"
        };

        for (const QString& path : possiblePaths) {
            if (QFile::exists(path)) {
                dbPath = path;
                qDebug() << "找到数据库文件：" << path;
                break;
            }
        }

        if (dbPath.isEmpty()) {
            dbPath = QCoreApplication::applicationDirPath() + "/../../../stock_db.db";
        }
    }

    // 创建数据库连接
    try {
        auto& conn = Connection::getInstance();
        if (!conn.initialize(dbPath.toStdString())) {
            qCritical() << "数据库初始化失败：" << dbPath;
            return false;
        }

        if (!conn.connect()) {
            qCritical() << "数据库连接失败";
            return false;
        }

        ConnectionManager::getInstance().registerConnection(conn);
        qDebug() << "数据库连接成功：" << dbPath;

        // 创建 DAO 对象
        m_stockDAO = new StockDAO();
        m_priceDAO = new PriceDAO();

        return true;

    } catch (const std::exception& e) {
        qCritical() << "数据库初始化异常：" << e.what();
        return false;
    }
}

void ApplicationManager::createWindows() {
    // ========== 1. 创建入口窗口(简化版) ==========
    m_mainWindow = new MainWindow();
    m_mainWindow->setAttribute(Qt::WA_DeleteOnClose);  // 窗口关闭时自动删除
    
    connect(m_mainWindow, &MainWindow::openTaskWindowRequested,
            this, &ApplicationManager::showTaskWindow);
    connect(m_mainWindow, &MainWindow::openConfigWindowRequested,
            this, &ApplicationManager::showConfigWindow);
    connect(m_mainWindow, &MainWindow::openQueryWindowRequested,
            this, &ApplicationManager::showQueryWindow);
    connect(m_mainWindow, &MainWindow::openSyncWindowRequested,
            this, &ApplicationManager::showSyncWindow);
    connect(m_mainWindow, &MainWindow::openChartWindowRequested,
            this, &ApplicationManager::showChartWindow);

    // ========== 2. 创建任务管理窗口 ==========
    m_taskWindow = new TaskWindow();
    m_taskWindow->setAttribute(Qt::WA_DeleteOnClose);

    // ========== 3. 创建配置窗口 ==========
    m_configWindow = new ConfigWindow();
    m_configWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_configWindow, &ConfigWindow::themeChanged,
            this, &ApplicationManager::applyTheme);

    // ========== 4. 创建查询窗口 ==========
    m_queryWindow = new QueryWindow();
    m_queryWindow->setAttribute(Qt::WA_DeleteOnClose);
    m_queryWindow->setDAOs(m_stockDAO, m_priceDAO);  // 共享 DAO 对象

    // ========== 5. 创建数据库同步窗口 ==========
    m_syncWindow = new SyncWindow();
    m_syncWindow->setAttribute(Qt::WA_DeleteOnClose);

    // ========== 6. 创建图表窗口(可选) ==========
    m_chartWindow = new ChartWindow();
    m_chartWindow->setAttribute(Qt::WA_DeleteOnClose);
    m_chartWindow->setDAOs(m_stockDAO, m_priceDAO);  // 共享 DAO 对象

    qDebug() << "所有独立窗口创建完成";
}

void ApplicationManager::showMainWindow() {
    if (m_mainWindow) {
        m_mainWindow->show();
        m_mainWindow->raise();
        m_mainWindow->activateWindow();
        emit windowSwitched("入口窗口");
    }
}

void ApplicationManager::showTaskWindow() {
    if (m_taskWindow) {
        m_taskWindow->show();
        m_taskWindow->raise();
        m_taskWindow->activateWindow();
        emit windowSwitched("任务管理");
    }
}

void ApplicationManager::showConfigWindow() {
    if (m_configWindow) {
        m_configWindow->show();
        m_configWindow->raise();
        m_configWindow->activateWindow();
        emit windowSwitched("配置管理");
    }
}

void ApplicationManager::showQueryWindow() {
    if (m_queryWindow) {
        m_queryWindow->show();
        m_queryWindow->raise();
        m_queryWindow->activateWindow();
        emit windowSwitched("数据查询");
    }
}

void ApplicationManager::showSyncWindow() {
    if (m_syncWindow) {
        m_syncWindow->show();
        m_syncWindow->raise();
        m_syncWindow->activateWindow();
        emit windowSwitched("数据库同步");
    }
}

void ApplicationManager::showChartWindow() {
    if (m_chartWindow) {
        m_chartWindow->show();
        m_chartWindow->raise();
        m_chartWindow->activateWindow();
        emit windowSwitched("K线图表");
    }
}

void ApplicationManager::closeAllWindows() {
    if (m_mainWindow) m_mainWindow->close();
    if (m_taskWindow) m_taskWindow->close();
    if (m_configWindow) m_configWindow->close();
    if (m_queryWindow) m_queryWindow->close();
    if (m_syncWindow) m_syncWindow->close();
    if (m_chartWindow) m_chartWindow->close();
}

void ApplicationManager::applyTheme(const QString& themeName) {
    m_currentTheme = themeName;

    // 加载 QSS 样式表
    QString qssFileName = QString(":/styles/%1.qss").arg(themeName);
    QFile qssFile(qssFileName);

    if (!qssFile.exists()) {
        QString appDir = QCoreApplication::applicationDirPath();
        qssFileName = QString("%1/../resources/styles/%2.qss").arg(appDir, themeName);
        qssFile.setFileName(qssFileName);
    }

    if (!qssFile.exists()) {
        createQSSFiles();
        qssFile.setFileName(qssFileName);
    }

    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&qssFile);
        QString styleSheet = in.readAll();
        qssFile.close();

        static_cast<QApplication*>(QApplication::instance())->setStyleSheet(styleSheet);
        qDebug() << "应用全局主题：" << themeName;
    } else {
        qWarning() << "无法加载主题样式：" << qssFileName;

        // 使用内置默认样式
        if (themeName == "dark") {
            static_cast<QApplication*>(QApplication::instance())->setStyleSheet(
                "QMainWindow { background-color: #1e1e1e; color: #e0e0e0; }"
                "QWidget { background-color: #2d2d2d; color: #e0e0e0; }"
                "QPushButton { background-color: #444444; color: #e0e0e0; border: 1px solid #555555; padding: 5px 15px; }"
            );
        } else {
            static_cast<QApplication*>(QApplication::instance())->setStyleSheet("");
        }
    }

    // 保存主题设置
    QSettings settings("StockLens", "StockUI");
    settings.setValue("theme", themeName);
}

void ApplicationManager::createQSSFiles() {
    QString appDir = QCoreApplication::applicationDirPath();
    QString resourceDir = QString("%1/../resources/styles").arg(appDir);

    QDir dir(resourceDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 创建浅色主题样式
    QFile lightFile(QString("%1/light.qss").arg(resourceDir));
    if (lightFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&lightFile);
        out << QString(
            "/* 浅色主题样式 */\n"
            "QMainWindow { background-color: #f5f5f5; }\n"
            "QWidget { background-color: #ffffff; color: #333333; }\n"
            "QPushButton { background-color: #0078d4; color: white; border: none; padding: 5px 15px; border-radius: 3px; }\n"
        );
        lightFile.close();
    }

    // 创建深色主题样式
    QFile darkFile(QString("%1/dark.qss").arg(resourceDir));
    if (darkFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&darkFile);
        out << QString(
            "/* 深色主题样式 */\n"
            "QMainWindow { background-color: #1e1e1e; color: #e0e0e0; }\n"
            "QWidget { background-color: #2d2d2d; color: #e0e0e0; }\n"
            "QPushButton { background-color: #444444; color: #e0e0e0; border: 1px solid #555555; padding: 5px 15px; border-radius: 3px; }\n"
        );
        darkFile.close();
    }

    qDebug() << "创建 QSS 样式表文件";
}