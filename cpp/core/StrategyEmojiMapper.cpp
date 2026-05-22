#include "StrategyEmojiMapper.h"
#include "../log/Logger.h"

namespace core {

std::map<std::string, std::string> StrategyEmojiMapper::emojiMap_;
bool StrategyEmojiMapper::initialized_ = false;

void StrategyEmojiMapper::initialize() {
    if (initialized_) return;
    
    // 默认映射表
    emojiMap_ = {
        {"EMA17TO25Strategy", "🔗"},
        {"EMA25Greater17PriceMatchStrategy", "⬆️"},
        {"EMA17BreakoutStrategy", "🚀"},
        {"EMA25CrossoverStrategy", "🔄"},
        {"EMAConvergenceStrategy", "🎯"},
        {"MACrossStrategy", "📈"},
        {"MACDStrategy", "📊"},
        {"RSIStrategy", "🎢"},
        {"BOLLStrategy", "🎪"},
        {"GridStrategy", "🧱"},
        {"SurgeSignalStrategy", "💥"},
        {"FundamentalStrategy", "💎"}
    };
    
    initialized_ = true;
    
    LOG_DEBUG("StrategyEmojiMapper 初始化完成，已注册 " + 
              std::to_string(emojiMap_.size()) + " 个策略 emoji");
}

std::string StrategyEmojiMapper::getEmoji(const std::string& strategyName) {
    if (!initialized_) initialize();
    
    auto it = emojiMap_.find(strategyName);
    if (it != emojiMap_.end()) {
        return it->second;
    }
    
    // 未注册则返回原名称
    return strategyName;
}

bool StrategyEmojiMapper::isRegistered(const std::string& strategyName) {
    if (!initialized_) initialize();
    return emojiMap_.find(strategyName) != emojiMap_.end();
}

void StrategyEmojiMapper::registerEmoji(const std::string& strategyName, const std::string& emoji) {
    if (!initialized_) initialize();
    emojiMap_[strategyName] = emoji;
}

const std::map<std::string, std::string>& StrategyEmojiMapper::getAllMappings() {
    if (!initialized_) initialize();
    return emojiMap_;
}

} // namespace core