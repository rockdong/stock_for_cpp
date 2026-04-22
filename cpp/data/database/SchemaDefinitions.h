#ifndef DATA_DATABASE_SCHEMA_DEFINITIONS_H
#define DATA_DATABASE_SCHEMA_DEFINITIONS_H

#include "DatabaseTypes.h"

namespace data {
namespace schema {

using ColumnList = std::vector<ColumnDefinition>;
using IndexList = std::vector<IndexDefinition>;

const ColumnList STOCKS_COLUMNS = {
    {"id", DataType::INTEGER, true, true},
    {"ts_code", DataType::TEXT, false, false, 20, false, "", false, true, true},
    {"symbol", DataType::TEXT},
    {"name", DataType::TEXT},
    {"area", DataType::TEXT},
    {"industry", DataType::TEXT},
    {"fullname", DataType::TEXT},
    {"enname", DataType::TEXT},
    {"cnspell", DataType::TEXT},
    {"market", DataType::TEXT},
    {"exchange", DataType::TEXT},
    {"curr_type", DataType::TEXT},
    {"list_status", DataType::TEXT},
    {"list_date", DataType::TEXT},
    {"delist_date", DataType::TEXT},
    {"is_hs", DataType::TEXT},
    {"created_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"},
    {"updated_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"}
};

const IndexList STOCKS_INDEXES = {
    {"idx_stocks_ts_code", "stocks", {"ts_code"}, false}
};

const TableDefinition STOCKS_TABLE = {"stocks", STOCKS_COLUMNS, STOCKS_INDEXES};

const ColumnList PRICES_COLUMNS = {
    {"id", DataType::INTEGER, true, true},
    {"ts_code", DataType::TEXT, false, false, 0, false},
    {"trade_date", DataType::TEXT, false, false, 0, false},
    {"open", DataType::REAL},
    {"high", DataType::REAL},
    {"low", DataType::REAL},
    {"close", DataType::REAL},
    {"pre_close", DataType::REAL},
    {"change_pct", DataType::REAL},
    {"volume", DataType::BIGINT},
    {"amount", DataType::REAL},
    {"created_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"}
};

const IndexList PRICES_INDEXES = {
    {"idx_prices_ts_code", "prices", {"ts_code"}, false},
    {"idx_prices_trade_date", "prices", {"trade_date"}, false},
    {"idx_prices_lookup", "prices", {"ts_code", "trade_date"}, false}
};

const TableDefinition PRICES_TABLE = {"prices", PRICES_COLUMNS, PRICES_INDEXES};

const ColumnList TRADES_COLUMNS = {
    {"id", DataType::INTEGER, true, true},
    {"ts_code", DataType::TEXT, false, false, 0, false},
    {"trade_type", DataType::TEXT, false, false, 0, false},
    {"price", DataType::REAL, false, false, 0, false},
    {"quantity", DataType::INTEGER, false, false, 0, false},
    {"amount", DataType::REAL, false, false, 0, false},
    {"trade_time", DataType::DATETIME, false, false, 0, false},
    {"created_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"}
};

const IndexList TRADES_INDEXES = {
    {"idx_trades_ts_code", "trades", {"ts_code"}, false},
    {"idx_trades_trade_time", "trades", {"trade_time"}, false}
};

const TableDefinition TRADES_TABLE = {"trades", TRADES_COLUMNS, TRADES_INDEXES};

const ColumnList ANALYSIS_RESULTS_COLUMNS = {
    {"id", DataType::INTEGER, true, true},
    {"ts_code", DataType::TEXT, false, false, 0, false},
    {"strategy_name", DataType::TEXT, false, false, 0, false},
    {"trade_date", DataType::TEXT, false, false, 0, false},
    {"label", DataType::TEXT, false, false, 0, false},
    {"opt", DataType::TEXT},
    {"freq", DataType::TEXT},
    {"created_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"},
    {"updated_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"}
};

const IndexList ANALYSIS_RESULTS_INDEXES = {
    {"idx_analysis_ts_code", "analysis_results", {"ts_code"}, false},
    {"idx_analysis_strategy", "analysis_results", {"strategy_name"}, false},
    {"idx_analysis_date", "analysis_results", {"trade_date"}, false},
    {"idx_analysis_label", "analysis_results", {"label"}, false}
};

const TableDefinition ANALYSIS_RESULTS_TABLE = {"analysis_results", ANALYSIS_RESULTS_COLUMNS, ANALYSIS_RESULTS_INDEXES};

const ColumnList ANALYSIS_PROGRESS_COLUMNS = {
    {"id", DataType::INTEGER, true, false, 0, false, "", false, false, false, "id = 1"},
    {"phase1_status", DataType::TEXT, false, false, 0, true, "'idle'"},
    {"phase1_total", DataType::INTEGER, false, false, 0, true, "0"},
    {"phase1_completed", DataType::INTEGER, false, false, 0, true, "0"},
    {"phase1_qualified", DataType::INTEGER, false, false, 0, true, "0"},
    {"phase2_status", DataType::TEXT, false, false, 0, true, "'idle'"},
    {"phase2_total", DataType::INTEGER, false, false, 0, true, "0"},
    {"phase2_completed", DataType::INTEGER, false, false, 0, true, "0"},
    {"phase2_failed", DataType::INTEGER, false, false, 0, true, "0"},
    {"started_at", DataType::DATETIME},
    {"phase1_completed_at", DataType::DATETIME},
    {"updated_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"}
};

const TableDefinition ANALYSIS_PROGRESS_TABLE = {"analysis_progress", ANALYSIS_PROGRESS_COLUMNS};

const ColumnList CHART_DATA_COLUMNS = {
    {"id", DataType::INTEGER, true, true},
    {"ts_code", DataType::TEXT, false, false, 0, false},
    {"freq", DataType::TEXT, false, false, 0, false},
    {"analysis_date", DataType::TEXT, false, false, 0, false},
    {"data", DataType::TEXT, false, false, 0, false},
    {"updated_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"}
};

const IndexList CHART_DATA_INDEXES = {
    {"idx_chart_lookup", "chart_data", {"ts_code", "freq", "analysis_date"}, false}
};

const TableDefinition CHART_DATA_TABLE = {"chart_data", CHART_DATA_COLUMNS, CHART_DATA_INDEXES};

const ColumnList ANALYSIS_PROCESS_RECORDS_COLUMNS = {
    {"id", DataType::INTEGER, true, true},
    {"ts_code", DataType::TEXT, false, false, 0, false},
    {"stock_name", DataType::TEXT},
    {"trade_date", DataType::TEXT, false, false, 0, false},
    {"data", DataType::TEXT, false, false, 0, false, "'{\"strategies\":[]}'"},
    {"created_at", DataType::DATETIME, false, false, 0, true, "CURRENT_TIMESTAMP"},
    {"expires_at", DataType::DATETIME}
};

const IndexList ANALYSIS_PROCESS_RECORDS_INDEXES = {
    {"idx_process_ts_code", "analysis_process_records", {"ts_code"}, false},
    {"idx_process_trade_date", "analysis_process_records", {"trade_date"}, false}
};

const TableDefinition ANALYSIS_PROCESS_RECORDS_TABLE = {"analysis_process_records", ANALYSIS_PROCESS_RECORDS_COLUMNS, ANALYSIS_PROCESS_RECORDS_INDEXES};

const std::vector<TableDefinition> ALL_TABLES = {
    STOCKS_TABLE,
    PRICES_TABLE,
    TRADES_TABLE,
    ANALYSIS_RESULTS_TABLE,
    ANALYSIS_PROGRESS_TABLE,
    CHART_DATA_TABLE,
    ANALYSIS_PROCESS_RECORDS_TABLE
};

} // namespace schema
} // namespace data

#endif // DATA_DATABASE_SCHEMA_DEFINITIONS_H