#include "MemoryCache.h"
#include "Logger.h"

namespace data {

MemoryCache::MemoryCache(size_t maxSize)
    : maxSize_(maxSize) {
    LOG_INFO("内存缓存初始化，最大容量: " + std::to_string(maxSize_));
}

bool MemoryCache::set(const std::string& key, const std::any& value, int ttl) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 如果键已存在，先删除
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        // 从 LRU 列表中移除
        lruList_.erase(lruMap_[key]);
        lruMap_.erase(key);
        cache_.erase(it);
    }
    
    // 如果缓存已满，移除最少使用的项
    if (cache_.size() >= maxSize_) {
        evictLRU();
    }
    
    // 插入新项
    cache_.emplace(key, CacheItem(value, ttl));
    lruList_.push_front(key);
    lruMap_[key] = lruList_.begin();
    
    LOG_DEBUG("缓存设置: " + key + ", TTL=" + std::to_string(ttl));
    return true;
}

std::optional<std::any> MemoryCache::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        missCount_++;
        LOG_DEBUG("缓存未命中: " + key);
        return std::nullopt;
    }
    
    // 检查是否过期
    if (it->second.isExpired()) {
        // 移除过期项
        lruList_.erase(lruMap_[key]);
        lruMap_.erase(key);
        cache_.erase(it);
        missCount_++;
        LOG_DEBUG("缓存已过期: " + key);
        return std::nullopt;
    }
    
    // 更新访问顺序
    touch(key);
    hitCount_++;
    LOG_DEBUG("缓存命中: " + key);
    return it->second.value;
}

bool MemoryCache::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return false;
    }
    
    lruList_.erase(lruMap_[key]);
    lruMap_.erase(key);
    cache_.erase(it);
    
    LOG_DEBUG("缓存删除: " + key);
    return true;
}

bool MemoryCache::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return false;
    }
    
    // 检查是否过期
    if (it->second.isExpired()) {
        return false;
    }
    
    return true;
}

void MemoryCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    cache_.clear();
    lruList_.clear();
    lruMap_.clear();
    hitCount_ = 0;
    missCount_ = 0;
    
    LOG_INFO("缓存已清空");
}

size_t MemoryCache::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_.size();
}

size_t MemoryCache::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t cleanedCount = 0;
    auto it = cache_.begin();
    while (it != cache_.end()) {
        if (it->second.isExpired()) {
            lruList_.erase(lruMap_[it->first]);
            lruMap_.erase(it->first);
            it = cache_.erase(it);
            cleanedCount++;
        } else {
            ++it;
        }
    }
    
    if (cleanedCount > 0) {
        LOG_INFO("清理过期缓存: " + std::to_string(cleanedCount) + " 项");
    }
    
    return cleanedCount;
}

void MemoryCache::setMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    maxSize_ = maxSize;
    
    // 如果当前大小超过新的最大值，移除多余项
    while (cache_.size() > maxSize_) {
        evictLRU();
    }
    
    LOG_INFO("缓存最大容量设置为: " + std::to_string(maxSize_));
}

size_t MemoryCache::getMaxSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return maxSize_;
}

double MemoryCache::getHitRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t total = hitCount_ + missCount_;
    if (total == 0) {
        return 0.0;
    }
    
    return static_cast<double>(hitCount_) / total;
}

void MemoryCache::evictLRU() {
    if (lruList_.empty()) {
        return;
    }
    
    // 移除最少使用的项（列表尾部）
    std::string key = lruList_.back();
    lruList_.pop_back();
    lruMap_.erase(key);
    cache_.erase(key);
    
    LOG_DEBUG("LRU 淘汰: " + key);
}

void MemoryCache::touch(const std::string& key) {
    // 将键移到列表头部（最近使用）
    auto it = lruMap_.find(key);
    if (it != lruMap_.end()) {
        lruList_.erase(it->second);
        lruList_.push_front(key);
        lruMap_[key] = lruList_.begin();
    }
}

} // namespace data

