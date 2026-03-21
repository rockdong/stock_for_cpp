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
        << ", opt=" << opt
        << ", freq=" << freq
        << ", strength=" << strengthToString(strength)
        << ", confidence=" << confidence
        << ", risk_warning=" << risk_warning
        << "}";
    return oss.str();
}

} // namespace core

