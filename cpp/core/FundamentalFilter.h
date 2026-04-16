#ifndef CORE_FUNDAMENTAL_FILTER_H
#define CORE_FUNDAMENTAL_FILTER_H

#include "FundamentalData.h"
#include <vector>
#include <functional>

namespace core {

class FundamentalFilter {
public:
    FundamentalFilter();
    explicit FundamentalFilter(const FilterThresholds& thresholds);
    
    void setThresholds(const FilterThresholds& thresholds);
    FilterThresholds getThresholds() const;
    
    FundamentalScore calculateScore(const FinancialIndicator& indicator);
    
    std::vector<std::string> filterQualifiedStocks(
        const std::vector<FinancialIndicator>& indicators
    );
    
    std::vector<FundamentalScore> filterWithScores(
        const std::vector<FinancialIndicator>& indicators
    );
    
    bool isQualified(const FundamentalScore& score) const;

private:
    double calculateValuationScore(const FinancialIndicator& indicator);
    double calculateQualityScore(const FinancialIndicator& indicator);
    double calculateGrowthScore(const FinancialIndicator& indicator);
    double calculateHealthScore(const FinancialIndicator& indicator);
    
    std::string determineGrade(double totalScore) const;
    
    FilterThresholds thresholds_;
};

} // namespace core

#endif // CORE_FUNDAMENTAL_FILTER_H