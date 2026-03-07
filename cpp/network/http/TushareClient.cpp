#include "TushareClient.h"
#include "Config.h"
#include "Logger.h"

namespace network {

TushareClient::TushareClient() {
    // 从配置模块获取参数
    auto& config = config::Config::getInstance();
    api_token_ = config.getDataSourceApiKey();
    std::string base_url = config.getDataSourceUrl();
    
    if (api_token_.empty()) {
        LOG_ERROR("未配置 DATA_SOURCE_API_KEY");
        throw std::runtime_error("DATA_SOURCE_API_KEY not configured");
    }
    
    // 创建 HTTP 客户端
    http_client_ = std::make_unique<HttpClient>(base_url, config.getDataSourceTimeout());
    http_client_->setRetryTimes(config.getDataSourceRetryTimes());
    http_client_->setRetryDelay(config.getDataSourceRetryDelay());
    
    LOG_INFO("TushareClient 初始化完成（从配置）");
}

TushareClient::TushareClient(const std::string& api_token, const std::string& base_url)
    : api_token_(api_token) {
    http_client_ = std::make_unique<HttpClient>(base_url);
    LOG_INFO("TushareClient 初始化完成（手动配置）");
}

TushareResponse TushareClient::query(const std::string& api_name,
                                     const std::map<std::string, std::string>& params,
                                     const std::vector<std::string>& fields) {
    LOG_DEBUG("调用 Tushare API: " + api_name);
    
    // 构建请求 JSON
    std::string request_body = buildRequestJson(api_name, params, fields);
    
    // 发送 POST 请求
    auto http_response = http_client_->post("/", request_body, "application/json");
    
    // 解析响应
    if (http_response.isSuccess()) {
        return parseResponse(http_response.body);
    } else {
        TushareResponse response;
        response.code = -1;
        response.msg = "HTTP 请求失败: " + http_response.error_message;
        LOG_ERROR(response.msg);
        return response;
    }
}

// ========== 基础数据接口 ==========

TushareResponse TushareClient::getStockBasic(const std::string& list_status,
                                             const std::string& exchange) {
    std::map<std::string, std::string> params;
    if (!list_status.empty()) params["list_status"] = list_status;
    if (!exchange.empty()) params["exchange"] = exchange;
    
    return query("stock_basic", params);
}

TushareResponse TushareClient::getTradeCal(const std::string& start_date,
                                           const std::string& end_date,
                                           const std::string& exchange) {
    std::map<std::string, std::string> params;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    if (!exchange.empty()) params["exchange"] = exchange;
    
    return query("trade_cal", params);
}

// ========== 行情数据接口 ==========

TushareResponse TushareClient::getDailyQuote(const std::string& ts_code,
                                             const std::string& trade_date,
                                             const std::string& start_date,
                                             const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!trade_date.empty()) params["trade_date"] = trade_date;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("daily", params);
}

TushareResponse TushareClient::getWeeklyQuote(const std::string& ts_code,
                                              const std::string& trade_date,
                                              const std::string& start_date,
                                              const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!trade_date.empty()) params["trade_date"] = trade_date;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("weekly", params);
}

TushareResponse TushareClient::getMonthlyQuote(const std::string& ts_code,
                                               const std::string& trade_date,
                                               const std::string& start_date,
                                               const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!trade_date.empty()) params["trade_date"] = trade_date;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("monthly", params);
}

TushareResponse TushareClient::getAdjFactor(const std::string& ts_code,
                                            const std::string& trade_date,
                                            const std::string& start_date,
                                            const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!trade_date.empty()) params["trade_date"] = trade_date;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("adj_factor", params);
}

TushareResponse TushareClient::getDailyBasic(const std::string& ts_code,
                                             const std::string& trade_date,
                                             const std::string& start_date,
                                             const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!trade_date.empty()) params["trade_date"] = trade_date;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("daily_basic", params);
}

// ========== 财务数据接口 ==========

TushareResponse TushareClient::getIncome(const std::string& ts_code,
                                        const std::string& period,
                                        const std::string& start_date,
                                        const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!period.empty()) params["period"] = period;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("income", params);
}

TushareResponse TushareClient::getBalanceSheet(const std::string& ts_code,
                                               const std::string& period,
                                               const std::string& start_date,
                                               const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!period.empty()) params["period"] = period;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("balancesheet", params);
}

TushareResponse TushareClient::getCashFlow(const std::string& ts_code,
                                          const std::string& period,
                                          const std::string& start_date,
                                          const std::string& end_date) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!period.empty()) params["period"] = period;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("cashflow", params);
}

// ========== 市场参考数据 ==========

TushareResponse TushareClient::getHsConst(const std::string& ts_code,
                                         const std::string& hs_type) {
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!hs_type.empty()) params["hs_type"] = hs_type;
    
    return query("hs_const", params);
}

// ========== 配置方法 ==========

void TushareClient::setTimeout(int seconds) {
    http_client_->setTimeout(seconds);
}

void TushareClient::setRetryTimes(int times) {
    http_client_->setRetryTimes(times);
}

// ========== 私有方法 ==========

std::string TushareClient::buildRequestJson(const std::string& api_name,
                                           const std::map<std::string, std::string>& params,
                                           const std::vector<std::string>& fields) {
    json request;
    request["api_name"] = api_name;
    request["token"] = api_token_;
    
    // 添加参数
    json params_json;
    for (const auto& [key, value] : params) {
        params_json[key] = value;
    }
    request["params"] = params_json;
    
    // 添加字段
    if (!fields.empty()) {
        request["fields"] = fields;
    } else {
        request["fields"] = "";
    }
    
    return request.dump();
}

TushareResponse TushareClient::parseResponse(const std::string& response_body) {
    TushareResponse response;
    
    try {
        json response_json = json::parse(response_body);
        
        response.code = response_json.value("code", -1);
        response.msg = response_json.value("msg", "");
        
        if (response_json.contains("data")) {
            response.data = response_json["data"];
        }
        
        if (!response.isSuccess()) {
            LOG_ERROR("Tushare API 返回错误: " + response.msg);
        }
        
    } catch (const json::exception& e) {
        response.code = -1;
        response.msg = "JSON 解析失败: " + std::string(e.what());
        LOG_ERROR(response.msg);
    }
    
    return response;
}

} // namespace network

