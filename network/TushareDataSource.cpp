#include "TushareDataSource.h"
#include "Config.h"
#include "Logger.h"
#include <unordered_map>

namespace network {

TushareDataSource::TushareDataSource() {
    // 从配置模块创建客户端
    client_ = std::make_unique<TushareClient>();
    LOG_INFO("TushareDataSource 初始化完成（从配置）");
}

TushareDataSource::TushareDataSource(const std::string& api_token) {
    // 手动指定 Token
    auto& config = config::Config::getInstance();
    std::string base_url = config.getDataSourceUrl();
    client_ = std::make_unique<TushareClient>(api_token, base_url);
    LOG_INFO("TushareDataSource 初始化完成（手动配置）");
}

std::vector<Stock> TushareDataSource::getStockList() {
    return getStockList("L", "");
}

std::vector<Stock> TushareDataSource::getStockList(
    const std::string& list_status,
    const std::string& exchange) {
    
    LOG_DEBUG("获取股票列表 [状态=" + list_status + ", 交易所=" + 
              (exchange.empty() ? "全部" : exchange) + "]");
    
    auto response = client_->getStockBasic(list_status, exchange);
    
    if (response.isSuccess()) {
        return parseStockBasic(response);
    } else {
        LOG_ERROR("获取股票列表失败: " + response.msg);
        return {};
    }
}

Stock TushareDataSource::getStockInfo(const std::string& ts_code) {
    LOG_DEBUG("获取股票信息: " + ts_code);
    
    // 使用通用查询接口，指定 ts_code 参数
    std::map<std::string, std::string> params;
    params["ts_code"] = ts_code;
    
    auto response = client_->query("stock_basic", params);
    
    if (response.isSuccess()) {
        auto stocks = parseStockBasic(response);
        if (!stocks.empty()) {
            return stocks[0];
        } else {
            LOG_WARN("未找到股票: " + ts_code);
            return Stock{};
        }
    } else {
        LOG_ERROR("获取股票信息失败: " + response.msg);
        return Stock{};
    }
}

std::vector<StockData> TushareDataSource::getDailyData(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date) {
    
    LOG_DEBUG("获取日线数据: " + ts_code + " [" + start_date + " - " + end_date + "]");
    
    auto response = client_->getDailyQuote(ts_code, "", start_date, end_date);
    
    if (response.isSuccess()) {
        return parseStockData(response);
    } else {
        LOG_ERROR("获取日线数据失败: " + response.msg);
        return {};
    }
}

std::vector<StockData> TushareDataSource::getQuoteData(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date,
    const std::string& freq) {
    
    std::string freq_name;
    TushareResponse response;
    
    // 根据频率参数调用不同的接口
    if (freq == "d" || freq == "D") {
        freq_name = "日线";
        response = client_->getDailyQuote(ts_code, "", start_date, end_date);
    } else if (freq == "w" || freq == "W") {
        freq_name = "周线";
        response = client_->getWeeklyQuote(ts_code, "", start_date, end_date);
    } else if (freq == "m" || freq == "M") {
        freq_name = "月线";
        response = client_->getMonthlyQuote(ts_code, "", start_date, end_date);
    } else {
        LOG_ERROR("不支持的频率参数: " + freq + "，支持的参数: d(日线), w(周线), m(月线)");
        return {};
    }
    
    LOG_DEBUG("获取" + freq_name + "数据: " + ts_code + " [" + start_date + " - " + end_date + "]");
    
    if (response.isSuccess()) {
        return parseStockData(response);
    } else {
        LOG_ERROR("获取" + freq_name + "数据失败: " + response.msg);
        return {};
    }
}

StockData TushareDataSource::getLatestQuote(const std::string& ts_code) {
    LOG_DEBUG("获取最新行情: " + ts_code);
    
    auto data_list = getDailyData(ts_code, "", "");
    
    if (!data_list.empty()) {
        return data_list[0];  // 返回最新的一条数据
    } else {
        LOG_WARN("未获取到最新行情: " + ts_code);
        return StockData{};
    }
}

bool TushareDataSource::testConnection() {
    LOG_DEBUG("测试 Tushare 连接");
    
    try {
        auto response = client_->getTradeCal("20240101", "20240101");
        return response.isSuccess();
    } catch (const std::exception& e) {
        LOG_ERROR("连接测试失败: " + std::string(e.what()));
        return false;
    }
}

// ========== 私有方法 ==========

std::vector<Stock> TushareDataSource::parseStockBasic(const TushareResponse& response) {
    std::vector<Stock> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        // 构建字段索引映射
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        // 辅助函数：安全获取字符串字段
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        // 解析数据
        for (const auto& item : items) {
            Stock stock;
            
            // 基础字段
            stock.ts_code = get_string(item, "ts_code");
            stock.symbol = get_string(item, "symbol");
            stock.name = get_string(item, "name");
            
            // 详细信息
            stock.area = get_string(item, "area");
            stock.industry = get_string(item, "industry");
            stock.fullname = get_string(item, "fullname");
            stock.enname = get_string(item, "enname");
            stock.cnspell = get_string(item, "cnspell");
            
            // 市场信息
            stock.market = get_string(item, "market");
            stock.exchange = get_string(item, "exchange");
            stock.curr_type = get_string(item, "curr_type");
            
            // 上市信息
            stock.list_status = get_string(item, "list_status");
            stock.list_date = get_string(item, "list_date");
            stock.delist_date = get_string(item, "delist_date");
            stock.is_hs = get_string(item, "is_hs");
            
            result.push_back(stock);
        }
        
        LOG_INFO("解析股票列表成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析股票列表失败: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<StockData> TushareDataSource::parseStockData(const TushareResponse& response) {
    std::vector<StockData> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        // 构建字段索引映射
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        // 辅助函数：安全获取字符串字段
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        // 辅助函数：安全获取 double 字段
        auto get_double = [&](const json& item, const std::string& field_name) -> double {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        // 辅助函数：安全获取 long 字段
        auto get_long = [&](const json& item, const std::string& field_name) -> long {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<long>();
            }
            return 0;
        };
        
        // 解析数据
        for (const auto& item : items) {
            StockData data;
            
            data.ts_code = get_string(item, "ts_code");
            data.trade_date = get_string(item, "trade_date");
            data.open = get_double(item, "open");
            data.high = get_double(item, "high");
            data.low = get_double(item, "low");
            data.close = get_double(item, "close");
            data.pre_close = get_double(item, "pre_close");
            data.change = get_double(item, "change");
            data.pct_chg = get_double(item, "pct_chg");
            data.volume = get_long(item, "vol");
            data.amount = get_double(item, "amount");
            
            result.push_back(data);
        }
        
        LOG_INFO("解析股票数据成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析股票数据失败: " + std::string(e.what()));
    }
    
    return result;
}

} // namespace network

