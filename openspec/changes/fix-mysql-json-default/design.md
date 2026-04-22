## Context

当前系统使用 MySQLConnection.cpp 中的 `createTables()` 方法创建数据库表。`analysis_process_records` 表使用了 MySQL 不支持的语法：

```sql
data JSON NOT NULL DEFAULT '{"strategies":[]}'
```

MySQL 限制：JSON、BLOB、TEXT、GEOMETRY 类型列不能设置 DEFAULT 值。

对比成功的 `chart_data` 表：
```sql
data JSON NOT NULL  -- 没有 DEFAULT，成功创建
```

## Goals / Non-Goals

**Goals:**
- 移除 JSON 列的 DEFAULT 值定义
- 确保表创建成功，程序能正常启动
- 保持 SQLite 和 MySQL 的兼容性

**Non-Goals:**
- 不修改其他表的创建逻辑（已成功的表不需要修改）
- 不重构整个数据库连接架构
- 不添加 JSON 列 DEFAULT 值的模拟机制

## Decisions

### 决策1: 直接移除 DEFAULT 值

**选择**: 移除 `JSON NOT NULL DEFAULT` 中的 DEFAULT 部分

**理由**:
- 简单直接，改动最小
- 不引入额外复杂性
- `chart_data` 表已验证此方式可行

**备选方案**:
- 方案A: 使用触发器设置默认值 - 过度复杂，维护成本高
- 方案B: 应用层在插入时提供默认值 - 需要检查所有插入点，改动范围大
- 方案C（选择）: 直接移除 DEFAULT - 改动最小，风险最低

### 决策2: 插入数据时的默认值处理

**选择**: 检查是否有代码依赖此默认值，如有则修改插入逻辑

**理由**:
- 当前表创建失败，还没有数据插入
- 需要检查 AnalysisProcessRecordDAO 的插入逻辑

## Risks / Trade-offs

**[风险] 插入逻辑可能依赖 DEFAULT 值**
→ 检查 AnalysisProcessRecordDAO，确认插入时是否需要提供默认值

**[风险] 现有数据可能需要迁移**
→ 此表是新表（之前创建失败），无现有数据，无需迁移

**[Trade-off] 丧失数据库层级的默认值保护**
→ 应用层需要负责提供默认值，增加了代码责任