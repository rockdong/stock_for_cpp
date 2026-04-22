## Why

当前代码在 `DB_TYPE=mysql` 但编译时未启用 MySQL 支持时，会自动降级使用 SQLite 并静默创建 SQLite 表。这种降级行为造成两个问题：

1. **误导用户**：用户配置了 MySQL 连接参数，以为数据存入 MySQL，实际却悄悄使用了 SQLite
2. **数据丢失风险**：用户通过 sqlite-browser 连接 MySQL 数据库发现空库，误以为是表创建失败，实际数据在另一个 SQLite 文件中

用户明确要求：**MySQL 配置时必须使用 MySQL，失败时直接报错，不要降级处理**。

## What Changes

- **移除降级逻辑**：`ConnectionFactory.cpp` 中删除 "MySQL 支持未编译，降级使用 SQLite" 的 fallback
- **启用严格模式**：当 `DB_TYPE=mysql` 但 `HAS_MYSQL` 未定义时，抛出异常终止启动
- **改进错误信息**：提供明确的错误提示，告知用户需要安装 MySQL 库并重新编译
- **BREAKING**：旧配置 `DB_TYPE=mysql` 在未编译 MySQL 支持时，程序将启动失败而非降级运行

## Capabilities

### New Capabilities
- `mysql-strict-failure`: MySQL 配置失败时的严格处理行为 - 抛出异常而非降级

### Modified Capabilities
- 无（这是一个新的失败处理策略，不影响现有 spec 定义的行为）

## Impact

- **代码**：`cpp/data/database/ConnectionFactory.cpp` 第 33-37 行的降级逻辑
- **用户体验**：用户必须确保 MySQL 开发库已安装（`libmysqlclient-dev`）并重新编译
- **Docker/CI**：当前已安装 `libmysqlclient-dev`，编译应能检测到 MySQL；需验证 Dockerfile 运行时依赖
- **日志输出**：失败时输出明确的错误信息而非警告