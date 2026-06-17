#include <QApplication>
#include <QMessageBox>
#include <QFile>

// 核心库
#include "../stock_core/stock_core.h"

// GUI 组件
#include "ui/MainWindow.h"

// 版本信息
#include "version.h"

int main(int argc, char* argv[]) {
    try {
        // 1. 初始化 Qt 应用
        QApplication app(argc, argv);
        app.setApplicationName("Stock Analysis System");
        app.setApplicationVersion(VERSION_STRING);
        app.setOrganizationName("StockAnalysis");

        // 2. 设置应用样式（现代化）
        app.setStyle("Fusion");

        // 3. 初始化核心库
        if (!stock_core::initialize(".env")) {
            QMessageBox::critical(nullptr, "初始化失败",
                "无法初始化核心库，请检查配置文件 .env");
            return 1;
        }

        // 4. 创建主窗口
        stock_gui::MainWindow mainWindow;
        mainWindow.setWindowTitle(QString("Stock Analysis System v%1").arg(VERSION_STRING));
        mainWindow.resize(1200, 800);
        mainWindow.show();

        // 5. 运行应用
        int result = app.exec();

        // 6. 清理核心库
        stock_core::shutdown();

        return result;

    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "错误",
            QString("发生异常: %1").arg(e.what()));
        return 1;
    }
}