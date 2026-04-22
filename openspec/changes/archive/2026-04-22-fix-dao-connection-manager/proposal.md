## Why

当前系统存在数据库连接架构缺陷：DAO 类直接调用 `Connection::getInstance()`（SQLite 单例），绕过了 `ConnectionFactory`。当配置 `DB_TYPE=mysql` 时，`ConnectionFactory` 正确创建 `MySQLConnection::getInstance()`，但所有 DAO 类仍使用未连接的 SQLite 单例，导致"数据库未连接"错误，分析任务无法执行。

这是多线程环境下的架构问题，阻碍了 MySQL 支持的完整实现。

## What Changes

- 创建 `ConnectionManager` 类，统一管理数据库连接实例
- `ConnectionFactory::createConnection()` 返回连接后注册到 `ConnectionManager`
- 修改所有 DAO 类使用 `ConnectionManager::getConnection()` 获取当前连接
- 保持 `Connection` 和 `MySQLConnection` 单例模式不变

**BREAKING**: DAO 类不再直接调用 `Connection::getInstance()`

## Capabilities

### New Capabilities

- `dao-connection`: DAO 类通过 ConnectionManager 获取正确的数据库连接

### Modified Capabilities

无修改现有能力（这是内部架构修复，不影响 spec 级别的行为）

## Impact

**直接影响**：
- 新增 `cpp/data/database/ConnectionManager.h/cpp`
- 修改 `ConnectionFactory.cpp` - 注册连接到 ConnectionManager
- 修改所有 DAO 类（StockDAO、PriceDAO、AnalysisResultDAO、ChartDataDAO、AnalysisProcessRecordDAO）- 约 30+ 处 `Connection::getInstance()` 调用

**间接影响**：
- 多线程分析任务可正确访问 MySQL 连接
- SQLite 和 MySQL 模式统一工作

**兼容性**：
- 不影响现有 SQLite 单机模式
- MySQL 模式完全可用