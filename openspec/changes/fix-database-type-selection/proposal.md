## Why

当前系统配置层已支持通过 `DB_TYPE` 环境变量选择 MySQL 或 SQLite，但代码实现层硬编码使用 SQLite 连接，导致 MySQL 配置无效。这造成：
1. 用户配置 MySQL 后发现只创建了空数据库，无表结构
2. 运行时实际使用的数据库类型与配置不符
3. 启动时缺少日志帮助用户确认实际使用的数据库类型

## What Changes

- 添加数据库抽象接口 `IConnection`，统一 SQLite 和 MySQL 的连接操作
- 添加数据库工厂 `ConnectionFactory`，根据 `DB_TYPE` 配置动态选择连接实现
- 修改 `main.cpp` 使用工厂获取连接，而非硬编码 SQLite
- 补充 MySQL 实现缺失的 `trades` 表定义
- 增强启动日志，输出数据库类型、连接参数和表创建结果

## Capabilities

### New Capabilities
- `database-connection-abstraction`: 数据库连接抽象层，提供统一接口和工厂模式，支持配置驱动的 MySQL/SQLite 切换

### Modified Capabilities
无（本次为架构改进，不涉及现有 spec 的行为变更）

## Impact

**直接影响的文件**:
- `cpp/data/database/Connection.h` - 添加 IConnection 接口
- `cpp/data/database/Connection.cpp` - SQLite 实现继承 IConnection
- `cpp/data/database/MySQLConnection.h` - MySQL 实现继承 IConnection
- `cpp/data/database/MySQLConnection.cpp` - 补充 trades 表，实现接口
- `cpp/data/database/ConnectionFactory.h` - 新增工厂类
- `cpp/data/database/ConnectionFactory.cpp` - 工厂实现
- `cpp/config/Config.h` - 添加 `getDbType()` getter
- `cpp/main.cpp` - 使用工厂获取连接，增加启动日志

**间接影响**:
- 所有 DAO 类无需修改，继续通过 `Connection::getInstance()` 或工厂获取连接
- 用户可通过 `.env` 配置真正切换数据库类型

**兼容性**:
- 现有 SQLite 使用方式保持兼容（默认行为不变）
- 新增 MySQL 支持不影响现有功能