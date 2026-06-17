#pragma once

#include <QObject>
#include <QString>
#include <vector>

namespace stock_gui {

/**
 * @brief 日志适配器 - 将日志信息适配到 Qt GUI
 *
 * 功能:
 * - 接收日志信息
 * - 发送 Qt 信号更新日志文本框
 * - 支持日志缓存和清空
 */
class LogAdapter : public QObject {
    Q_OBJECT

public:
    explicit LogAdapter(QObject* parent = nullptr);
    ~LogAdapter() override;

    // 添加日志
    void AddLog(const QString& log);

    // 清空日志
    void ClearLogs();

    // 获取所有日志
    QString GetAllLogs() const;

signals:
    // Qt 信号（用于更新 GUI）
    void LogAdded(const QString& log);
    void LogsCleared();

private:
    std::vector<QString> logs_;
    size_t maxSize_ = 100;  // 最大日志数量
};

}  // namespace stock_gui