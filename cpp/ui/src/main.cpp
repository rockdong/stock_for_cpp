/**
 * @file main.cpp
 * @brief Qt UI 应用入口
 * @author StockLens Team
 * @version 1.0.0
 */

#include <QApplication>
#include <QFile>
#include <QIcon>
#include "MainWindow.h"

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
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("StockLens");

#ifdef VERSION_STRING
    app.setApplicationVersion(QString::fromStdString(VERSION_STRING));
#endif

    // 加载样式表（如果存在）
    QFile styleFile(":/styles/main.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    // 创建主窗口
    MainWindow mainWindow;
    mainWindow.setWindowTitle("Stock for C++ - Qt UI");
    mainWindow.resize(1200, 800);
    mainWindow.show();

    return app.exec();
}