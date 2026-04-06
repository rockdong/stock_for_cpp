#pragma once

#include <string>

namespace eventbus::events {

struct AppStarted {
    std::string version;
    std::string buildDate;
};

struct AppStopping {
    std::string reason;
};

struct ProgressUpdated {
    int total;
    int completed;
    int failed;
    std::string status;
};

struct StockAnalysisStarted {
    std::string tsCode;
    std::string stockName;
};

struct StockAnalysisCompleted {
    std::string tsCode;
    std::string stockName;
    bool success;
    std::string errorMessage;
};

struct StockAnalysisFailed {
    std::string tsCode;
    std::string stockName;
    std::string error;
};

struct StrategySignalGenerated {
    std::string tsCode;
    std::string strategyName;
    std::string signal;
    double confidence;
    std::string freq;
};

struct DataFetchCompleted {
    std::string tsCode;
    std::string freq;
    int dataCount;
};

struct DataFetchFailed {
    std::string tsCode;
    std::string freq;
    std::string error;
};

}