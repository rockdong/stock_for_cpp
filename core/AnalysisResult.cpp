#include "AnalysisResult.h"
#include <sstream>

namespace core {

std::string AnalysisResult::toString() const {
    std::ostringstream oss;
    oss << "AnalysisResult{"
        << "ts_code=" << ts_code
        << ", strategy=" << strategy_name
        << ", date=" << trade_date
        << ", label=" << label
        << "}";
    return oss.str();
}

} // namespace core

