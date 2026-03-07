#ifndef DATA_MEMORY_CACHE_H
#define DATA_MEMORY_CACHE_H

#include "ICache.h"
#include <unordered_map>
#include <list>
#include <mutex>
#include <chrono>

namespace data {

/**
 * @brief 缓存项
 */
struct CacheItem {
    std::any value;
    std::chrono::steady_clock::time_point expireTime;
    bool hasExpire;
    
    CacheItem(const std::any& v, int ttl)
        : value(v)
        , hasExpire(ttl > 0) {
        if (hasExpire) {
            expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttl);
        }
    }
    
    bool isExpired() const {
        if (!hasExpire) {
            return false;
        }
        return std::chrono::steady_clock::now() > expireTime;
    }
};

/**
 * @brief 内存缓存实现（LRU 策略）
 * 职责：提供基于内存的缓存功能
 */
class MemoryCache : public ICache {
public:
    /**
     * @brief 构造函数
     * @param maxSize 最大缓存数量
     */
    explicit MemoryCache(size_t maxSize = 1000);
    
    ~MemoryCache() override = default;

    /**
     * @brief 设置缓存
     * @param key 键
     * @param value 值
     * @param ttl 过期时间（秒），0 表示永不过期
     * @return 是否成功
     */
    bool set(const std::string& key, const std::any& value, int ttl = 0) override;

    /**
     * @brief 获取缓存
     * @param key 键
     * @return 值（可选）
     */
    std::optional<std::any> get(const std::string& key) override;

    /**
     * @brief 删除缓存
     * @param key 键
     * @return 是否成功
     */
    bool remove(const std::string& key) override;

    /**
     * @brief 检查键是否存在
     * @param key 键
     * @return 是否存在
     */
    bool exists(const std::string& key) override;

    /**
     * @brief 清空所有缓存
     */
    void clear() override;

    /**
     * @brief 获取缓存大小
     * @return 缓存项数量
     */
    size_t size() const override;

    /**
     * @brief 清理过期缓存
     * @return 清理的数量
     */
    size_t cleanup() override;

    /**
     * @brief 设置最大缓存数量
     * @param maxSize 最大数量
     */
    void setMaxSize(size_t maxSize);

    /**
     * @brief 获取最大缓存数量
     * @return 最大数量
     */
    size_t getMaxSize() const;

    /**
     * @brief 获取缓存命中率
     * @return 命中率（0.0 - 1.0）
     */
    double getHitRate() const;

private:
    /**
     * @brief 移除最少使用的项（LRU）
     */
    void evictLRU();

    /**
     * @brief 更新访问顺序
     * @param key 键
     */
    void touch(const std::string& key);

    size_t maxSize_;
    std::unordered_map<std::string, CacheItem> cache_;
    std::list<std::string> lruList_;  // 最近使用列表（头部最新，尾部最旧）
    std::unordered_map<std::string, std::list<std::string>::iterator> lruMap_;
    
    mutable std::mutex mutex_;
    
    // 统计信息
    size_t hitCount_ = 0;
    size_t missCount_ = 0;
};

} // namespace data

#endif // DATA_MEMORY_CACHE_H

