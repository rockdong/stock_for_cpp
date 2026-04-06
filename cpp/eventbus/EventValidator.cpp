#include "EventValidator.h"

namespace eventbus {

bool EventValidator::validateProgress(const events::ProgressUpdated& event) {
    if (event.total < 0 || event.completed < 0 || event.failed < 0) {
        return false;
    }
    
    if (event.completed + event.failed > event.total) {
        return false;
    }
    
    if (event.status != "running" && 
        event.status != "completed" && 
        event.status != "failed") {
        return false;
    }
    
    return true;
}

bool EventValidator::validateStockCode(const std::string& tsCode) {
    if (tsCode.size() != 9) {
        return false;
    }
    
    size_t dotPos = tsCode.find('.');
    if (dotPos == std::string::npos) {
        return false;
    }
    
    std::string market = tsCode.substr(dotPos + 1);
    if (market != "SZ" && market != "SH") {
        return false;
    }
    
    return true;
}

bool EventValidator::validateSignal(const std::string& signal) {
    return signal == "buy" || signal == "sell" || signal == "hold";
}

bool EventValidator::validateConfidence(double confidence) {
    return confidence >= 0.0 && confidence <= 1.0;
}

}