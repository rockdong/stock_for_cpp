#include "stock_core.h"
#include "StockCoreContext.h"
#include <stdexcept>

namespace stock_core {

// ========== 全局接口实现 ==========

bool initialize(const std::string& configPath) {
    try {
        auto& context = StockCoreContext::getInstance();
        return context.initialize(configPath);
    } catch (const std::exception& e) {
        // 初始化失败,输出到 stderr (因为日志可能未初始化)
        std::cerr << "核心库初始化异常: " << e.what() << std::endl;
        return false;
    }
}

void shutdown() {
    try {
        auto& context = StockCoreContext::getInstance();
        context.shutdown();
    } catch (const std::exception& e) {
        std::cerr << "核心库清理异常: " << e.what() << std::endl;
    }
}

StockCoreContext* getContext() {
    auto& context = StockCoreContext::getInstance();
    return context.isInitialized() ? &context : nullptr;
}

// ========== API 实现 ==========

namespace api {

config::Config& getConfig() {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    return context->getConfig();
}

data::Connection& getDatabase() {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    return context->getDatabase();
}

std::shared_ptr<network::IDataSource> getDataSource() {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    return context->getDataSource();
}

core::StrategyManager& getStrategyManager() {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    return context->getStrategyManager();
}

scheduler::Scheduler& getScheduler() {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    return context->getScheduler();
}

std::vector<core::Stock> loadStockList() {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    return context->loadStockList();
}

void performDailyAnalysis(const std::vector<core::Stock>& stocks) {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    context->performDailyAnalysis(stocks);
}

void runScheduler(const std::string& executeTime) {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    context->runScheduler(executeTime);
}

void setProgressCallback(ProgressCallback callback) {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    context->setProgressCallback(callback);
}

int importStocksFromCSV(const std::string& csvFilePath) {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化,请先调用 stock_core::initialize()");
    }
    return context->importStocksFromCSV(csvFilePath);
}

} // namespace api

} // namespace stock_core