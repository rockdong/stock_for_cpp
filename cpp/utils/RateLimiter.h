#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <mutex>
#include <condition_variable>
#include <chrono>

class RateLimiter {
public:
    static RateLimiter& getInstance();
    
    void configure(int rate_per_second, int burst_size);
    void acquire();
    void stop();
    
private:
    RateLimiter();
    ~RateLimiter() = default;
    
    RateLimiter(const RateLimiter&) = delete;
    RateLimiter& operator=(const RateLimiter&) = delete;
    
    void refill();
    
    std::mutex mutex_;
    std::condition_variable cv_;
    int tokens_;
    int max_tokens_;
    int refill_rate_;
    std::chrono::steady_clock::time_point last_refill_;
    bool stopped_;
};

#endif