#!/bin/bash

# Stock Analysis System - 数据库初始化脚本
# 版本: v1.0.0
# 创建日期: 2026-06-16

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印函数
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查 sqlite3 是否安装
check_sqlite3() {
    if ! command -v sqlite3 &> /dev/null; then
        print_error "sqlite3 未安装"
        print_info "请安装 sqlite3: brew install sqlite3 (macOS) 或 apt install sqlite3 (Linux)"
        exit 1
    fi
    print_success "sqlite3 已安装: $(sqlite3 --version)"
}

# 创建数据目录
create_data_dir() {
    DATA_DIR="data"
    if [ ! -d "$DATA_DIR" ]; then
        print_info "创建数据目录: $DATA_DIR"
        mkdir -p "$DATA_DIR"
        print_success "数据目录已创建"
    else
        print_info "数据目录已存在: $DATA_DIR"
    fi
}

# 创建数据库文件
create_database() {
    DB_FILE="$DATA_DIR/stock.db"
    
    if [ -f "$DB_FILE" ]; then
        print_warning "数据库文件已存在: $DB_FILE"
        read -p "是否重新初始化数据库？(y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "跳过数据库初始化"
            return
        fi
        print_info "删除旧数据库文件"
        rm -f "$DB_FILE"
    fi
    
    print_info "创建数据库文件: $DB_FILE"
    
    # 创建表结构
    sqlite3 "$DB_FILE" <<EOF
-- 股票基础信息表
CREATE TABLE IF NOT EXISTS stocks (
    ts_code TEXT PRIMARY KEY,
    symbol TEXT NOT NULL,
    name TEXT NOT NULL,
    industry TEXT,
    market TEXT,
    list_date TEXT,
    delist_date TEXT,
    is_active INTEGER DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 分析结果表
CREATE TABLE IF NOT EXISTS analysis_results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,
    analysis_date TEXT NOT NULL,
    strategy_name TEXT NOT NULL,
    result TEXT,
    confidence REAL,
    is_positive INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (ts_code) REFERENCES stocks(ts_code)
);

-- 价格数据表
CREATE TABLE IF NOT EXISTS price_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,
    trade_date TEXT NOT NULL,
    open REAL,
    high REAL,
    low REAL,
    close REAL,
    pre_close REAL,
    change REAL,
    pct_chg REAL,
    vol REAL,
    amount REAL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (ts_code) REFERENCES stocks(ts_code),
    UNIQUE(ts_code, trade_date)
);

-- 技术指标表
CREATE TABLE IF NOT EXISTS technical_indicators (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,
    trade_date TEXT NOT NULL,
    indicator_type TEXT NOT NULL,
    indicator_value REAL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (ts_code) REFERENCES stocks(ts_code),
    UNIQUE(ts_code, trade_date, indicator_type)
);

-- 系统配置表
CREATE TABLE IF NOT EXISTS system_config (
    key TEXT PRIMARY KEY,
    value TEXT,
    description TEXT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 日志表
CREATE TABLE IF NOT EXISTS logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    level TEXT NOT NULL,
    message TEXT NOT NULL,
    source TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引
CREATE INDEX IF NOT EXISTS idx_stocks_symbol ON stocks(symbol);
CREATE INDEX IF NOT EXISTS idx_stocks_market ON stocks(market);
CREATE INDEX IF NOT EXISTS idx_stocks_active ON stocks(is_active);

CREATE INDEX IF NOT EXISTS idx_analysis_ts_code ON analysis_results(ts_code);
CREATE INDEX IF NOT EXISTS idx_analysis_date ON analysis_results(analysis_date);
CREATE INDEX IF NOT EXISTS idx_analysis_strategy ON analysis_results(strategy_name);

CREATE INDEX IF NOT EXISTS idx_price_ts_code ON price_data(ts_code);
CREATE INDEX IF NOT EXISTS idx_price_date ON price_data(trade_date);

CREATE INDEX IF NOT EXISTS idx_indicator_ts_code ON technical_indicators(ts_code);
CREATE INDEX IF NOT EXISTS idx_indicator_date ON technical_indicators(trade_date);
CREATE INDEX IF NOT EXISTS idx_indicator_type ON technical_indicators(indicator_type);

-- 插入默认配置
INSERT INTO system_config (key, value, description) VALUES
    ('version', '1.0.0', '数据库版本'),
    ('created_at', datetime('now'), '创建时间'),
    ('last_analysis_date', '', '最后一次分析日期');
EOF
    
    print_success "数据库初始化完成"
    
    # 验证表创建
    TABLES=$(sqlite3 "$DB_FILE" ".tables")
    print_info "已创建的表: $TABLES"
    
    # 显示数据库大小
    DB_SIZE=$(ls -lh "$DB_FILE" | awk '{print $5}')
    print_success "数据库大小: $DB_SIZE"
}

# 添加示例数据（可选）
add_sample_data() {
    print_info "是否添加示例数据？"
    read -p "添加示例股票数据？(y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        return
    fi
    
    DB_FILE="$DATA_DIR/stock.db"
    
    print_info "添加示例数据..."
    
    # 添加示例股票
    sqlite3 "$DB_FILE" <<EOF
INSERT INTO stocks (ts_code, symbol, name, industry, market) VALUES
    ('000001.SZ', '000001', '平安银行', '银行', '深市主板'),
    ('000002.SZ', '000002', '万科A', '房地产', '深市主板'),
    ('600000.SH', '600000', '浦发银行', '银行', '沪市主板'),
    ('600036.SH', '600036', '招商银行', '银行', '沪市主板'),
    ('600519.SH', '600519', '贵州茅台', '食品饮料', '沪市主板');
EOF
    
    print_success "已添加 5 个示例股票"
    
    # 查询示例数据
    STOCKS=$(sqlite3 "$DB_FILE" "SELECT ts_code, name FROM stocks LIMIT 5")
    print_info "示例股票:"
    echo "$STOCKS"
}

# 显示数据库信息
show_database_info() {
    DB_FILE="$DATA_DIR/stock.db"
    
    print_info "========== 数据库信息 =========="
    print_info "数据库路径: $DB_FILE"
    print_info "数据库大小: $(ls -lh "$DB_FILE" | awk '{print $5}')"
    
    print_info "表结构:"
    sqlite3 "$DB_FILE" ".tables"
    
    print_info "股票数量: $(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM stocks")"
    print_info "分析记录: $(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM analysis_results")"
    
    print_info "数据库版本: $(sqlite3 "$DB_FILE" "SELECT value FROM system_config WHERE key='version'")"
    print_info "=================================="
}

# 主函数
main() {
    print_info "========== Stock Analysis System 数据库初始化 =========="
    print_info "开始时间: $(date '+%Y-%m-%d %H:%M:%S')"
    
    # 1. 检查 sqlite3
    check_sqlite3
    
    # 2. 创建数据目录
    create_data_dir
    
    # 3. 创建数据库
    create_database
    
    # 4. 添加示例数据（可选）
    add_sample_data
    
    # 5. 显示数据库信息
    show_database_info
    
    print_success "数据库初始化完成！"
    print_info "结束时间: $(date '+%Y-%m-%d %H:%M:%S')"
    
    print_info "下一步："
    print_info "1. 配置 API 密钥: export DATA_SOURCE_API_KEY=your_api_key"
    print_info "2. 运行客户端: cd build && ./stock_exe/stock_exe --once"
    print_info "3. 查看日志: tail -f logs/app.log"
}

# 执行主函数
main