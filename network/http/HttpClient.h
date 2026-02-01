#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <map>
#include <memory>

// 禁用 SSL 支持（本项目只使用 HTTP）
// 注意：httplib.h 使用 #ifdef 检查，所以不要定义这个宏
// CMakeLists.txt 中已经通过 HTTPLIB_USE_OPENSSL_IF_AVAILABLE=OFF 禁用了

#include <httplib.h>

namespace network {

/**
 * @brief HTTP 响应结构
 */
struct HttpResponse {
    int status_code;                          // HTTP 状态码
    std::string body;                         // 响应体
    std::map<std::string, std::string> headers;  // 响应头
    std::string error_message;                // 错误信息
    
    bool isSuccess() const {
        return status_code >= 200 && status_code < 300;
    }
};

/**
 * @brief HTTP 客户端类
 * 封装 cpp-httplib，提供简单易用的 HTTP 请求接口
 */
class HttpClient {
public:
    /**
     * @brief 构造函数
     * @param base_url 基础 URL（如 http://api.waditu.com）
     * @param timeout_seconds 超时时间（秒）
     */
    explicit HttpClient(const std::string& base_url, int timeout_seconds = 30);
    
    ~HttpClient() = default;

    /**
     * @brief GET 请求
     * @param path 请求路径
     * @param headers 请求头
     * @return HTTP 响应
     */
    HttpResponse get(const std::string& path, 
                     const std::map<std::string, std::string>& headers = {});

    /**
     * @brief POST 请求
     * @param path 请求路径
     * @param body 请求体
     * @param content_type 内容类型
     * @param headers 请求头
     * @return HTTP 响应
     */
    HttpResponse post(const std::string& path,
                      const std::string& body,
                      const std::string& content_type = "application/json",
                      const std::map<std::string, std::string>& headers = {});

    /**
     * @brief 设置超时时间
     * @param seconds 超时秒数
     */
    void setTimeout(int seconds);

    /**
     * @brief 设置重试次数
     * @param times 重试次数
     */
    void setRetryTimes(int times);

    /**
     * @brief 设置重试延迟
     * @param milliseconds 延迟毫秒数
     */
    void setRetryDelay(int milliseconds);

private:
    /**
     * @brief 执行 HTTP 请求（带重试）
     * @param request_func 请求函数
     * @return HTTP 响应
     */
    template<typename Func>
    HttpResponse executeWithRetry(Func request_func);

    std::string base_url_;
    std::string host_;
    int port_;
    int timeout_seconds_;
    int retry_times_;
    int retry_delay_ms_;
    
    // HTTP 客户端（只支持 HTTP，不支持 HTTPS）
    std::shared_ptr<httplib::Client> client_;
};

} // namespace network

#endif // HTTP_CLIENT_H

