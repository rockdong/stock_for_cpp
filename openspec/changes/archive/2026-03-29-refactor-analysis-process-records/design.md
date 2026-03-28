## Context

### 当前状态
`analysis_process_records` 表存储分析过程数据，但存在以下问题：
- 每条记录只存一个时间点，数据分散
- 查询 10 天数据需要聚合多条记录

### 约束
- 兼容现有 SQLite 数据库
- 保持 Node.js API 向后兼容（版本控制）
- 数据保留策略：7 天自动过期

### 涉及模块
- C++ 分析引擎（数据写入）
- Node.js REST API（数据读写）
- Web Frontend（数据展示）

## Goals / Non-Goals

**Goals:**
- 重构表结构，将 10 个周期数据合并为 JSON 数组存储
- 更新 C++ DAO 支持 JSON 序列化
- 更新 Node.js API 适配新格式
- 更新前端展示分析结果

**Non-Goals:**
- 不修改 `analysis_results` 表（独立的结论表）
- 不修改 `analysis_progress` 表（全局进度表）
- 不改变数据保留策略（仍为 7 天）
- 不添加状态追踪功能

## Decisions

### 1. 数据存储格式：JSON 数组 vs 多行记录

**选择：JSON 数组**

| 方案 | 优点 | 缺点 |
|------|------|------|
| JSON 数组 | 单次查询获取全部数据、减少记录数、便于前端使用 | JSON 解析开销、SQLite JSON 函数支持有限 |
| 多行记录 | 标准关系型设计、支持 SQL 聚合 | 查询效率低、记录数多、需关联查询 |

**理由**：
- 前端图表需要一次性获取 10 条数据，JSON 数组直接可用
- 减少数据库记录数 10 倍，提升查询效率
- SQLite 支持 JSON 函数（json_extract, json_array），足够使用

### 2. 时间格式统一

**选择：根据 `freq` 使用不同格式**

| freq | time 格式 | 示例 |
|------|-----------|------|
| d | YYYY-MM-DD | 2026-03-26 |
| w | YYYY-WXX | 2026-W13 |
| m | YYYY-MM | 2026-03 |

**理由**：
- 保持与现有 `trade_date` 字段格式兼容
- 便于前端按周期展示

### 3. 数据迁移策略

**选择：重建表 + 数据迁移脚本**

步骤：
1. 创建新表 `analysis_process_records_new`
2. 从旧表读取数据，按 `(ts_code, strategy_name, trade_date, freq)` 分组
3. 聚合 10 条记录为 JSON 数组，写入新表
4. 重命名旧表为备份，新表替换旧表

**理由**：
- 结构变化大，ALTER TABLE 不适用
- 需要数据聚合逻辑，脚本迁移更可控

## Risks / Trade-offs

### [风险] JSON 解析性能
- **影响**：大数据量查询时 JSON 解析可能成为瓶颈
- **缓解**：前端只请求必要字段；考虑添加虚拟列索引

### [风险] 数据迁移失败
- **影响**：历史数据丢失
- **缓解**：迁移前备份旧表；迁移后验证数据完整性

### [风险] C++ JSON 序列化复杂度
- **影响**：增加 C++ 代码复杂度
- **缓解**：使用 nlohmann/json 库（已集成）；封装工具函数

### [权衡] 存储空间
- JSON 格式比纯数值占用更多空间
- 但减少了记录数和索引开销，整体影响可控

## Migration Plan

### Phase 1: 数据库迁移
1. 创建迁移脚本 `002_refactor_analysis_process_records.sql`
2. 部署时自动执行迁移
3. 验证数据完整性

### Phase 2: C++ 更新
1. 更新 `AnalysisResult` 结构体（已有 strength/confidence）
2. 创建 `ProcessRecordDAO` 写入新格式
3. 实现 JSON 序列化工具函数

### Phase 3: Node.js API 更新
1. 更新 API 适配新表结构
2. 保持响应格式向后兼容

### Phase 4: 前端更新
1. 更新类型定义
2. 适配新数据格式

### Rollback
- 保留旧表备份 `analysis_process_records_backup`
- 回滚时重命名恢复