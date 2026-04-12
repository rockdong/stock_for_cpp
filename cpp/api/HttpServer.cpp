#include "HttpServer.h"
#include "ApiRouter.h"
#include "../thirdparty/cpp-httplib/httplib.h"
#include "../log/Logger.h"

namespace api {

HttpServer::HttpServer(int port)
    : port_(port)
    , running_(false)
    , server_(std::make_unique<httplib::Server>())
{
}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    if (running_) return;
    
    setupRoutes();
    
    running_ = true;
    serverThread_ = std::thread([this]() {
        LOG_INFO("HTTP API Server 启动，端口: " + std::to_string(port_));
        if (!server_->listen("0.0.0.0", port_)) {
            LOG_ERROR("HTTP API Server 启动失败");
            running_ = false;
        }
    });
}

void HttpServer::stop() {
    if (!running_) return;
    
    LOG_INFO("HTTP API Server 正在停止...");
    server_->stop();
    
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
    
    running_ = false;
    LOG_INFO("HTTP API Server 已停止");
}

bool HttpServer::isRunning() const {
    return running_;
}

int HttpServer::getPort() const {
    return port_;
}

void HttpServer::setupRoutes() {
    api::setupRoutes(*server_);
}

} // namespace api