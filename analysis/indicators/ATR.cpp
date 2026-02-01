#include "ATR.h"
#include <stdexcept>
#include <cstring>

namespace analysis {

ATR::ATR(int period)
    : period_(period) {
    parameters_["period"] = period;
}

std::shared_ptr<IndicatorResult> ATR::calculate(
    const std::vector<double>& high,
    const std::vector<double>& low,
    const std::vector<double>& close
) {
    validateDataLength(high, low, close);
    validateData(close, period_);
    
    auto result = std::make_shared<IndicatorResult>("ATR");
    
    int size = static_cast<int>(close.size());
    std::vector<double> output(size);
    
    int outBegIdx = 0;
    int outNbElement = 0;
    
    // 调用 TA-Lib 函数
    TA_RetCode retCode = TA_ATR(
        0,                          // startIdx
        size - 1,                   // endIdx
        high.data(),                // inHigh
        low.data(),                 // inLow
        close.data(),               // inClose
        period_,                    // optInTimePeriod
        &outBegIdx,                 // outBegIdx
        &outNbElement,              // outNBElement
        output.data()               // outReal
    );
    
    if (retCode != TA_SUCCESS) {
        throw std::runtime_error("ATR 计算失败: TA-Lib 错误码 " + std::to_string(retCode));
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

std::shared_ptr<IndicatorResult> ATR::calculate(const std::vector<double>& prices) {
    // 当只有收盘价时，使用收盘价代替高低价
    return calculate(prices, prices, prices);
}

std::vector<double> ATR::compute(
    const std::vector<double>& high,
    const std::vector<double>& low,
    const std::vector<double>& close,
    int period
) {
    ATR indicator(period);
    auto result = indicator.calculate(high, low, close);
    return result->values;
}

} // namespace analysis

