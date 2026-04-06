#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <typeindex>
#include <unordered_map>

namespace dexode {

class EventBus {
public:
    EventBus() = default;
    ~EventBus() = default;

    template <typename Event>
    void postpone(Event event) {
        std::lock_guard<std::mutex> lock(mutex_);
        eventQueue_.push([this, event = std::move(event)]() {
            fireImmediate<Event>(std::move(event));
        });
    }

    void process() {
        std::queue<std::function<void()>> tempQueue;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tempQueue = std::move(eventQueue_);
            eventQueue_ = {};
        }

        while (!tempQueue.empty()) {
            auto& handler = tempQueue.front();
            if (handler) {
                handler();
            }
            tempQueue.pop();
        }
    }

    template <typename Event>
    void fire(Event event) {
        fireImmediate<Event>(std::move(event));
    }

    class Listener {
    public:
        explicit Listener(std::shared_ptr<EventBus> bus) : bus_(bus) {}

        template <typename Event, typename Callback>
        void listen(Callback callback) {
            auto bus = bus_.lock();
            if (bus) {
                bus->addListener<Event>(callback);
            }
        }

    private:
        std::weak_ptr<EventBus> bus_;
    };

private:
    std::mutex mutex_;
    std::queue<std::function<void()>> eventQueue_;
    std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> listeners_;

    template <typename Event>
    void addListener(std::function<void(const Event&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto idx = std::type_index(typeid(Event));
        listeners_[idx].push_back([callback](const void* event) {
            callback(*static_cast<const Event*>(event));
        });
    }

    template <typename Event>
    void fireImmediate(Event event) {
        std::vector<std::function<void(const void*)>> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto idx = std::type_index(typeid(Event));
            auto it = listeners_.find(idx);
            if (it != listeners_.end()) {
                callbacks = it->second;
            }
        }

        for (auto& callback : callbacks) {
            callback(&event);
        }
    }
};

}