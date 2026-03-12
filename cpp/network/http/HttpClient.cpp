#include "HttpClient.h"
#include "Logger.h"
#include <thread>
#include <chrono>

namespace network {

HttpClient::HttpClient(const std::string& base_url, int timeout_seconds)
    : base_url_(base_url)
    , timeout_seconds_(timeout_seconds)
    , retry_times_(3)
    , retry_delay_ms_(1000) {
    
    // 简单解析 URL（只支持 HTTP）
    // 格式: http://host:port 或 http://host
    size_t protocol_pos = base_url.find("://");
    if (protocol_pos == std::string::npos) {
        LOG_ERROR("无效的 URL 格式: " + base_url);
        throw std::invalid_argument("Invalid URL format: " + base_url);
    }
    
    std::string protocol = base_url.substr(0, protocol_pos);
    std::string rest = base_url.substr(protocol_pos + 3);
    
    // 只支持 HTTP 协议
    if (protocol != "http") {
        LOG_ERROR("只支持 HTTP 协议，不支持: " + protocol);
        throw std::invalid_argument("Only HTTP protocol is supported, got: " + protocol);
    }
    
    // 提取主机和端口
    size_t port_pos = rest.find(':');
    size_t path_pos = rest.find('/');
    
    if (port_pos != std::string::npos && (path_pos == std::string::npos || port_pos < path_pos)) {
        // 有端口号: http://host:port 或 http://host:port/path
        host_ = rest.substr(0, port_pos);
        size_t port_end = (path_pos != std::string::npos) ? path_pos : rest.length();
        std::string port_str = rest.substr(port_pos + 1, port_end - port_pos - 1);
        try {
            port_ = std::stoi(port_str);
        } catch (const std::exception& e) {
            LOG_ERROR("无效的端口号: " + port_str);
            throw std::invalid_argument("Invalid port number: " + port_str);
        }
    } else {
        // 没有端口号: http://host 或 http://host/path
        host_ = (path_pos != std::string::npos) ? rest.substr(0, path_pos) : rest;
        port_ = 80;  // HTTP 默认端口
    }
    
    // 创建 HTTP 客户端
    client_ = std::make_shared<httplib::Client>(host_, port_);
    
    // 设置超时
    client_->set_connection_timeout(timeout_seconds_);
    client_->set_read_timeout(timeout_seconds_);
    client_->set_write_timeout(timeout_seconds_);
    
    LOG_DEBUG("HttpClient 初始化: " + base_url + " (host=" + host_ + ", port=" + std::to_string(port_) + ")");
}

HttpResponse HttpClient::get(const std::string& path,
                              const std::map<std::string, std::string>& headers) {
    return executeWithRetry([&]() -> HttpResponse {
        HttpResponse response;
        
        // 设置请求头
        httplib::Headers req_headers;
        for (const auto& [key, value] : headers) {
            req_headers.emplace(key, value);
        }
        
        // 发送请求
        auto res = client_->Get(path.c_str(), req_headers);
        
        if (res) {
            response.status_code = res->status;
            response.body = res->body;
            
            for (const auto& [key, value] : res->headers) {
                response.headers[key] = value;
            }
            
            LOG_DEBUG("GET " + path + " - Status: " + std::to_string(res->status));
        } else {
            response.status_code = 0;
            std::string error_detail = httplib::to_string(res.error());
            response.error_message = "Request failed: " + error_detail;
            
            // 在超时或请求失败时打印详细信息
            LOG_ERROR("GET " + path + " 失败: " + error_detail);
            LOG_ERROR("错误类型: " + std::to_string((int)res.error()));
        }
        
        return response;
    });
}

HttpResponse HttpClient::post(const std::string& path,
                               const std::string& body,
                               const std::string& content_type,
                               const std::map<std::string, std::string>& headers) {
    return executeWithRetry([&]() -> HttpResponse {
        HttpResponse response;
        
        // 设置请求头
        httplib::Headers req_headers;
        req_headers.emplace("Content-Type", content_type);
        for (const auto& [key, value] : headers) {
            req_headers.emplace(key, value);
        }
        
        // 发送请求
        auto res = client_->Post(path.c_str(), req_headers, body, content_type.c_str());
        
        if (res) {
            response.status_code = res->status;
            response.body = res->body;
            
            for (const auto& [key, value] : res->headers) {
                response.headers[key] = value;
            }
            
            LOG_DEBUG("POST " + path + " - Status: " + std::to_string(res->status));
        } else {
            response.status_code = 0;
            std::string error_detail = httplib::to_string(res.error());
            response.error_message = "Request failed: " + error_detail;
            
            // 在超时或请求失败时打印详细信息
            LOG_ERROR("POST " + path + " 失败: " + error_detail);
            LOG_ERROR("错误类型: " + std::to_string((int)res.error()));
        }
        
        return response;
    });
}

void HttpClient::setTimeout(int seconds) {
    timeout_seconds_ = seconds;
    if (client_) {
        client_->set_connection_timeout(seconds);
        client_->set_read_timeout(seconds);
        client_->set_write_timeout(seconds);
    }
}

void HttpClient::setRetryTimes(int times) {
    retry_times_ = times;
}

void HttpClient::setRetryDelay(int milliseconds) {
    retry_delay_ms_ = milliseconds;
}

template<typename Func>
HttpResponse HttpClient::executeWithRetry(Func request_func) {
    HttpResponse response;
    
    for (int i = 0; i <= retry_times_; ++i) {
        if (i > 0) {
            LOG_WARN("重试请求 (" + std::to_string(i) + "/" + std::to_string(retry_times_) + ")");
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_ms_));
        }
        
        response = request_func();
        
        if (response.isSuccess()) {
            return response;
        }
    }
    
    LOG_ERROR("请求失败，已重试 " + std::to_string(retry_times_) + " 次");
    return response;
}

} // namespace network

