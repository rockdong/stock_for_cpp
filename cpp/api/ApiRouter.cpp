#include "ApiRouter.h"
#include "../thirdparty/cpp-httplib/httplib.h"
#include "../thirdparty/json/include/nlohmann/json.hpp"
#include "../log/Logger.h"
#include <sqlite3.h>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <mutex>

namespace api {

using json = nlohmann::json;

static std::string getDbPath() {
    const char* dbPath = std::getenv("DB_PATH");
    return dbPath ? std::string(dbPath) : "/app/stock.db";
}

static sqlite3* g_db = nullptr;
static std::mutex g_dbMutex;

static sqlite3* getDb() {
    std::lock_guard<std::mutex> lock(g_dbMutex);
    if (!g_db) {
        if (sqlite3_open(getDbPath().c_str(), &g_db) != SQLITE_OK) {
            LOG_ERROR("无法打开数据库: " + std::string(sqlite3_errmsg(g_db)));
            return nullptr;
        }
        sqlite3_busy_timeout(g_db, 5000);
    }
    return g_db;
}

static void closeDb() {
    std::lock_guard<std::mutex> lock(g_dbMutex);
    if (g_db) {
        sqlite3_close(g_db);
        g_db = nullptr;
    }
}

std::string jsonSuccess(const std::string& data) {
    json res;
    res["success"] = true;
    res["data"] = json::parse(data);
    return res.dump();
}

std::string jsonError(const std::string& error) {
    json res;
    res["success"] = false;
    res["error"] = error;
    return res.dump();
}

std::string getStrategies() {
    const char* strategiesEnv = std::getenv("STRATEGIES");
    if (!strategiesEnv) return jsonSuccess("[]");
    
    std::string env(strategiesEnv);
    json strategies = json::array();
    
    std::stringstream ss(env);
    std::string item;
    while (std::getline(ss, item, ';')) {
        item.erase(std::remove_if(item.begin(), item.end(), ::isspace), item.end());
        if (!item.empty()) strategies.push_back(item);
    }
    
    return jsonSuccess(strategies.dump());
}

std::string getStocks(int limit) {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json stocks = json::array();
    std::string sql = "SELECT ts_code, name, industry, market FROM stocks LIMIT " + std::to_string(limit);
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json stock;
            stock["ts_code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            stock["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            stock["industry"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            stock["market"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            stocks.push_back(stock);
        }
        sqlite3_finalize(stmt);
    }
    
    return jsonSuccess(stocks.dump());
}

std::string searchStocks(const std::string& keyword) {
    if (keyword.empty()) return jsonSuccess("[]");
    
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json stocks = json::array();
    std::string sql = "SELECT ts_code, name, industry, market FROM stocks WHERE name LIKE ? OR ts_code LIKE ? LIMIT 20";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string pattern = "%" + keyword + "%";
        sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json stock;
            stock["ts_code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            stock["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            stock["industry"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            stock["market"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            stocks.push_back(stock);
        }
        sqlite3_finalize(stmt);
    }
    
    return jsonSuccess(stocks.dump());
}

std::string getStockByCode(const std::string& code) {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json stock;
    std::string sql = "SELECT ts_code, name, industry, market FROM stocks WHERE ts_code = ?";
    
    sqlite3_stmt* stmt;
    bool found = false;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            stock["ts_code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            stock["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            stock["industry"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            stock["market"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        }
        sqlite3_finalize(stmt);
    }
    
    if (found) {
        std::string priceSql = "SELECT close, high, low, volume FROM chart_data WHERE ts_code = ? ORDER BY time DESC LIMIT 1";
        if (sqlite3_prepare_v2(db, priceSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                stock["price"] = sqlite3_column_double(stmt, 0);
                stock["high"] = sqlite3_column_double(stmt, 1);
                stock["low"] = sqlite3_column_double(stmt, 2);
                stock["volume"] = sqlite3_column_double(stmt, 3);
            }
            sqlite3_finalize(stmt);
        }
    }
    
    if (!found) return jsonError("股票不存在");
    return jsonSuccess(stock.dump());
}

std::string getAnalysisSignals(const std::string& label, int limit) {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json results = json::array();
    std::string sql;
    if (label.empty()) {
        sql = "SELECT a.ts_code, s.name, a.strategy_name, a.label, a.freq, a.trade_date "
              "FROM analysis_results a LEFT JOIN stocks s ON a.ts_code = s.ts_code "
              "ORDER BY a.trade_date DESC LIMIT ?";
    } else {
        sql = "SELECT a.ts_code, s.name, a.strategy_name, a.label, a.freq, a.trade_date "
              "FROM analysis_results a LEFT JOIN stocks s ON a.ts_code = s.ts_code "
              "WHERE a.label = ? ORDER BY a.trade_date DESC LIMIT ?";
    }
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (!label.empty()) {
            sqlite3_bind_text(stmt, 1, label.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 2, limit);
        } else {
            sqlite3_bind_int(stmt, 1, limit);
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json row;
            row["ts_code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            row["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            row["strategy_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            row["label"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            row["freq"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            row["trade_date"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            results.push_back(row);
        }
        sqlite3_finalize(stmt);
    }
    
    return jsonSuccess(results.dump());
}

std::string getAnalysisProgress() {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json progress;
    progress["id"] = 1;
    progress["total"] = 0;
    progress["completed"] = 0;
    progress["failed"] = 0;
    progress["status"] = "idle";
    
    std::string sql = "SELECT * FROM analysis_progress ORDER BY id DESC LIMIT 1";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            progress["id"] = sqlite3_column_int(stmt, 0);
            progress["total"] = sqlite3_column_int(stmt, 1);
            progress["completed"] = sqlite3_column_int(stmt, 2);
            progress["failed"] = sqlite3_column_int(stmt, 3);
            progress["status"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            progress["started_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            progress["updated_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        }
        sqlite3_finalize(stmt);
    }
    
    return jsonSuccess(progress.dump());
}

std::string getAnalysisProcess(const std::map<std::string, std::string>& params) {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json results = json::array();
    int limit = params.count("limit") ? std::stoi(params.at("limit")) : 100;
    
    std::string sql = "SELECT id, ts_code, stock_name, trade_date, data, created_at FROM analysis_process_records WHERE 1=1";
    
    if (params.count("ts_code")) {
        sql += " AND ts_code = '" + params.at("ts_code") + "'";
    }
    if (params.count("start_date")) {
        sql += " AND trade_date >= '" + params.at("start_date") + "'";
    }
    if (params.count("end_date")) {
        sql += " AND trade_date <= '" + params.at("end_date") + "'";
    }
    
    sql += " ORDER BY trade_date DESC, created_at DESC LIMIT " + std::to_string(limit);
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json row;
            row["id"] = sqlite3_column_int(stmt, 0);
            row["ts_code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            row["stock_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            row["trade_date"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            
            const char* dataStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            if (dataStr) {
                try {
                    row["data"] = json::parse(dataStr);
                } catch (...) {
                    row["data"] = json::object();
                }
            } else {
                row["data"] = json::object();
            }
            
            row["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            results.push_back(row);
        }
        sqlite3_finalize(stmt);
    }
    
    return jsonSuccess(results.dump());
}

std::string getAnalysisProcessById(int id) {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json result;
    std::string sql = "SELECT id, ts_code, stock_name, trade_date, data, created_at FROM analysis_process_records WHERE id = ?";
    
    sqlite3_stmt* stmt;
    bool found = false;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            result["id"] = sqlite3_column_int(stmt, 0);
            result["ts_code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            result["stock_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            result["trade_date"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            
            const char* dataStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            if (dataStr) {
                try {
                    result["data"] = json::parse(dataStr);
                } catch (...) {
                    result["data"] = json::object();
                }
            } else {
                result["data"] = json::object();
            }
            
            result["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        }
        sqlite3_finalize(stmt);
    }
    
    if (!found) return jsonError("记录不存在");
    return jsonSuccess(result.dump());
}

std::string getAnalysisProcessChart(const std::string& tsCode, const std::string& strategy, const std::string& freq) {
    sqlite3* db = getDb();
    if (!db) return jsonSuccess("[]");
    
    std::string sql = "SELECT data FROM analysis_process_records WHERE ts_code = ? ORDER BY trade_date DESC LIMIT 1";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tsCode.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* dataStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (dataStr) {
                try {
                    json data = json::parse(dataStr);
                    json candles = json::array();
                    
                    if (data.contains("strategies") && data["strategies"].is_array()) {
                        for (const auto& s : data["strategies"]) {
                            std::string sName = s.contains("name") ? s["name"].get<std::string>() : "";
                            if (strategy.empty() || sName.find(strategy) != std::string::npos) {
                                if (s.contains("freqs") && s["freqs"].is_array()) {
                                    for (const auto& f : s["freqs"]) {
                                        if (f.contains("freq") && f["freq"].get<std::string>() == freq) {
                                            if (f.contains("candles")) {
                                                candles = f["candles"];
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    sqlite3_finalize(stmt);
                    return jsonSuccess(candles.dump());
                } catch (...) {}
            }
        }
        sqlite3_finalize(stmt);
    }
    
    return jsonSuccess("[]");
}

std::string getAnalysisByCode(const std::string& code) {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json result;
    std::string sql = "SELECT a.ts_code, s.name, a.strategy_name, a.label, a.freq, a.trade_date "
                      "FROM analysis_results a LEFT JOIN stocks s ON a.ts_code = s.ts_code "
                      "WHERE a.ts_code = ? ORDER BY a.trade_date DESC LIMIT 1";
    
    sqlite3_stmt* stmt;
    bool found = false;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            result["ts_code"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            result["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            result["strategy_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            result["label"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            result["freq"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            result["trade_date"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        }
        sqlite3_finalize(stmt);
    }
    
    if (!found) return jsonError("无分析结果");
    return jsonSuccess(result.dump());
}

std::string getCharts(const std::string& code, const std::string& freq, int limit) {
    sqlite3* db = getDb();
    if (!db) return jsonError("数据库连接失败");
    
    json charts = json::array();
    std::string sql = "SELECT time, open, high, low, close, volume FROM chart_data "
                      "WHERE ts_code = ? AND freq = ? ORDER BY time DESC LIMIT ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, freq.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, limit);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json candle;
            candle["time"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            candle["open"] = sqlite3_column_double(stmt, 1);
            candle["high"] = sqlite3_column_double(stmt, 2);
            candle["low"] = sqlite3_column_double(stmt, 3);
            candle["close"] = sqlite3_column_double(stmt, 4);
            candle["volume"] = sqlite3_column_double(stmt, 5);
            charts.push_back(candle);
        }
        sqlite3_finalize(stmt);
    }
    
    std::reverse(charts.begin(), charts.end());
    return jsonSuccess(charts.dump());
}

void setupRoutes(httplib::Server& server) {
    server.set_keep_alive_timeout(30);
    server.set_read_timeout(30);
    server.set_write_timeout(30);
    
    server.Get("/api/stocks", [](const httplib::Request& req, httplib::Response& res) {
        int limit = req.has_param("limit") ? std::stoi(req.get_param_value("limit")) : 100;
        res.set_content(getStocks(limit), "application/json");
    });
    
    server.Get("/api/stocks/search", [](const httplib::Request& req, httplib::Response& res) {
        std::string keyword = req.has_param("keyword") ? req.get_param_value("keyword") : "";
        res.set_content(searchStocks(keyword), "application/json");
    });
    
    server.Get(R"(/api/stocks/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string code = req.matches[1];
        res.set_content(getStockByCode(code), "application/json");
    });
    
    server.Get("/api/analysis/signals", [](const httplib::Request& req, httplib::Response& res) {
        std::string label = req.has_param("label") ? req.get_param_value("label") : "";
        int limit = req.has_param("limit") ? std::stoi(req.get_param_value("limit")) : 50;
        res.set_content(getAnalysisSignals(label, limit), "application/json");
    });
    
    server.Get("/api/analysis/progress", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(getAnalysisProgress(), "application/json");
    });
    
    server.Get("/api/analysis/process", [](const httplib::Request& req, httplib::Response& res) {
        std::map<std::string, std::string> params;
        if (req.has_param("ts_code")) params["ts_code"] = req.get_param_value("ts_code");
        if (req.has_param("start_date")) params["start_date"] = req.get_param_value("start_date");
        if (req.has_param("end_date")) params["end_date"] = req.get_param_value("end_date");
        if (req.has_param("limit")) params["limit"] = req.get_param_value("limit");
        res.set_content(getAnalysisProcess(params), "application/json");
    });
    
    server.Get("/api/analysis/process/strategies", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(getStrategies(), "application/json");
    });
    
    server.Get(R"(/api/analysis/process/chart/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string tsCode = req.matches[1];
        std::string strategy = req.has_param("strategy") ? req.get_param_value("strategy") : "";
        std::string freq = req.has_param("freq") ? req.get_param_value("freq") : "d";
        res.set_content(getAnalysisProcessChart(tsCode, strategy, freq), "application/json");
    });
    
    server.Get(R"(/api/analysis/process/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1]);
        res.set_content(getAnalysisProcessById(id), "application/json");
    });
    
    server.Get(R"(/api/analysis/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string code = req.matches[1];
        res.set_content(getAnalysisByCode(code), "application/json");
    });
    
    server.Get(R"(/api/charts/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string code = req.matches[1];
        std::string freq = req.has_param("freq") ? req.get_param_value("freq") : "d";
        int limit = req.has_param("limit") ? std::stoi(req.get_param_value("limit")) : 100;
        res.set_content(getCharts(code, freq, limit), "application/json");
    });
    
    server.Get("/api/user/watchlist", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(jsonSuccess("[]"), "application/json");
    });
}

void cleanupDatabase() {
    closeDb();
}

} // namespace api