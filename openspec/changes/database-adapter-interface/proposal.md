## Why

当前数据库层直接在 Connection.cpp 和 MySQLConnection.cpp 中硬编码 SQL 语句，导致 SQLite 和 MySQL 的语法差异无法统一处理。ALTER TABLE ADD COLUMN IF NOT EXISTS 在 SQLite 中不支持，INSERT OR IGNORE vs INSERT IGNORE 语法不同，数据类型映射不一致（TEXT vs VARCHAR、INTEGER vs INT、JSON vs TEXT）。每次新增数据库功能需要在两个文件中重复编写相似的建表逻辑，维护成本高且容易遗漏同步。

## What Changes

### 新增文件
- `DatabaseTypes.h` — 定义 DataType 枚举、ColumnDefinition 和 IndexDefinition 结构体
- `IDatabaseAdapter.h` — 数据库抽象接口，包含表操作、字段操作、索引操作、数据操作方法
- `SQLiteAdapter.h/cpp` — SQLite 实现，生成 SQLite 兼容的 SQL 语句
- `MySQLAdapter.h/cpp` — MySQL 实现，生成 MySQL 兼容的 SQL 语句
- `AdapterFactory.h/cpp` — 工厂类，根据配置创建对应适配器

### 重构文件
- `Connection.cpp` — 使用 SQLiteAdapter 生成 SQL，移除硬编码的建表语句
- `MySQLConnection.cpp` — 使用 MySQLAdapter 生成 SQL，移除硬编码的建表语句

### **BREAKING** 变更
- 移除 sqlpp11 依赖，使用统一的接口进行数据库操作
- DAO 类需要适配新的接口或继续使用底层 connection 对象

## Capabilities

### New Capabilities
- `database-types`: 数据类型定义结构（DataType、ColumnDefinition、IndexDefinition）
- `database-adapter`: 数据库抽象接口（表/字段/索引/数据的统一操作）
- `sqlite-adapter`: SQLite 适配器实现
- `mysql-adapter`: MySQL 适配器实现
- `adapter-factory`: 适配器工厂类

### Modified Capabilities
- `core-system`: 数据库初始化流程改为使用适配器

## Impact

- **新增代码**: 约 6 个新文件，800+ 行代码
- **重构代码**: Connection.cpp 和 MySQLConnection.cpp 的 createTables() 方法
- **编译依赖**: 无新增外部依赖，使用现有的 sqlpp11 连接对象
- **向后兼容**: 保持 IConnection 接口不变，新增方法通过组合方式实现