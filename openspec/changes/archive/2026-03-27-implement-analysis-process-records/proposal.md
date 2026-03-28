## Why

Web 前端查询分析过程数据为空，导致用户无法查看 K 线、技术指标等详细分析过程。目前 C++ 策略引擎只写入最终信号（买入/卖出），不保存分析过程数据。`analysis_process_records` 表结构已存在，但从未有代码写入数据。

## What Changes

- 创建 `AnalysisProcessRecordDAO` 数据访问层，支持 CRUD 操作
- 修改策略分析流程，在检测到信号时保存最近 10 个周期的 K 线和技术指标
- 将过程数据序列化为 JSON 数组存入 `data` 字段
- 支持按股票代码、策略、日期、信号类型查询历史过程数据

## Capabilities

### New Capabilities
- `analysis-process-storage`: 分析过程数据持久化能力，存储策略分析时的 K 线数据和技术指标

### Modified Capabilities
- 无（这是新增功能，不影响现有 analysis-results 的行为）

## Impact

**代码变更**:
- 新增 `cpp/data/database/AnalysisProcessRecordDAO.h/.cpp`
- 修改 `cpp/main.cpp` 中的策略执行流程
- 修改各策略类（EMA17TO25Strategy 等）添加过程数据保存

**数据流**:
```
策略分析 → 生成信号 → AnalysisResultDAO.insert() [已有]
                    → AnalysisProcessRecordDAO.insert() [新增]
```

**API 端点**:
- `/api/analysis/process` - 已实现，返回空数组，本变更后将有数据
- `/api/analysis/process/chart/:ts_code` - 图表数据接口

**兼容性**: 完全向后兼容，不影响现有功能