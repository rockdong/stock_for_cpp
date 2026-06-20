/**
 * @file main.cpp
 * @brief Qt UI 应用入口 - 使用 ApplicationManager (重构后)
 * @author StockLens Team
 * @version 2.0.0
 */

#include <QApplication>
#include "ApplicationManager.h"

#ifdef __has_include
#  if __has_include("../../version.h")
#    include "../../version.h"
#  endif
#endif

int main(int argc, char* argv[]) {
    // 高 DPI 支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    // 设置应用信息
    app.setApplicationName("Stock for C++ UI");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("StockLens");

#ifdef VERSION_STRING
    app.setApplicationVersion(QString::fromStdString(VERSION_STRING));
#endif

    // ========== 创建应用程序管理器 (重构后核心改变) ==========

    ApplicationManager appManager;

    // 初始化: 数据库连接、窗口创建、主题加载
    appManager.initialize();

    // 显示入口窗口
    appManager.showMainWindow();

    return app.exec();
}