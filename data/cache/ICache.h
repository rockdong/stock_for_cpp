#ifndef DATA_ICACHE_H
#define DATA_ICACHE_H

#include <string>
#include <optional>
#include <any>

namespace data {

/**
 * @brief 缓存接口
 * 策略模式：定义缓存操作的统一接口
 */
class ICache {
public:
    virtual ~ICache() = default;

    /**
     * @brief 设置缓存
     * @param key 键
     * @param value 值
     * @param ttl 过期时间（秒），0 表示永不过期
     * @return 是否成功
     */
    virtual bool set(const std::string& key, const std::any& value, int ttl = 0) = 0;

    /**
     * @brief 获取缓存
     * @param key 键
     * @return 值（可选）
     */
    virtual std::optional<std::any> get(const std::string& key) = 0;

    /**
     * @brief 删除缓存
     * @param key 键
     * @return 是否成功
     */
    virtual bool remove(const std::string& key) = 0;

    /**
     * @brief 检查键是否存在
     * @param key 键
     * @return 是否存在
     */
    virtual bool exists(const std::string& key) = 0;

    /**
     * @brief 清空所有缓存
     */
    virtual void clear() = 0;

    /**
     * @brief 获取缓存大小
     * @return 缓存项数量
     */
    virtual size_t size() const = 0;

    /**
     * @brief 清理过期缓存
     * @return 清理的数量
     */
    virtual size_t cleanup() = 0;
};

} // namespace data

#endif // DATA_ICACHE_H

