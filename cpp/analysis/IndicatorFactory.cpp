#include "IndicatorFactory.h"
#include "indicators/MA.h"
#include "indicators/EMA.h"
#include "indicators/MACD.h"
#include "indicators/RSI.h"
#include "indicators/KDJ.h"
#include "indicators/BOLL.h"
#include "indicators/ATR.h"
#include <stdexcept>
#include <algorithm>

namespace analysis {

IndicatorPtr IndicatorFactory::create(IndicatorType type, const std::map<std::string, double>& params) {
    IndicatorPtr indicator;
    
    switch (type) {
        case IndicatorType::MA: {
            int period = 20;
            TA_MAType maType = TA_MAType_SMA;
            
            auto it = params.find("period");
            if (it != params.end()) {
                period = static_cast<int>(it->second);
            }
            
            it = params.find("maType");
            if (it != params.end()) {
                maType = static_cast<TA_MAType>(static_cast<int>(it->second));
            }
            
            indicator = std::make_shared<MA>(period, maType);
            break;
        }
        
        case IndicatorType::EMA: {
            int period = 12;
            
            auto it = params.find("period");
            if (it != params.end()) {
                period = static_cast<int>(it->second);
            }
            
            indicator = std::make_shared<EMA>(period);
            break;
        }
        
        case IndicatorType::MACD: {
            int fastPeriod = 12;
            int slowPeriod = 26;
            int signalPeriod = 9;
            
            auto it = params.find("fastPeriod");
            if (it != params.end()) {
                fastPeriod = static_cast<int>(it->second);
            }
            
            it = params.find("slowPeriod");
            if (it != params.end()) {
                slowPeriod = static_cast<int>(it->second);
            }
            
            it = params.find("signalPeriod");
            if (it != params.end()) {
                signalPeriod = static_cast<int>(it->second);
            }
            
            indicator = std::make_shared<MACD>(fastPeriod, slowPeriod, signalPeriod);
            break;
        }
        
        case IndicatorType::RSI: {
            int period = 14;
            
            auto it = params.find("period");
            if (it != params.end()) {
                period = static_cast<int>(it->second);
            }
            
            indicator = std::make_shared<RSI>(period);
            break;
        }
        
        case IndicatorType::KDJ: {
            int fastK_Period = 9;
            int slowK_Period = 3;
            int slowD_Period = 3;
            
            auto it = params.find("fastK_Period");
            if (it != params.end()) {
                fastK_Period = static_cast<int>(it->second);
            }
            
            it = params.find("slowK_Period");
            if (it != params.end()) {
                slowK_Period = static_cast<int>(it->second);
            }
            
            it = params.find("slowD_Period");
            if (it != params.end()) {
                slowD_Period = static_cast<int>(it->second);
            }
            
            indicator = std::make_shared<KDJ>(fastK_Period, slowK_Period, slowD_Period);
            break;
        }
        
        case IndicatorType::BOLL: {
            int period = 20;
            double nbDevUp = 2.0;
            double nbDevDn = 2.0;
            
            auto it = params.find("period");
            if (it != params.end()) {
                period = static_cast<int>(it->second);
            }
            
            it = params.find("nbDevUp");
            if (it != params.end()) {
                nbDevUp = it->second;
            }
            
            it = params.find("nbDevDn");
            if (it != params.end()) {
                nbDevDn = it->second;
            }
            
            indicator = std::make_shared<BOLL>(period, nbDevUp, nbDevDn);
            break;
        }
        
        case IndicatorType::ATR: {
            int period = 14;
            
            auto it = params.find("period");
            if (it != params.end()) {
                period = static_cast<int>(it->second);
            }
            
            indicator = std::make_shared<ATR>(period);
            break;
        }
        
        default:
            throw std::invalid_argument("不支持的指标类型");
    }
    
    return indicator;
}

IndicatorPtr IndicatorFactory::create(const std::string& name, const std::map<std::string, double>& params) {
    IndicatorType type = stringToType(name);
    return create(type, params);
}

std::vector<std::string> IndicatorFactory::getSupportedIndicators() {
    return {"MA", "EMA", "MACD", "RSI", "KDJ", "BOLL", "ATR"};
}

IndicatorFactory::IndicatorType IndicatorFactory::stringToType(const std::string& name) {
    std::string upperName = name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);
    
    if (upperName == "MA") return IndicatorType::MA;
    if (upperName == "EMA") return IndicatorType::EMA;
    if (upperName == "MACD") return IndicatorType::MACD;
    if (upperName == "RSI") return IndicatorType::RSI;
    if (upperName == "KDJ") return IndicatorType::KDJ;
    if (upperName == "BOLL") return IndicatorType::BOLL;
    if (upperName == "ATR") return IndicatorType::ATR;
    
    throw std::invalid_argument("不支持的指标名称: " + name);
}

} // namespace analysis

