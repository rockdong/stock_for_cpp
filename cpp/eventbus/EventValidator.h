#pragma once

#include "events/SystemEvents.h"
#include <string>

namespace eventbus {

class EventValidator {
public:
    static bool validateProgress(const events::ProgressUpdated& event);
    static bool validateStockCode(const std::string& tsCode);
    static bool validateSignal(const std::string& signal);
    static bool validateConfidence(double confidence);
};

}