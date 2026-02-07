#include "StrategyManager.h"
#include "StrategyFactory.h"
#include "../log/Logger.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace core {

StrategyManager& StrategyManager::getInstance() {
    static StrategyManager instance;
    return instance;
}

bool StrategyManager::initialize(const std::string& strategies_config) {
    if (strategies_config.empty()) {
        LOG_WARN("策略配置为空");
        return false;
    }

    // 清除现有策略
    clear();

    // 解析配置
    auto strategyNames = parseConfig(strategies_config);

    if (strategyNames.empty()) {
        LOG_WARN("未找到有效的策略配置");
        return false;
    }

    // 创建并注册策略
    int successCount = 0;
    for (const auto& name : strategyNames) {
        auto strategy = createStrategy(name);
        if (strategy) {
            if (registerStrategy(strategy)) {
                successCount++;
                LOG_INFO("注册策略成功: " + strategy->getName() + " (" + strategy->getDescription() + ")");
            }
        } else {
            LOG_WARN("无法创建策略: " + name);
        }
    }

    LOG_INFO("策略管理器初始化完成: 成功注册 " + std::to_string(successCount) + "/" + std::to_string(strategyNames.size()) + " 个策略");
    return successCount > 0;
}

bool StrategyManager::initializeFromEnv(const std::string& env_key) {
    const char* config = std::getenv(env_key.c_str());
    
    if (!config) {
        LOG_WARN("环境变量 " + env_key + " 未设置");
        return false;
    }

    LOG_INFO("从环境变量 " + env_key + " 加载策略配置: " + std::string(config));
    return initialize(std::string(config));
}

std::vector<StrategyPtr> StrategyManager::getStrategies() const {
    std::vector<StrategyPtr> result;
    result.reserve(strategies_.size());
    
    for (const auto& pair : strategies_) {
        result.push_back(pair.second);
    }
    
    return result;
}

StrategyPtr StrategyManager::getStrategy(const std::string& name) const {
    std::string normalizedName = normalizeName(name);
    auto it = strategies_.find(normalizedName);
    
    if (it != strategies_.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::vector<std::string> StrategyManager::getStrategyNames() const {
    std::vector<std::string> names;
    names.reserve(strategies_.size());
    
    for (const auto& pair : strategies_) {
        names.push_back(pair.second->getName());
    }
    
    return names;
}

bool StrategyManager::hasStrategy(const std::string& name) const {
    std::string normalizedName = normalizeName(name);
    return strategies_.find(normalizedName) != strategies_.end();
}

bool StrategyManager::registerStrategy(StrategyPtr strategy) {
    if (!strategy) {
        LOG_ERROR("无法注册空策略");
        return false;
    }

    std::string normalizedName = normalizeName(strategy->getName());
    
    if (hasStrategy(normalizedName)) {
        LOG_WARN("策略已存在: " + strategy->getName());
        return false;
    }

    strategies_[normalizedName] = strategy;
    return true;
}

void StrategyManager::clear() {
    strategies_.clear();
}

size_t StrategyManager::count() const {
    return strategies_.size();
}

std::map<std::string, std::optional<AnalysisResult>> StrategyManager::analyzeAll(
    const std::string& tsCode,
    const std::vector<StockData>& data
) const {
    std::map<std::string, std::optional<AnalysisResult>> results;
    
    for (const auto& pair : strategies_) {
        const auto& strategy = pair.second;
        auto result = strategy->analyze(tsCode, data);
        results[strategy->getName()] = result;
    }
    
    return results;
}

std::vector<std::string> StrategyManager::parseConfig(const std::string& config) const {
    std::vector<std::string> names;
    std::stringstream ss(config);
    std::string name;
    
    // 使用分号分隔
    while (std::getline(ss, name, ';')) {
        // 去除首尾空格
        name.erase(0, name.find_first_not_of(" \t\r\n"));
        name.erase(name.find_last_not_of(" \t\r\n") + 1);
        
        if (!name.empty()) {
            names.push_back(name);
        }
    }
    
    return names;
}

StrategyPtr StrategyManager::createStrategy(const std::string& name) const {
    std::string normalizedName = normalizeName(name);
    
    // 使用 StrategyFactory 创建策略
    try {
        return StrategyFactory::create(normalizedName);
    } catch (const std::exception& e) {
        LOG_ERROR("创建策略失败: " + name + ", 错误: " + std::string(e.what()));
        return nullptr;
    }
}

std::string StrategyManager::normalizeName(const std::string& name) const {
    std::string normalized = name;
    
    // 转换为大写
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    // 移除空格
    normalized.erase(std::remove_if(normalized.begin(), normalized.end(),
                                    [](unsigned char c) { return std::isspace(c); }),
                     normalized.end());
    
    return normalized;
}

} // namespace core

