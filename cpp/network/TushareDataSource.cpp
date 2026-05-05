#include "TushareDataSource.h"
#include "Config.h"
#include "Logger.h"
#include "../utils/KLineAggregator.h"
#include <unordered_map>
#include <algorithm>

namespace network {

namespace {
    bool shouldFilterStock(const std::string& ts_code, const std::string& name) {
        if (ts_code.empty()) return true;
        
        std::string symbol = ts_code.substr(0, ts_code.find('.'));
        
        if (symbol.size() >= 3 && symbol.substr(0, 3) == "300") return true;
        
        if (symbol.size() >= 3 && symbol.substr(0, 3) == "688") return true;
        
        if (!name.empty()) {
            std::string upperName = name;
            std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);
            if (upperName.find("ST") != std::string::npos) return true;
        }
        
        return false;
    }
}

TushareDataSource::TushareDataSource() {
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
    return getStockList("L", "SSE,SZSE", "主板");
}

std::vector<Stock> TushareDataSource::getStockList(
    const std::string& list_status,
    const std::string& exchange,
    const std::string& market) {
    
    try {
        LOG_DEBUG("获取股票列表 [状态=" + list_status + ", 交易所=" + 
                  (exchange.empty() ? "全部" : exchange) + "]");
        
        auto response = client_->getStockBasic(list_status, exchange);
        
        if (response.isSuccess()) {
            return parseStockBasic(response);
        } else {
            LOG_ERROR("获取股票列表失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取股票列表异常: " + std::string(e.what()));
        return {};
    }
}

Stock TushareDataSource::getStockInfo(const std::string& ts_code) {
    try {
        LOG_DEBUG("获取股票信息: " + ts_code);
        
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
    } catch (const std::exception& e) {
        LOG_ERROR("获取股票信息异常: " + ts_code + " - " + std::string(e.what()));
        return Stock{};
    }
}

std::vector<StockData> TushareDataSource::getDailyData(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date) {
    
    try {
        LOG_DEBUG("获取日线数据: " + ts_code + " [" + start_date + " - " + end_date + "]");
        
        auto response = client_->getDailyQuote(ts_code, "", start_date, end_date);
        
        if (response.isSuccess()) {
            return parseStockData(response);
        } else {
            LOG_ERROR("获取日线数据失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取日线数据异常: " + ts_code + " - " + std::string(e.what()));
        return {};
    }
}

std::vector<StockData> TushareDataSource::getQuoteData(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date,
    const std::string& freq) {
    
    try {
        std::string freq_name;
        TushareResponse response;
        
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
            auto data = parseStockData(response);
            
            if (freq == "w" || freq == "W" || freq == "m" || freq == "M") {
                auto dailyData = getDailyData(ts_code, start_date, end_date);
                
                if (data.empty() && !dailyData.empty()) {
                    LOG_INFO(freq_name + "数据为空，用日线数据聚合");
                    if (freq == "w" || freq == "W") {
                        data = utils::KLineAggregator::aggregateToWeekly(dailyData);
                    } else {
                        data = utils::KLineAggregator::aggregateToMonthly(dailyData);
                    }
                    LOG_INFO("聚合结果: " + std::to_string(data.size()) + " 条");
                } else if (!data.empty() && !dailyData.empty()) {
                    std::string lastPeriodDate = data.back().trade_date;
                    std::string lastDailyDate = dailyData.back().trade_date;
                    
                    bool needAggregate = false;
                    if (freq == "w" || freq == "W") {
                        std::string lastPeriodWeek = utils::KLineAggregator::getWeekKey(lastPeriodDate);
                        std::string lastDailyWeek = utils::KLineAggregator::getWeekKey(lastDailyDate);
                        needAggregate = (lastDailyWeek > lastPeriodWeek);
                    } else {
                        std::string lastPeriodMonth = utils::KLineAggregator::getMonthKey(lastPeriodDate);
                        std::string lastDailyMonth = utils::KLineAggregator::getMonthKey(lastDailyDate);
                        needAggregate = (lastDailyMonth > lastPeriodMonth);
                    }
                    
                    if (needAggregate) {
                        LOG_INFO("检测到未完成的" + freq_name + "数据，日线最新: " + lastDailyDate + "，" + freq_name + "最新: " + lastPeriodDate);
                        
                        std::vector<core::StockData> newDailyData;
                        for (const auto& d : dailyData) {
                            if (d.trade_date > lastPeriodDate) {
                                newDailyData.push_back(d);
                            }
                        }
                        
                        if (!newDailyData.empty()) {
                            std::vector<core::StockData> aggregatedData;
                            if (freq == "w" || freq == "W") {
                                aggregatedData = utils::KLineAggregator::aggregateToWeekly(newDailyData);
                            } else {
                                aggregatedData = utils::KLineAggregator::aggregateToMonthly(newDailyData);
                            }
                            
                            if (!aggregatedData.empty()) {
                                LOG_INFO("聚合未完成周期数据: " + std::to_string(aggregatedData.size()) + " 条");
                                data.insert(data.end(), aggregatedData.begin(), aggregatedData.end());
                            }
                        }
                    }
                }
            }
            
            return data;
        } else {
            LOG_ERROR("获取" + freq_name + "数据失败: " + response.msg);
            
            if (freq == "w" || freq == "W" || freq == "m" || freq == "M") {
                LOG_INFO("尝试用日线数据聚合");
                
                auto dailyData = getDailyData(ts_code, start_date, end_date);
                if (!dailyData.empty()) {
                    if (freq == "w" || freq == "W") {
                        auto data = utils::KLineAggregator::aggregateToWeekly(dailyData);
                        LOG_INFO("聚合结果: " + std::to_string(data.size()) + " 条");
                        return data;
                    } else {
                        auto data = utils::KLineAggregator::aggregateToMonthly(dailyData);
                        LOG_INFO("聚合结果: " + std::to_string(data.size()) + " 条");
                        return data;
                    }
                }
            }
            
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取行情数据异常: " + ts_code + " - " + std::string(e.what()));
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

std::vector<core::FinancialIndicator> TushareDataSource::getFinancialIndicators(
    const std::string& ts_code,
    const std::string& period) {
    
    try {
        LOG_DEBUG("获取财务指标数据" + (ts_code.empty() ? "" : ": " + ts_code));
        
        std::map<std::string, std::string> params;
        if (!ts_code.empty()) {
            params["ts_code"] = ts_code;
        }
        if (!period.empty()) {
            params["period"] = period;
        }
        
        std::vector<std::string> fields = {
            "ts_code", "ann_date", "end_date",
            "pe", "pe_ttm", "pb", "ps", "pcf",
            "roe", "roe_ttm", "roa", "grossprofit_margin", "netprofit_margin", "operating_margin",
            "or_yoy", "op_yoy", "ebt_yoy", "netprofit_yoy", "tr_yoy",
            "debt_to_assets", "assets_turn", "current_ratio",
            "ocf_to_operatingprofit", "ocf_to_or"
        };
        
        auto response = client_->query("fina_indicator", params, fields);
        
        if (response.isSuccess()) {
            return parseFinancialIndicators(response);
        } else {
            LOG_ERROR("获取财务指标失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取财务指标异常: " + std::string(e.what()));
        return {};
    }
}

std::vector<MoneyFlow> TushareDataSource::getMoneyFlow(
    const std::string& ts_code,
    const std::string& trade_date,
    const std::string& start_date,
    const std::string& end_date) {
    
    try {
        LOG_DEBUG("获取资金流向数据" + 
                  (ts_code.empty() ? "" : ": " + ts_code) +
                  (trade_date.empty() ? "" : " 日期: " + trade_date));
        
        auto response = client_->getMoneyFlow(ts_code, trade_date, start_date, end_date);
        
        if (response.isSuccess()) {
            return parseMoneyFlow(response);
        } else {
            LOG_ERROR("获取资金流向失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取资金流向异常: " + std::string(e.what()));
        return {};
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
        int filteredCount = 0;
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
            
            if (shouldFilterStock(stock.ts_code, stock.name)) {
                filteredCount++;
                continue;
            }
            
            result.push_back(stock);
        }
        
        LOG_INFO("解析股票列表成功，共 " + std::to_string(result.size()) + " 条（过滤创业板/科创板/ST: " + std::to_string(filteredCount) + " 条）");
        
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
            
            // 向前插入
            result.insert(result.begin(), data);
        }

        // 打印最后一个的日期
        if (!result.empty()) {
            LOG_INFO("最后一个的日期: " + result.back().trade_date);
        }

        LOG_INFO("解析股票数据成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析股票数据失败: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<core::FinancialIndicator> TushareDataSource::parseFinancialIndicators(const TushareResponse& response) {
    std::vector<core::FinancialIndicator> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        auto get_double = [&](const json& item, const std::string& field_name) -> double {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        for (const auto& item : items) {
            core::FinancialIndicator indicator;
            
            indicator.ts_code = get_string(item, "ts_code");
            indicator.ann_date = get_string(item, "ann_date");
            indicator.end_date = get_string(item, "end_date");
            
            indicator.pe = get_double(item, "pe");
            indicator.pe_ttm = get_double(item, "pe_ttm");
            indicator.pb = get_double(item, "pb");
            indicator.ps = get_double(item, "ps");
            indicator.pcf = get_double(item, "pcf");
            
            indicator.roe = get_double(item, "roe");
            indicator.roe_ttm = get_double(item, "roe_ttm");
            indicator.roa = get_double(item, "roa");
            indicator.grossprofit_margin = get_double(item, "grossprofit_margin");
            indicator.netprofit_margin = get_double(item, "netprofit_margin");
            indicator.operating_margin = get_double(item, "operating_margin");
            
            indicator.or_yoy = get_double(item, "or_yoy");
            indicator.op_yoy = get_double(item, "op_yoy");
            indicator.ebt_yoy = get_double(item, "ebt_yoy");
            indicator.netprofit_yoy = get_double(item, "netprofit_yoy");
            indicator.tr_yoy = get_double(item, "tr_yoy");
            
            indicator.debt_to_assets = get_double(item, "debt_to_assets");
            indicator.assets_turn = get_double(item, "assets_turn");
            indicator.current_ratio = get_double(item, "current_ratio");
            
            indicator.ocf_to_operatingprofit = get_double(item, "ocf_to_operatingprofit");
            indicator.ocf_to_or = get_double(item, "ocf_to_or");
            
            result.push_back(indicator);
        }
        
        LOG_INFO("解析财务指标成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析财务指标失败: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<MoneyFlow> TushareDataSource::parseMoneyFlow(const TushareResponse& response) {
    std::vector<MoneyFlow> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        auto get_int = [&](const json& item, const std::string& field_name) -> int {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<int>();
            }
            return 0;
        };
        
        auto get_double = [&](const json& item, const std::string& field_name) -> double {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        for (const auto& item : items) {
            MoneyFlow flow;
            
            flow.ts_code = get_string(item, "ts_code");
            flow.trade_date = get_string(item, "trade_date");
            
            flow.buy_sm_vol = get_int(item, "buy_sm_vol");
            flow.buy_sm_amount = get_double(item, "buy_sm_amount");
            flow.sell_sm_vol = get_int(item, "sell_sm_vol");
            flow.sell_sm_amount = get_double(item, "sell_sm_amount");
            
            flow.buy_md_vol = get_int(item, "buy_md_vol");
            flow.buy_md_amount = get_double(item, "buy_md_amount");
            flow.sell_md_vol = get_int(item, "sell_md_vol");
            flow.sell_md_amount = get_double(item, "sell_md_amount");
            
            flow.buy_lg_vol = get_int(item, "buy_lg_vol");
            flow.buy_lg_amount = get_double(item, "buy_lg_amount");
            flow.sell_lg_vol = get_int(item, "sell_lg_vol");
            flow.sell_lg_amount = get_double(item, "sell_lg_amount");
            
            flow.buy_elg_vol = get_int(item, "buy_elg_vol");
            flow.buy_elg_amount = get_double(item, "buy_elg_amount");
            flow.sell_elg_vol = get_int(item, "sell_elg_vol");
            flow.sell_elg_amount = get_double(item, "sell_elg_amount");
            
            flow.net_mf_vol = get_int(item, "net_mf_vol");
            flow.net_mf_amount = get_double(item, "net_mf_amount");
            
            result.push_back(flow);
        }
        
        LOG_INFO("解析资金流向成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析资金流向失败: " + std::string(e.what()));
    }
    
    return result;
}

// ========== 停复牌/业绩预告/快报/分红数据 ==========

std::vector<SuspendInfo> TushareDataSource::getSuspendInfo(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date) {
    
    try {
        LOG_DEBUG("获取停复牌信息" + 
                  (ts_code.empty() ? "" : ": " + ts_code) +
                  (start_date.empty() ? "" : " [" + start_date + " - " + end_date + "]"));
        
        auto response = client_->getSuspendD(ts_code, "", "", start_date, end_date);
        
        if (response.isSuccess()) {
            return parseSuspendInfo(response);
        } else {
            LOG_ERROR("获取停复牌信息失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取停复牌信息异常: " + std::string(e.what()));
        return {};
    }
}

std::vector<Forecast> TushareDataSource::getForecast(
    const std::string& ts_code,
    const std::string& period,
    const std::string& start_date,
    const std::string& end_date) {
    
    try {
        LOG_DEBUG("获取业绩预告" + 
                  (ts_code.empty() ? "" : ": " + ts_code) +
                  (period.empty() ? "" : " 报告期: " + period));
        
        auto response = client_->getForecast(ts_code, period, start_date, end_date);
        
        if (response.isSuccess()) {
            return parseForecast(response);
        } else {
            LOG_ERROR("获取业绩预告失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取业绩预告异常: " + std::string(e.what()));
        return {};
    }
}

std::vector<Express> TushareDataSource::getExpress(
    const std::string& ts_code,
    const std::string& period,
    const std::string& start_date,
    const std::string& end_date) {
    
    try {
        LOG_DEBUG("获取业绩快报" + 
                  (ts_code.empty() ? "" : ": " + ts_code) +
                  (period.empty() ? "" : " 报告期: " + period));
        
        auto response = client_->getExpress(ts_code, period, start_date, end_date);
        
        if (response.isSuccess()) {
            return parseExpress(response);
        } else {
            LOG_ERROR("获取业绩快报失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取业绩快报异常: " + std::string(e.what()));
        return {};
    }
}

std::vector<Dividend> TushareDataSource::getDividend(
    const std::string& ts_code,
    const std::string& end_date,
    const std::string& start_date,
    const std::string& ann_date) {
    
    try {
        LOG_DEBUG("获取分红送股数据" + 
                  (ts_code.empty() ? "" : ": " + ts_code) +
                  (end_date.empty() ? "" : " 年度: " + end_date));
        
        auto response = client_->getDividend(ts_code, end_date, start_date, ann_date);
        
        if (response.isSuccess()) {
            return parseDividend(response);
        } else {
            LOG_ERROR("获取分红送股数据失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取分红送股数据异常: " + std::string(e.what()));
        return {};
    }
}

// ========== 解析方法 ==========

std::vector<SuspendInfo> TushareDataSource::parseSuspendInfo(const TushareResponse& response) {
    std::vector<SuspendInfo> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        for (const auto& item : items) {
            SuspendInfo info;
            
            info.ts_code = get_string(item, "ts_code");
            info.suspend_date = get_string(item, "suspend_date");
            info.resume_date = get_string(item, "resume_date");
            info.suspend_type = get_string(item, "suspend_type");
            info.suspend_reason = get_string(item, "suspend_reason");
            
            result.push_back(info);
        }
        
        LOG_INFO("解析停复牌信息成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析停复牌信息失败: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<Forecast> TushareDataSource::parseForecast(const TushareResponse& response) {
    std::vector<Forecast> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        auto get_double = [&](const json& item, const std::string& field_name) -> double {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                if (item[it->second].is_string()) {
                    std::string str_val = item[it->second].get<std::string>();
                    try {
                        return std::stod(str_val);
                    } catch (...) {
                        return 0.0;
                    }
                }
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        for (const auto& item : items) {
            Forecast forecast;
            
            forecast.ts_code = get_string(item, "ts_code");
            forecast.ann_date = get_string(item, "ann_date");
            forecast.end_date = get_string(item, "end_date");
            forecast.type = get_string(item, "type");
            forecast.p_change_min = get_double(item, "p_change_min");
            forecast.p_change_max = get_double(item, "p_change_max");
            forecast.net_profit_min = get_double(item, "net_profit_min");
            forecast.net_profit_max = get_double(item, "net_profit_max");
            forecast.summary = get_string(item, "summary");
            forecast.change_reason = get_string(item, "change_reason");
            
            result.push_back(forecast);
        }
        
        LOG_INFO("解析业绩预告成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析业绩预告失败: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<Express> TushareDataSource::parseExpress(const TushareResponse& response) {
    std::vector<Express> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        auto get_double = [&](const json& item, const std::string& field_name) -> double {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                if (item[it->second].is_string()) {
                    std::string str_val = item[it->second].get<std::string>();
                    try {
                        return std::stod(str_val);
                    } catch (...) {
                        return 0.0;
                    }
                }
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        for (const auto& item : items) {
            Express express;
            
            express.ts_code = get_string(item, "ts_code");
            express.ann_date = get_string(item, "ann_date");
            express.end_date = get_string(item, "end_date");
            express.revenue = get_double(item, "revenue");
            express.operate_profit = get_double(item, "operate_profit");
            express.total_profit = get_double(item, "total_profit");
            express.n_income = get_double(item, "n_income");
            express.total_assets = get_double(item, "total_assets");
            express.total_hldr_eqy_exc_min_int = get_double(item, "total_hldr_eqy_exc_min_int");
            express.total_share = get_double(item, "total_share");
            express.eps = get_double(item, "eps");
            
            result.push_back(express);
        }
        
        LOG_INFO("解析业绩快报成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析业绩快报失败: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<Dividend> TushareDataSource::parseDividend(const TushareResponse& response) {
    std::vector<Dividend> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        auto get_double = [&](const json& item, const std::string& field_name) -> double {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                if (item[it->second].is_string()) {
                    std::string str_val = item[it->second].get<std::string>();
                    try {
                        return std::stod(str_val);
                    } catch (...) {
                        return 0.0;
                    }
                }
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        for (const auto& item : items) {
            Dividend dividend;
            
            dividend.ts_code = get_string(item, "ts_code");
            dividend.ann_date = get_string(item, "ann_date");
            dividend.end_date = get_string(item, "end_date");
            dividend.div_proc = get_string(item, "div_proc");
            dividend.stk_div = get_double(item, "stk_div");
            dividend.stk_bo_rate = get_double(item, "stk_bo_rate");
            dividend.cash_div = get_double(item, "cash_div");
            dividend.cash_div_tax = get_double(item, "cash_div_tax");
            dividend.record_date = get_string(item, "record_date");
            dividend.ex_date = get_string(item, "ex_date");
            dividend.pay_date = get_string(item, "pay_date");
            dividend.div_listdate = get_string(item, "div_listdate");
            
            result.push_back(dividend);
        }
        
        LOG_INFO("解析分红送股数据成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析分红送股数据失败: " + std::string(e.what()));
    }
    
    return result;
}

} // namespace network

