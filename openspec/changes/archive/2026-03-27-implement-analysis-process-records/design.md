## Context

**当前状态**:
- `analysis_process_records` 表已创建，结构完整
- C++ 策略引擎在检测到信号时只写入 `analysis_results` 表（最终信号）
- 策略分析过程中计算的 K 线数据、EMA、MACD、RSI 等指标未保存

**约束**:
- 使用现有 sqlpp11 ORM 框架
- JSON 序列化使用 nlohmann/json 库
- 不影响现有分析流程性能
- 数据保留 7 天（由数据库触发器自动过期）

## Goals / Non-Goals

**Goals:**
- 实现过程数据的持久化存储
- 保留最近 10 个周期的 K 线和技术指标
- 支持按股票、策略、日期、信号查询
- 与现有 AnalysisResultDAO 保持一致的代码风格

**Non-Goals:**
- 不修改 Web 前端代码（API 已实现）
- 不修改数据库表结构
- 不实现历史数据的回填

## Decisions

### 1. 数据模型设计

**决定**: 创建 `AnalysisProcessRecord` 结构体，包含 JSON 序列化方法

```cpp
struct ProcessDataPoint {
    std::string time;      // 日期
    double open, high, low, close;
    int64_t volume;
    std::optional<double> ema17, ema25, macd, rsi;
};

struct AnalysisProcessRecord {
    int id = 0;
    std::string ts_code;
    std::string stock_name;
    std::string strategy_name;
    std::string trade_date;
    std::string freq = "d";
    std::string signal = "NONE";
    std::vector<ProcessDataPoint> data;  // 10 条数据
    std::string created_at;
    std::string expires_at;
};
```

**备选方案**:
- ❌ 扁平化字段：需要修改表结构，影响现有 API
- ✅ JSON 数组：灵活，无需改表，已设计好

### 2. DAO 层设计

**决定**: 参考 `AnalysisResultDAO` 创建 `AnalysisProcessRecordDAO`

**关键方法**:
```cpp
class AnalysisProcessRecordDAO {
public:
    bool insert(const AnalysisProcessRecord& record);
    bool upsert(const AnalysisProcessRecord& record);  // 使用 UNIQUE 约束
    std::vector<AnalysisProcessRecord> findByTsCode(const std::string& ts_code);
    std::vector<AnalysisProcessRecord> findByStrategy(const std::string& strategy);
    std::optional<AnalysisProcessRecord> findLatest(const std::string& ts_code, 
                                                     const std::string& strategy);
};
```

### 3. 策略集成方式

**决定**: 在 `Strategy::analyze()` 返回前，调用 DAO 保存过程数据

**修改位置**: `cpp/main.cpp` 中的分析流程

```cpp
// 伪代码
auto result = strategy.analyze(ts_code, data);
if (result.has_value()) {
    analysisResultDao.insert(*result);
    
    // 新增：保存过程数据
    AnalysisProcessRecord processRecord;
    processRecord.ts_code = ts_code;
    processRecord.strategy_name = result->strategy_name;
    processRecord.trade_date = result->trade_date;
    processRecord.signal = result->label;
    processRecord.data = extractLastNPeriods(data, 10);  // 提取最近10条
    processRecordDao.upsert(processRecord);
}
```

### 4. JSON 序列化

**决定**: 使用 nlohmann/json 的 `to_json`/`from_json` 模式

```cpp
void to_json(nlohmann::json& j, const ProcessDataPoint& p) {
    j = nlohmann::json{
        {"time", p.time},
        {"open", p.open},
        {"high", p.high},
        {"low", p.low},
        {"close", p.close},
        {"volume", p.volume}
    };
    if (p.ema17) j["ema17"] = *p.ema17;
    if (p.ema25) j["ema25"] = *p.ema25;
    if (p.macd) j["macd"] = *p.macd;
    if (p.rsi) j["rsi"] = *p.rsi;
}
```

## Risks / Trade-offs

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 性能影响 | 每次分析额外一次 DB 写入 | 使用 upsert 批量操作；异步写入可后续优化 |
| JSON 解析开销 | 查询时需要反序列化 | 只在需要时解析；前端已实现延迟加载 |
| 数据一致性 | 策略代码和 DAO 耦合 | 封装为 `saveAnalysisResult()` 函数 |

## Migration Plan

1. **部署**: 无需数据库迁移，表已存在
2. **回滚**: 删除 DAO 代码即可，不影响现有功能
3. **验证**: 通过 `/api/analysis/process` API 验证数据写入