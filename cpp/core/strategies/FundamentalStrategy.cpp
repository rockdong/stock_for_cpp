#include "FundamentalStrategy.h"
#include "../network/DataSourceFactory.h"
#include "../utils/TimeUtil.h"
#include "../log/Logger.h"

namespace core {

FundamentalStrategy::FundamentalStrategy(const std::map<std::string, double>& params)
    : StrategyBase("FundamentalStrategy", "基本面策略 - 估值/盈利/增长/健康评分")
{
    setParameter("strong_buy_threshold", 80.0);
    setParameter("medium_buy_threshold", 60.0);
    setParameter("weak_buy_threshold", 40.0);
    setParameter("sell_threshold", 40.0);
    
    setParameters(params);
    
    // 更新阈值
    strongBuyThreshold_ = getParameter("strong_buy_threshold", 80.0);
    mediumBuyThreshold_ = getParameter("medium_buy_threshold", 60.0);
    weakBuyThreshold_ = getParameter("weak_buy_threshold", 40.0);
    sellThreshold_ = getParameter("sell_threshold", 40.0);
}

std::optional<AnalysisResult> FundamentalStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    if (data.empty()) {
        return std::nullopt;
    }
    
    // 获取分析日期（使用最新交易日期）
    std::string tradeDate = data.back().trade_date;
    
    // 获取财务指标数据
    auto indicatorOpt = getFinancialIndicator(tsCode, tradeDate);
    if (!indicatorOpt) {
        LOG_WARN("FundamentalStrategy: 无法获取 " + tsCode + " 的财务数据");
        return std::nullopt;
    }
    
    const FinancialIndicator& indicator = *indicatorOpt;
    
    // 使用 FundamentalFilter 计算评分
    FundamentalScore score = filter_.calculateScore(indicator);
    
    // 生成信号
    return generateSignal(tsCode, tradeDate, score.total, score.grade);
}

bool FundamentalStrategy::validateParameters() const {
    return strongBuyThreshold_ >= mediumBuyThreshold_ &&
           mediumBuyThreshold_ >= weakBuyThreshold_ &&
           weakBuyThreshold_ >= sellThreshold_;
}

std::optional<FinancialIndicator> FundamentalStrategy::getFinancialIndicator(
    const std::string& tsCode,
    const std::string& tradeDate
) const {
    try {
        auto dataSource = network::DataSourceFactory::createFromConfig();
        
        // 获取每日指标数据
        auto tushareSource = std::dynamic_pointer_cast<network::TushareDataSource>(dataSource);
        if (!tushareSource) {
            return std::nullopt;
        }
        
        auto response = tushareSource->getClient()->getDailyBasic(tsCode, tradeDate, "", "");
        
        if (!response.isSuccess() || !response.data.contains("items")) {
            return std::nullopt;
        }
        
        auto items = response.data["items"];
        if (items.empty()) {
            return std::nullopt;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        std::map<std::string, size_t> fieldIndex;
        for (size_t i = 0; i < fields.size(); ++i) {
            fieldIndex[fields[i]] = i;
        }
        
        auto get_double = [&](const auto& item, const std::string& fieldName) -> double {
            auto it = fieldIndex.find(fieldName);
            if (it != fieldIndex.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        const auto& item = items[0];
        
        FinancialIndicator indicator;
        indicator.ts_code = tsCode;
        indicator.ann_date = tradeDate;
        indicator.end_date = tradeDate;
        indicator.pe = get_double(item, "pe");
        indicator.pe_ttm = get_double(item, "pe_ttm");
        indicator.pb = get_double(item, "pb");
        
        return indicator;
        
    } catch (const std::exception& e) {
        LOG_ERROR("FundamentalStrategy 获取财务数据失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

AnalysisResult FundamentalStrategy::generateSignal(
    const std::string& tsCode,
    const std::string& tradeDate,
    double totalScore,
    const std::string& grade
) const {
    std::string label;
    std::string opt;
    SignalStrength strength = SignalStrength::WEAK;
    double confidence = totalScore;
    std::string warning;
    
    if (totalScore >= strongBuyThreshold_) {
        label = "买入";
        opt = "buy";
        strength = SignalStrength::STRONG;
        warning = "基本面优秀，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    } else if (totalScore >= mediumBuyThreshold_) {
        label = "买入";
        opt = "buy";
        strength = SignalStrength::MEDIUM;
        warning = "基本面良好，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    } else if (totalScore >= weakBuyThreshold_) {
        label = "买入";
        opt = "buy";
        strength = SignalStrength::WEAK;
        warning = "基本面一般，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    } else {
        label = "卖出";
        opt = "sell";
        warning = "基本面较差，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    }
    
    return createResult(tsCode, tradeDate, label, strength, confidence, warning, opt);
}

} // namespace core