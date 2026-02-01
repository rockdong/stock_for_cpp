#include "MACD.h"
#include <stdexcept>
#include <cstring>

namespace analysis {

MACD::MACD(int fastPeriod, int slowPeriod, int signalPeriod)
    : fastPeriod_(fastPeriod), slowPeriod_(slowPeriod), signalPeriod_(signalPeriod) {
    parameters_["fastPeriod"] = fastPeriod;
    parameters_["slowPeriod"] = slowPeriod;
    parameters_["signalPeriod"] = signalPeriod;
}

std::shared_ptr<IndicatorResult> MACD::calculate(const std::vector<double>& prices) {
    int minSize = slowPeriod_ + signalPeriod_ - 1;
    validateData(prices, minSize);
    
    auto result = std::make_shared<MACDResult>();
    
    int size = static_cast<int>(prices.size());
    std::vector<double> macdOut(size);
    std::vector<double> signalOut(size);
    std::vector<double> histOut(size);
    
    int outBegIdx = 0;
    int outNbElement = 0;
    
    // 调用 TA-Lib 函数
    TA_RetCode retCode = TA_MACD(
        0,                          // startIdx
        size - 1,                   // endIdx
        prices.data(),              // inReal
        fastPeriod_,                // optInFastPeriod
        slowPeriod_,                // optInSlowPeriod
        signalPeriod_,              // optInSignalPeriod
        &outBegIdx,                 // outBegIdx
        &outNbElement,              // outNBElement
        macdOut.data(),             // outMACD
        signalOut.data(),           // outMACDSignal
        histOut.data()              // outMACDHist
    );
    
    if (retCode != TA_SUCCESS) {
        throw std::runtime_error("MACD 计算失败: TA-Lib 错误码 " + std::to_string(retCode));
    }
    
    // 调整输出数组大小
    result->macd.resize(size, 0.0);
    result->signal.resize(size, 0.0);
    result->histogram.resize(size, 0.0);
    
    // 复制有效数据
    std::memcpy(
        result->macd.data() + outBegIdx,
        macdOut.data(),
        outNbElement * sizeof(double)
    );
    std::memcpy(
        result->signal.data() + outBegIdx,
        signalOut.data(),
        outNbElement * sizeof(double)
    );
    std::memcpy(
        result->histogram.data() + outBegIdx,
        histOut.data(),
        outNbElement * sizeof(double)
    );
    
    // 设置主要值为 MACD 线
    result->values = result->macd;
    
    // 添加到 series
    result->series["macd"] = result->macd;
    result->series["signal"] = result->signal;
    result->series["histogram"] = result->histogram;
    
    return result;
}

std::vector<double> MACD::compute(
    const std::vector<double>& prices,
    int fastPeriod,
    int slowPeriod,
    int signalPeriod
) {
    MACD indicator(fastPeriod, slowPeriod, signalPeriod);
    auto result = indicator.calculate(prices);
    auto macdResult = std::static_pointer_cast<MACDResult>(result);
    return macdResult->macd;
}

} // namespace analysis

