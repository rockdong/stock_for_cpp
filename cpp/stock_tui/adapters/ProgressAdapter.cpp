#include "ProgressAdapter.h"

namespace stock_tui {

ProgressAdapter::ProgressAdapter() = default;

void ProgressAdapter::Update(int total, int completed, int failed, const std::string& status) {
    total_ = total;
    completed_ = completed;
    failed_ = failed;
    status_ = status;
}

std::function<void(int, int, int, const std::string&)> ProgressAdapter::GetCallback() {
    return [this](int total, int completed, int failed, const std::string& status) {
        Update(total, completed, failed, status);
    };
}

} // namespace stock_tui