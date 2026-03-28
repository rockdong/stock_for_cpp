## Why

当前 `analysis_process_records` 表每只股票会产生多条记录（每个策略+频率组合一条），导致数据分散、查询复杂。需要重构为每只股票只记录一条，包含所有策略和频率的完整分析数据。

## What Changes

- 重构数据模型：从 `(股票, 策略, 频率)` 改为 `(股票)` 一条记录
- JSON 结构重构：`data` 字段存储嵌套的策略→频率→K线数据
- 表结构调整：移除 `strategy_name`、`freq`、`signal` 列，修改 UNIQUE 约束
- DAO 层更新：修改插入/查询逻辑
- API 兼容：保持现有 API 返回格式兼容

## Capabilities

### Modified Capabilities
- `analysis-process-storage`: 从每个策略一条记录改为每只股票一条记录，包含所有策略数据

## Impact

**代码变更**:
- 修改 `cpp/data/database/AnalysisProcessRecord.h` - 新增嵌套数据结构
- 修改 `cpp/data/database/AnalysisProcessRecord.cpp` - 更新 JSON 序列化
- 修改 `cpp/data/database/AnalysisProcessRecordTable.h` - 移除列
- 修改 `cpp/data/database/AnalysisProcessRecordDAO.cpp` - 更新 upsert 逻辑
- 修改 `cpp/main.cpp` - 重写过程数据收集逻辑

**数据库迁移**:
- 创建新表或 ALTER 现有表
- 迁移现有数据（可选）

**API 变更**:
- `/api/analysis/process` - 返回格式调整，保持向后兼容