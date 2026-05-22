#ifndef CORE_STRATEGY_EMOJI_MAPPER_H
#define CORE_STRATEGY_EMOJI_MAPPER_H

#include <string>
#include <map>

namespace core {

/**
 * @brief 策略 Emoji 映射器
 * 
 * 提供策略名称与 emoji 的双向映射
 */
class StrategyEmojiMapper {
public:
    /**
     * @brief 初始化映射表
     */
    static void initialize();
    
    /**
     * @brief 策略名称 → Emoji
     * @param strategyName 策略名称
     * @return Emoji 字符串，未注册则返回原名称
     */
    static std::string getEmoji(const std::string& strategyName);
    
    /**
     * @brief 检查策略是否已注册 emoji
     */
    static bool isRegistered(const std::string& strategyName);
    
    /**
     * @brief 注册策略的 emoji（用于扩展）
     */
    static void registerEmoji(const std::string& strategyName, const std::string& emoji);
    
    /**
     * @brief 获取所有映射
     */
    static const std::map<std::string, std::string>& getAllMappings();

private:
    static std::map<std::string, std::string> emojiMap_;
    static bool initialized_;
};

} // namespace core

#endif // CORE_STRATEGY_EMOJI_MAPPER_H