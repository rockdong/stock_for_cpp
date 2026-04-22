## Why

MySQL 8.0 不允许 JSON 类型列设置 DEFAULT 值，这是 MySQL 的限制而非 bug。当前 `analysis_process_records` 表定义中使用了 `JSON NOT NULL DEFAULT '{"strategies":[]}'`，导致表创建失败，程序启动时数据库连接失败，整个分析引擎无法运行。

这个问题在 Docker 容器环境中暴露出来，因为之前 SQLite 支持此语法，迁移到 MySQL 后才发现不兼容。

## What Changes

- 移除 `analysis_process_records` 表中 JSON 列的 DEFAULT 值定义
- 在插入数据时提供默认 JSON 值 `'{"strategies":[]}'`
- 确保 MySQL 和 SQLite 的 SQL 语法兼容

## Capabilities

### New Capabilities

无新增能力

### Modified Capabilities

- `mysql-connection`: 修改 MySQL 表创建逻辑，移除 JSON DEFAULT 值

## Impact

**直接影响**：
- `cpp/data/database/MySQLConnection.cpp` - createTables() 方法中的 SQL 语句
- `cpp/data/database/MySQLAdapter.cpp` - 可能的插入逻辑（需要检查是否有依赖）

**间接影响**：
- Docker 容器启动流程 - 数据库初始化阶段
- 分析引擎启动 - 成功创建表后才能正常运行

**兼容性**：
- SQLite 继续支持 JSON DEFAULT 值（SQLite 允许）
- MySQL 需要在插入时提供默认值