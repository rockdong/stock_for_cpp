#include "TushareDataSource.h"
#include "Config.h"
#include "Logger.h"

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

std::vector<StockBasic> TushareDataSource::getStockList() {
    LOG_DEBUG("获取股票列表");
    
    auto response = client_->getStockBasic("L", "");
    
    if (response.isSuccess()) {
        return parseStockBasic(response);
    } else {
        LOG_ERROR("获取股票列表失败: " + response.msg);
        return {};
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

std::vector<StockBasic> TushareDataSource::parseStockBasic(const TushareResponse& response) {
    std::vector<StockBasic> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        // 查找字段索引
        int ts_code_idx = -1, symbol_idx = -1, name_idx = -1;
        int area_idx = -1, industry_idx = -1, market_idx = -1, list_date_idx = -1;
        
        for (size_t i = 0; i < fields.size(); ++i) {
            if (fields[i] == "ts_code") ts_code_idx = i;
            else if (fields[i] == "symbol") symbol_idx = i;
            else if (fields[i] == "name") name_idx = i;
            else if (fields[i] == "area") area_idx = i;
            else if (fields[i] == "industry") industry_idx = i;
            else if (fields[i] == "market") market_idx = i;
            else if (fields[i] == "list_date") list_date_idx = i;
        }
        
        // 解析数据
        for (const auto& item : items) {
            StockBasic stock;
            
            if (ts_code_idx >= 0 && !item[ts_code_idx].is_null())
                stock.ts_code = item[ts_code_idx].get<std::string>();
            if (symbol_idx >= 0 && !item[symbol_idx].is_null())
                stock.symbol = item[symbol_idx].get<std::string>();
            if (name_idx >= 0 && !item[name_idx].is_null())
                stock.name = item[name_idx].get<std::string>();
            if (area_idx >= 0 && !item[area_idx].is_null())
                stock.area = item[area_idx].get<std::string>();
            if (industry_idx >= 0 && !item[industry_idx].is_null())
                stock.industry = item[industry_idx].get<std::string>();
            if (market_idx >= 0 && !item[market_idx].is_null())
                stock.market = item[market_idx].get<std::string>();
            if (list_date_idx >= 0 && !item[list_date_idx].is_null())
                stock.list_date = item[list_date_idx].get<std::string>();
            
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
        
        // 查找字段索引
        int ts_code_idx = -1, trade_date_idx = -1;
        int open_idx = -1, high_idx = -1, low_idx = -1, close_idx = -1;
        int pre_close_idx = -1, change_idx = -1, pct_chg_idx = -1;
        int vol_idx = -1, amount_idx = -1;
        
        for (size_t i = 0; i < fields.size(); ++i) {
            if (fields[i] == "ts_code") ts_code_idx = i;
            else if (fields[i] == "trade_date") trade_date_idx = i;
            else if (fields[i] == "open") open_idx = i;
            else if (fields[i] == "high") high_idx = i;
            else if (fields[i] == "low") low_idx = i;
            else if (fields[i] == "close") close_idx = i;
            else if (fields[i] == "pre_close") pre_close_idx = i;
            else if (fields[i] == "change") change_idx = i;
            else if (fields[i] == "pct_chg") pct_chg_idx = i;
            else if (fields[i] == "vol") vol_idx = i;
            else if (fields[i] == "amount") amount_idx = i;
        }
        
        // 解析数据
        for (const auto& item : items) {
            StockData data;
            
            if (ts_code_idx >= 0 && !item[ts_code_idx].is_null())
                data.ts_code = item[ts_code_idx].get<std::string>();
            if (trade_date_idx >= 0 && !item[trade_date_idx].is_null())
                data.trade_date = item[trade_date_idx].get<std::string>();
            if (open_idx >= 0 && !item[open_idx].is_null())
                data.open = item[open_idx].get<double>();
            if (high_idx >= 0 && !item[high_idx].is_null())
                data.high = item[high_idx].get<double>();
            if (low_idx >= 0 && !item[low_idx].is_null())
                data.low = item[low_idx].get<double>();
            if (close_idx >= 0 && !item[close_idx].is_null())
                data.close = item[close_idx].get<double>();
            if (pre_close_idx >= 0 && !item[pre_close_idx].is_null())
                data.pre_close = item[pre_close_idx].get<double>();
            if (change_idx >= 0 && !item[change_idx].is_null())
                data.change = item[change_idx].get<double>();
            if (pct_chg_idx >= 0 && !item[pct_chg_idx].is_null())
                data.pct_chg = item[pct_chg_idx].get<double>();
            if (vol_idx >= 0 && !item[vol_idx].is_null())
                data.volume = item[vol_idx].get<long>();
            if (amount_idx >= 0 && !item[amount_idx].is_null())
                data.amount = item[amount_idx].get<double>();
            
            result.push_back(data);
        }
        
        LOG_INFO("解析股票数据成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析股票数据失败: " + std::string(e.what()));
    }
    
    return result;
}

} // namespace network

