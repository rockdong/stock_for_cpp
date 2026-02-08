#include "Strategy.h"

namespace core {

// StrategyBase implementation
StrategyBase::StrategyBase(const std::string& name, const std::string& description)
    : name_(name)
    , description_(description)
{
}

void StrategyBase::setParameters(const std::map<std::string, double>& params) {
    parameters_ = params;
}

std::map<std::string, double> StrategyBase::getParameters() const {
    return parameters_;
}

bool StrategyBase::validateParameters() const {
    // 基类默认验证通过，子类可以重写
    return true;
}

double StrategyBase::getParameter(const std::string& key, double defaultValue) const {
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second;
    }
    return defaultValue;
}

void StrategyBase::setParameter(const std::string& key, double value) {
    parameters_[key] = value;
}

bool StrategyBase::hasEnoughData(const std::vector<StockData>& data, size_t minSize) const {
    return data.size() >= minSize;
}

AnalysisResult StrategyBase::createResult(
    const std::string& tsCode,
    const std::string& tradeDate,
    const std::string& label,
    const std::string& opt,
    const std::string& freq
) const {
    return AnalysisResult(tsCode, name_, tradeDate, label, opt, freq);
}

} // namespace core

