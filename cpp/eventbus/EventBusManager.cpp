#include "EventBusManager.h"
#include "../log/Logger.h"
#include <stdexcept>

namespace eventbus {

EventBusManager& EventBusManager::getInstance() {
    static EventBusManager instance;
    return instance;
}

EventBusManager::EventBusManager() {
    try {
        coreBus_ = std::make_shared<dexode::EventBus>();
        LOG_INFO("EventBus initialized successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("EventBus initialization failed: " + std::string(e.what()));
        throw std::runtime_error("EventBus initialization failed");
    }
}

EventBusManager::~EventBusManager() {
    LOG_INFO("EventBus destroyed");
}

std::shared_ptr<dexode::EventBus> EventBusManager::getCoreBus() {
    return coreBus_;
}

void EventBusManager::process() {
    try {
        coreBus_->process();
    } catch (const std::exception& e) {
        LOG_ERROR("EventBus processing failed: " + std::string(e.what()));
    }
}

}