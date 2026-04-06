#pragma once

#include "EventBusManager.h"
#include "events/SystemEvents.h"

namespace eventbus {

inline bool initialize() {
    try {
        auto& manager = EventBusManager::getInstance();
        return manager.getCoreBus() != nullptr;
    } catch (...) {
        return false;
    }
}

inline void shutdown() {
    try {
        auto& manager = EventBusManager::getInstance();
        manager.process();
    } catch (...) {
    }
}

}