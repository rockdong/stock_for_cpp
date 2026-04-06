#pragma once

#include <memory>
#include "EventBus.hpp"

namespace eventbus {

class EventBusManager {
public:
    static EventBusManager& getInstance();
    
    std::shared_ptr<dexode::EventBus> getCoreBus();
    
    void process();
    
private:
    EventBusManager();
    ~EventBusManager();
    
    EventBusManager(const EventBusManager&) = delete;
    EventBusManager& operator=(const EventBusManager&) = delete;
    
    std::shared_ptr<dexode::EventBus> coreBus_;
};

}