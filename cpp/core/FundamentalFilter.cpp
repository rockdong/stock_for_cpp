#include "FundamentalFilter.h"
#include "../log/Logger.h"
#include <algorithm>
#include <cmath>

namespace core {

FundamentalFilter::FundamentalFilter() {}

FundamentalFilter::FundamentalFilter(const FilterThresholds& thresholds)
    : thresholds_(thresholds) {}

void FundamentalFilter::setThresholds(const FilterThresholds& thresholds) {
    thresholds_ = thresholds;
}

FilterThresholds FundamentalFilter::getThresholds() const {
    return thresholds_;
}

FundamentalScore FundamentalFilter::calculateScore(const FinancialIndicator& indicator) {
    FundamentalScore score;
    score.ts_code = indicator.ts_code;
    
    score.valuation = calculateValuationScore(indicator);
    score.quality = calculateQualityScore(indicator);
    score.growth = calculateGrowthScore(indicator);
    score.health = calculateHealthScore(indicator);
    
    score.total = score.valuation + score.quality + score.growth + score.health;
    score.grade = determineGrade(score.total);
    score.is_qualified = isQualified(score);
    
    return score;
}

double FundamentalFilter::calculateValuationScore(const FinancialIndicator& indicator) {
    double score = 0.0;
    
    if (indicator.pe > 0 && indicator.pe < thresholds_.pe_max) {
        score += 10.0;
    }
    
    if (indicator.pb > 0 && indicator.pb < thresholds_.pb_max) {
        score += 8.0;
    }
    
    double growth = indicator.netprofit_yoy > 0 ? indicator.netprofit_yoy : 1.0;
    double peg = indicator.pe / growth;
    if (peg > 0 && peg < thresholds_.peg_max) {
        score += 7.0;
    }
    
    return score;
}

double FundamentalFilter::calculateQualityScore(const FinancialIndicator& indicator) {
    double score = 0.0;
    
    if (indicator.roe >= thresholds_.roe_min) {
        score += 15.0;
        if (indicator.roe >= 20.0) {
            score += 5.0;
        }
    }
    
    if (indicator.grossprofit_margin >= thresholds_.gross_margin_min) {
        score += 10.0;
    }
    
    if (indicator.netprofit_margin > thresholds_.net_margin_min) {
        score += 5.0;
    }
    
    return score;
}

double FundamentalFilter::calculateGrowthScore(const FinancialIndicator& indicator) {
    double score = 0.0;
    
    if (indicator.or_yoy >= thresholds_.revenue_growth_min) {
        score += 10.0;
    }
    
    if (indicator.netprofit_yoy >= thresholds_.profit_growth_min) {
        score += 10.0;
    }
    
    if (indicator.or_yoy > 15.0 && indicator.netprofit_yoy > 15.0) {
        score += 5.0;
    }
    
    return score;
}

double FundamentalFilter::calculateHealthScore(const FinancialIndicator& indicator) {
    double score = 0.0;
    
    if (indicator.debt_to_assets < thresholds_.debt_ratio_max) {
        score += 8.0;
    }
    
    if (indicator.current_ratio >= thresholds_.current_ratio_min) {
        score += 4.0;
    }
    
    if (indicator.ocf_to_or > 0.1) {
        score += 3.0;
    }
    
    return score;
}

std::string FundamentalFilter::determineGrade(double totalScore) const {
    if (totalScore >= 80.0) return "A";
    if (totalScore >= 60.0) return "B";
    if (totalScore >= 40.0) return "C";
    return "D";
}

bool FundamentalFilter::isQualified(const FundamentalScore& score) const {
    return score.total >= thresholds_.total_score_min;
}

std::vector<std::string> FundamentalFilter::filterQualifiedStocks(
    const std::vector<FinancialIndicator>& indicators) {
    
    std::vector<std::string> qualified;
    
    for (const auto& indicator : indicators) {
        FundamentalScore score = calculateScore(indicator);
        if (score.is_qualified) {
            qualified.push_back(indicator.ts_code);
        }
    }
    
    LOG_INFO("基本面筛选完成，优质股票: " + std::to_string(qualified.size()) + " / " + std::to_string(indicators.size()));
    
    return qualified;
}

std::vector<FundamentalScore> FundamentalFilter::filterWithScores(
    const std::vector<FinancialIndicator>& indicators) {
    
    std::vector<FundamentalScore> results;
    
    for (const auto& indicator : indicators) {
        FundamentalScore score = calculateScore(indicator);
        if (score.is_qualified) {
            results.push_back(score);
        }
    }
    
    std::sort(results.begin(), results.end(), 
        [](const FundamentalScore& a, const FundamentalScore& b) {
            return a.total > b.total;
        });
    
    return results;
}

} // namespace core