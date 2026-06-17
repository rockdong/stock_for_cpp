#pragma once

#include <QObject>
#include <functional>

namespace stock_gui {

/**
 * @brief 进度适配器 - 将 stock_core 进度回调适配到 Qt GUI
 *
 * 功能:
 * - 接收 stock_core 进度更新
 * - 发送 Qt 信号更新进度条和统计信息
 */
class ProgressAdapter : public QObject {
    Q_OBJECT

public:
    explicit ProgressAdapter(QObject* parent = nullptr);
    ~ProgressAdapter() override;

    // 获取进度回调函数（供 stock_core 注册）
    std::function<void(int total, int completed, int failed, const std::string& status)>
    GetCallback();

signals:
    // Qt 信号（用于更新 GUI）
    void ProgressUpdated(int total, int completed, int failed, const QString& status);

private:
    int total_;
    int completed_;
    int failed_;
    QString status_;
};

}  // namespace stock_gui