#include "LogAdapter.h"

namespace stock_gui {

LogAdapter::LogAdapter(QObject* parent)
    : QObject(parent)
    , logs_()
    , maxSize_(100)
{
}

LogAdapter::~LogAdapter() = default;

void LogAdapter::AddLog(const QString& log) {
    logs_.push_back(log);

    // 超过最大数量，删除旧日志
    if (logs_.size() > maxSize_) {
        logs_.erase(logs_.begin());
    }

    // 发送 Qt 信号
    emit LogAdded(log);
}

void LogAdapter::ClearLogs() {
    logs_.clear();
    emit LogsCleared();
}

QString LogAdapter::GetAllLogs() const {
    QString result;
    for (const auto& log : logs_) {
        result += log + "\n";
    }
    return result;
}

}  // namespace stock_gui