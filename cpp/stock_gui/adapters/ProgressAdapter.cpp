#include "ProgressAdapter.h"

namespace stock_gui {

ProgressAdapter::ProgressAdapter(QObject* parent)
    : QObject(parent)
    , total_(0)
    , completed_(0)
    , failed_(0)
    , status_("就绪")
{
}

ProgressAdapter::~ProgressAdapter() = default;

std::function<void(int total, int completed, int failed, const std::string& status)>
ProgressAdapter::GetCallback() {
    return [this](int total, int completed, int failed, const std::string& status) {
        total_ = total;
        completed_ = completed;
        failed_ = failed;
        status_ = QString::fromStdString(status);

        // 发送 Qt 信号
        emit ProgressUpdated(total_, completed_, failed_, status_);
    };
}

}  // namespace stock_gui