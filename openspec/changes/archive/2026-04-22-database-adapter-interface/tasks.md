## 1. 类型定义

- [x] 1.1 创建 DatabaseTypes.h 文件
- [x] 1.2 定义 DataType 枚举（TEXT, INTEGER, DECIMAL, DATETIME, TIMESTAMP, DATE, JSON, BOOLEAN）
- [x] 1.3 定义 ColumnDefinition 结构体（name, type, precision, scale, varcharLength, nullable, defaultValue, autoIncrement, primaryKey, unique, checkConstraint）
- [x] 1.4 定义 IndexDefinition 结构体（name, tableName, columns, unique）
- [x] 1.5 定义 TableDefinition 结构体（name, columns）

## 2. 抽象接口

- [x] 2.1 创建 IDatabaseAdapter.h 文件
- [x] 2.2 定义连接管理方法（connect, disconnect, isConnected, getDbType）
- [x] 2.3 定义事务方法（beginTransaction, commit, rollback）
- [x] 2.4 定义表操作方法（createTable, tableExists, dropTable）
- [x] 2.5 定义字段操作方法（columnExists, addColumn, getColumns）
- [x] 2.6 定义索引操作方法（createIndex, indexExists, dropIndex）
- [x] 2.7 定义数据操作方法（insertOrIgnore）
- [x] 2.8 定义迁移方法（runMigrations, getSchemaVersion, setSchemaVersion）

## 3. Schema 定义

- [x] 3.1 创建 SchemaDefinitions.h 文件
- [x] 3.2 定义 stocks 表结构（STOCKS_TABLE）
- [x] 3.3 定义 prices 表结构（PRICES_TABLE）
- [x] 3.4 定义 trades 表结构（TRADES_TABLE）
- [x] 3.5 定义 analysis_results 表结构（ANALYSIS_RESULTS_TABLE）
- [x] 3.6 定义 analysis_progress 表结构（ANALYSIS_PROGRESS_TABLE）
- [x] 3.7 定义 chart_data 表结构（CHART_DATA_TABLE）
- [x] 3.8 定义 analysis_process_records 表结构（ANALYSIS_PROCESS_RECORDS_TABLE）
- [x] 3.9 定义所有索引结构（STOCKS_INDEXES, PRICES_INDEXES 等）

## 4. SQLite 适配器

- [x] 4.1 创建 SQLiteAdapter.h 文件
- [x] 4.2 创建 SQLiteAdapter.cpp 文件
- [x] 4.3 实现连接管理方法（使用 sqlpp::sqlite3::connection）
- [x] 4.4 实现 generateCreateTableSQL 方法（生成 SQLite 语法的 CREATE TABLE）
- [x] 4.5 实现 generateAddColumnSQL 方法（不带 IF NOT EXISTS）
- [x] 4.6 实现 generateCreateIndexSQL 方法（带 IF NOT EXISTS）
- [x] 4.7 实现 generateInsertOrIgnoreSQL 方法（INSERT OR IGNORE）
- [x] 4.8 实现 columnExists 方法（使用 PRAGMA table_info）
- [x] 4.9 实现 tableExists 方法（使用 sqlite_master 表）
- [x] 4.10 实现 indexExists 方法（使用 sqlite_master 表）
- [x] 4.11 实现 configurePerformance 方法（执行 PRAGMA busy_timeout/journal_mode/synchronous）
- [x] 4.12 实现 createTables 方法（调用 createTable for each schema definition）
- [x] 4.13 实现迁移方法（schema_version 表）

## 5. MySQL 适配器

- [x] 5.1 创建 MySQLAdapter.h 文件
- [x] 5.2 创建 MySQLAdapter.cpp 文件
- [x] 5.3 实现连接管理方法（使用 sqlpp::mysql::connection）
- [x] 5.4 实现 generateCreateTableSQL 方法（生成 MySQL 语法的 CREATE TABLE + ENGINE/CHARSET）
- [x] 5.5 实现 generateAddColumnSQL 方法（带 IF NOT EXISTS）
- [x] 5.6 实现 generateCreateIndexSQL 方法（不带 IF NOT EXISTS，先检查存在）
- [x] 5.7 实现 generateInsertOrIgnoreSQL 方法（INSERT IGNORE）
- [x] 5.8 实现 columnExists 方法（使用 INFORMATION_SCHEMA.COLUMNS）
- [x] 5.9 实现 tableExists 方法（使用 INFORMATION_SCHEMA.TABLES）
- [x] 5.10 实现 indexExists 方法（使用 INFORMATION_SCHEMA.STATISTICS）
- [x] 5.11 实现 createTables 方法（调用 createTable for each schema definition）
- [x] 5.12 实现迁移方法（schema_version 表）

## 6. 工厂类

- [x] 6.1 创建 AdapterFactory.h 文件
- [x] 6.2 创建 AdapterFactory.cpp 文件
- [x] 6.3 实现 create 方法（根据 DB_TYPE 返回对应适配器）
- [x] 6.4 实现默认配置处理（无配置时返回 SQLiteAdapter）

## 7. 重构 Connection

- [x] 7.1 在 Connection.h 中添加 adapter_ 成员变量
- [x] 7.2 在 Connection::connect() 中创建 SQLiteAdapter 实例
- [x] 7.3 重构 Connection::createTables() 使用 adapter_->createTables()
- [x] 7.4 删除 Connection.cpp 中硬编码的 CREATE TABLE 语句
- [x] 7.5 删除 Connection.cpp 中硬编码的 ALTER TABLE 语句
- [x] 7.6 删除 Connection.cpp 中硬编码的 CREATE INDEX 语句
- [x] 7.7 删除 Connection.cpp 中硬编码的 INSERT OR IGNORE 语句
- [x] 7.8 添加 adapter_->configurePerformance() 调用

## 8. 重构 MySQLConnection

- [ ] 8.1 在 MySQLConnection.h 中添加 adapter_ 成员变量
- [ ] 8.2 在 MySQLConnection::connect() 中创建 MySQLAdapter 实例
- [ ] 8.3 重构 MySQLConnection::createTables() 使用 adapter_->createTables()
- [ ] 8.4 删除 MySQLConnection.cpp 中硬编码的 CREATE TABLE 语句
- [ ] 8.5 删除 MySQLConnection.cpp 中硬编码的 CREATE INDEX 语句
- [ ] 8.6 删除 MySQLConnection.cpp 中硬编码的 INSERT IGNORE 语句

## 9. 构建配置

- [x] 9.1 更新 CMakeLists.txt 添加新文件到编译列表
- [x] 9.2 添加 data_lib 的 target_include_directories
- [ ] 9.3 验证编译无错误（待 CI/CD）

## 10. 测试验证

- [ ] 10.1 编写 SQLiteAdapter 单元测试（generateCreateTableSQL）
- [ ] 10.2 编写 MySQLAdapter 单元测试（generateCreateTableSQL）
- [ ] 10.3 编写 SQLiteAdapter::columnExists 测试
- [ ] 10.4 编写 MySQLAdapter::columnExists 测试
- [ ] 10.5 编写 SQLiteAdapter::addColumn 测试（ifNotExists=true）
- [ ] 10.6 编写 MySQLAdapter::addColumn 测试（ifNotExists=true）
- [ ] 10.7 测试 Connection.cpp 重构后的 createTables()
- [ ] 10.8 测试 MySQLConnection.cpp 重构后的 createTables()
- [ ] 10.9 测试新数据库初始化流程
- [ ] 10.10 测试旧数据库升级流程（ALTER TABLE 正常执行）