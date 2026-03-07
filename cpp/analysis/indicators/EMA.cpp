#include "EMA.h"
#include <stdexcept>
#include <cstring>

namespace analysis {

EMA::EMA(int period)
    : period_(period) {
    parameters_["period"] = period;
}

std::shared_ptr<IndicatorResult> EMA::calculate(const std::vector<double>& prices) {
    validateData(prices, period_);
    
    auto result = std::make_shared<IndicatorResult>("EMA");
    
    int size = static_cast<int>(prices.size());
    std::vector<double> output(size);
    
    int outBegIdx = 0;
    int outNbElement = 0;
    
    // 调用 TA-Lib 函数
    TA_RetCode retCode = TA_EMA(
        0,                          // startIdx
        size - 1,                   // endIdx
        prices.data(),              // inReal
        period_,                    // optInTimePeriod
        &outBegIdx,                 // outBegIdx
        &outNbElement,              // outNBElement
        output.data()               // outReal
    );
    
    if (retCode != TA_SUCCESS) {
        throw std::runtime_error("EMA 计算失败: TA-Lib 错误码 " + std::to_string(retCode));
    }
    
    // 调整输出数组大小
    result->values.resize(size, 0.0);
    
    // 复制有效数据
    std::memcpy(
        result->values.data() + outBegIdx,
        output.data(),
        outNbElement * sizeof(double)
    );
    
    return result;
}

std::vector<double> EMA::compute(
    const std::vector<double>& prices,
    int period
) {
    EMA indicator(period);
    auto result = indicator.calculate(prices);
    return result->values;
}

} // namespace analysis

