#include "KDJ.h"
#include <stdexcept>
#include <cstring>

namespace analysis {

KDJ::KDJ(int fastK_Period, int slowK_Period, int slowD_Period)
    : fastK_Period_(fastK_Period), slowK_Period_(slowK_Period), slowD_Period_(slowD_Period) {
    parameters_["fastK_Period"] = fastK_Period;
    parameters_["slowK_Period"] = slowK_Period;
    parameters_["slowD_Period"] = slowD_Period;
}

std::shared_ptr<IndicatorResult> KDJ::calculate(
    const std::vector<double>& high,
    const std::vector<double>& low,
    const std::vector<double>& close
) {
    validateDataLength(high, low, close);
    validateData(close, fastK_Period_);
    
    auto result = std::make_shared<KDJResult>();
    
    int size = static_cast<int>(close.size());
    std::vector<double> kOut(size);
    std::vector<double> dOut(size);
    
    int outBegIdx = 0;
    int outNbElement = 0;
    
    // 调用 TA-Lib STOCH 函数计算 K 和 D
    TA_RetCode retCode = TA_STOCH(
        0,                          // startIdx
        size - 1,                   // endIdx
        high.data(),                // inHigh
        low.data(),                 // inLow
        close.data(),               // inClose
        fastK_Period_,              // optInFastK_Period
        slowK_Period_,              // optInSlowK_Period
        TA_MAType_SMA,              // optInSlowK_MAType
        slowD_Period_,              // optInSlowD_Period
        TA_MAType_SMA,              // optInSlowD_MAType
        &outBegIdx,                 // outBegIdx
        &outNbElement,              // outNBElement
        kOut.data(),                // outSlowK
        dOut.data()                 // outSlowD
    );
    
    if (retCode != TA_SUCCESS) {
        throw std::runtime_error("KDJ 计算失败: TA-Lib 错误码 " + std::to_string(retCode));
    }
    
    // 调整输出数组大小
    result->k.resize(size, 0.0);
    result->d.resize(size, 0.0);
    result->j.resize(size, 0.0);
    
    // 复制 K 和 D 数据
    std::memcpy(
        result->k.data() + outBegIdx,
        kOut.data(),
        outNbElement * sizeof(double)
    );
    std::memcpy(
        result->d.data() + outBegIdx,
        dOut.data(),
        outNbElement * sizeof(double)
    );
    
    // 计算 J = 3K - 2D
    for (size_t i = outBegIdx; i < static_cast<size_t>(outBegIdx + outNbElement); ++i) {
        result->j[i] = 3.0 * result->k[i] - 2.0 * result->d[i];
    }
    
    // 设置主要值为 K 线
    result->values = result->k;
    
    // 添加到 series
    result->series["k"] = result->k;
    result->series["d"] = result->d;
    result->series["j"] = result->j;
    
    return result;
}

std::shared_ptr<IndicatorResult> KDJ::calculate(const std::vector<double>& prices) {
    // 当只有收盘价时，使用收盘价代替高低价
    return calculate(prices, prices, prices);
}

std::vector<double> KDJ::compute(
    const std::vector<double>& high,
    const std::vector<double>& low,
    const std::vector<double>& close,
    int fastK_Period,
    int slowK_Period,
    int slowD_Period
) {
    KDJ indicator(fastK_Period, slowK_Period, slowD_Period);
    auto result = indicator.calculate(high, low, close);
    auto kdjResult = std::static_pointer_cast<KDJResult>(result);
    return kdjResult->k;
}

} // namespace analysis

