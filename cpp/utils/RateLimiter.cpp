#include "RateLimiter.h"
#include "../log/Logger.h"
#include <stdexcept>

RateLimiter::RateLimiter()
    : tokens_(0)
    , max_tokens_(10)
    , refill_rate_(8)
    , last_refill_(std::chrono::steady_clock::now())
    , stopped_(false) {
}

RateLimiter& RateLimiter::getInstance() {
    static RateLimiter instance;
    return instance;
}

void RateLimiter::configure(int rate_per_second, int burst_size) {
    std::lock_guard<std::mutex> lock(mutex_);
    refill_rate_ = rate_per_second;
    max_tokens_ = burst_size;
    tokens_ = burst_size;
    last_refill_ = std::chrono::steady_clock::now();
    LOG_INFO("RateLimiter 配置: rate=" + std::to_string(rate_per_second) + 
             "/秒, burst=" + std::to_string(burst_size));
}

void RateLimiter::refill() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refill_);
    
    if (elapsed.count() >= 1000 / refill_rate_) {
        int new_tokens = static_cast<int>(elapsed.count() * refill_rate_ / 1000);
        tokens_ = std::min(tokens_ + new_tokens, max_tokens_);
        last_refill_ = now;
    }
}

void RateLimiter::acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    
    while (true) {
        if (stopped_) {
            throw std::runtime_error("RateLimiter stopped");
        }
        
        refill();
        
        if (tokens_ > 0) {
            tokens_--;
            return;
        }
        
        cv_.wait_for(lock, std::chrono::milliseconds(100));
    }
}

void RateLimiter::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    stopped_ = true;
    cv_.notify_all();
    LOG_INFO("RateLimiter 已停止");
}