#include "MA.h"
#include <stdexcept>
#include <cstring>

namespace analysis {

MA::MA(int period, TA_MAType maType)
    : period_(period), maType_(maType) {
    parameters_["period"] = period;
    parameters_["maType"] = static_cast<double>(maType);
}

std::shared_ptr<IndicatorResult> MA::calculate(const std::vector<double>& prices) {
    validateData(prices, period_);
    
    auto result = std::make_shared<IndicatorResult>("MA");
    
    int size = static_cast<int>(prices.size());
    std::vector<double> output(size);
    
    int outBegIdx = 0;
    int outNbElement = 0;
    
    // 调用 TA-Lib 函数
    TA_RetCode retCode = TA_MA(
        0,                          // startIdx
        size - 1,                   // endIdx
        prices.data(),              // inReal
        period_,                    // optInTimePeriod
        maType_,                    // optInMAType
        &outBegIdx,                 // outBegIdx
        &outNbElement,              // outNBElement
        output.data()               // outReal
    );
    
    if (retCode != TA_SUCCESS) {
        throw std::runtime_error("MA 计算失败: TA-Lib 错误码 " + std::to_string(retCode));
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

std::vector<double> MA::compute(
    const std::vector<double>& prices,
    int period,
    TA_MAType maType
) {
    MA indicator(period, maType);
    auto result = indicator.calculate(prices);
    return result->values;
}

} // namespace analysis

