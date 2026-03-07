#include "BOLL.h"
#include <stdexcept>
#include <cstring>

namespace analysis {

BOLL::BOLL(int period, double nbDevUp, double nbDevDn)
    : period_(period), nbDevUp_(nbDevUp), nbDevDn_(nbDevDn) {
    parameters_["period"] = period;
    parameters_["nbDevUp"] = nbDevUp;
    parameters_["nbDevDn"] = nbDevDn;
}

std::shared_ptr<IndicatorResult> BOLL::calculate(const std::vector<double>& prices) {
    validateData(prices, period_);
    
    auto result = std::make_shared<BOLLResult>();
    
    int size = static_cast<int>(prices.size());
    std::vector<double> upperOut(size);
    std::vector<double> middleOut(size);
    std::vector<double> lowerOut(size);
    
    int outBegIdx = 0;
    int outNbElement = 0;
    
    // 调用 TA-Lib 函数
    TA_RetCode retCode = TA_BBANDS(
        0,                          // startIdx
        size - 1,                   // endIdx
        prices.data(),              // inReal
        period_,                    // optInTimePeriod
        nbDevUp_,                   // optInNbDevUp
        nbDevDn_,                   // optInNbDevDn
        TA_MAType_SMA,              // optInMAType
        &outBegIdx,                 // outBegIdx
        &outNbElement,              // outNBElement
        upperOut.data(),            // outRealUpperBand
        middleOut.data(),           // outRealMiddleBand
        lowerOut.data()             // outRealLowerBand
    );
    
    if (retCode != TA_SUCCESS) {
        throw std::runtime_error("BOLL 计算失败: TA-Lib 错误码 " + std::to_string(retCode));
    }
    
    // 调整输出数组大小
    result->upper.resize(size, 0.0);
    result->middle.resize(size, 0.0);
    result->lower.resize(size, 0.0);
    
    // 复制有效数据
    std::memcpy(
        result->upper.data() + outBegIdx,
        upperOut.data(),
        outNbElement * sizeof(double)
    );
    std::memcpy(
        result->middle.data() + outBegIdx,
        middleOut.data(),
        outNbElement * sizeof(double)
    );
    std::memcpy(
        result->lower.data() + outBegIdx,
        lowerOut.data(),
        outNbElement * sizeof(double)
    );
    
    // 设置主要值为中轨
    result->values = result->middle;
    
    // 添加到 series
    result->series["upper"] = result->upper;
    result->series["middle"] = result->middle;
    result->series["lower"] = result->lower;
    
    return result;
}

std::vector<double> BOLL::compute(
    const std::vector<double>& prices,
    int period,
    double nbDevUp,
    double nbDevDn
) {
    BOLL indicator(period, nbDevUp, nbDevDn);
    auto result = indicator.calculate(prices);
    auto bollResult = std::static_pointer_cast<BOLLResult>(result);
    return bollResult->middle;
}

} // namespace analysis

