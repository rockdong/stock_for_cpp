## Context

当前数据层架构：

```
ConnectionFactory → IConnection
                      ├─ Connection (SQLite)
                      │   ├─ connect()
                      │   ├─ execute(sql)
                      │   └─ createTables() ← 硬编码 SQLite SQL
                      └─ MySQLConnection (MySQL)
                          ├─ connect()
                          ├─ execute(sql)
                          └─ createTables() ← 硬编码 MySQL SQL
```

问题：
- createTables() 在两个实现类中重复定义相似的表结构
- SQL 语法差异导致维护困难（IF NOT EXISTS、INSERT OR IGNORE vs INSERT IGNORE）
- 新增表或字段需要在两个文件同步修改

**约束条件：**
- 必须与现有 IConnection 接口兼容
- DAO 层使用 sqlpp11 进行查询，需要继续支持
- 不能引入新的外部依赖

## Goals / Non-Goals

**Goals:**
- 统一的数据库类型定义（DataType、ColumnDefinition、IndexDefinition）
- 抽象的表/字段/索引操作接口
- SQLite 和 MySQL 的 SQL 语法自动适配
- 可扩展支持其他数据库（PostgreSQL）
- 迁移（schema versioning）支持

**Non-Goals:**
- 不替代 sqlpp11 的查询功能
- 不实现 ORM
- 不处理数据库连接池管理
- 不支持分布式数据库

## Decisions

### 1. 接口设计模式

**选择：组合模式（Adapter 组合进 Connection）**

```cpp
class Connection : public IConnection {
private:
    std::unique_ptr<IDatabaseAdapter> adapter_;
public:
    bool createTables() override {
        return adapter_->createTables();
    }
};
```

**理由：**
- 保持 IConnection 接口不变
- 适配器可独立测试
- 支持运行时切换数据库类型

**备选：继承模式**
- Adapter 继承 IConnection
- 缺点：MySQLConnection 和 SQLiteAdapter 分离困难

### 2. SQL 生成策略

**选择：运行时生成 SQL 字符串**

```cpp
std::string SQLiteAdapter::generateCreateTableSQL(
    const std::string& tableName,
    const std::vector<ColumnDefinition>& columns
) {
    // 拼接 SQL 字符串
}
```

**理由：**
- 灵活处理语法差异
- 无需模板引擎
- 易于调试（生成的 SQL 可见）

**备选：模板文件**
- 使用外部 SQL 模板文件
- 缺点：文件管理复杂，参数替换困难

### 3. 类型映射

**选择：枚举 + 结构体定义**

```cpp
enum DataType { TEXT, INTEGER, DECIMAL, DATETIME, TIMESTAMP, DATE, JSON, BOOLEAN };

struct ColumnDefinition {
    std::string name;
    DataType type;
    int precision = 0;     // DECIMAL(18,4) → precision=18, scale=4
    int scale = 0;
    int varcharLength = 0; // VARCHAR(20) → varcharLength=20
    bool nullable = true;
    std::string defaultValue;
    bool autoIncrement = false;
    bool primaryKey = false;
    bool unique = false;
};
```

**理由：**
- 类型安全
- 编译期检查
- 自文档化

**备选：字符串配置**
- `"INTEGER PRIMARY KEY AUTOINCREMENT"`
- 缺点：无法编译期验证，解析复杂

### 4. Schema 定义位置

**选择：集中定义在 SchemaDefinitions.h**

```cpp
namespace schema {

const std::vector<ColumnDefinition> STOCKS_COLUMNS = {
    {"id", DataType::INTEGER, 0, 0, 0, false, "", true, false, true, false},
    {"ts_code", DataType::TEXT, 0, 0, 20, false, "", false, true, false, false},
    ...
};

const TableDefinition STOCKS_TABLE = {"stocks", STOCKS_COLUMNS};

}
```

**理由：**
- 单一位置定义，易于维护
- SQLite 和 MySQL 共享同一结构
- 新增表只需添加一个定义

**备选：分散在各 DAO**
- 每个 DAO 定义自己的表
- 缺点：分散管理，难以全局视图

### 5. 迁移策略

**选择：版本号 + 迁移函数**

```cpp
class IDatabaseAdapter {
    virtual int getSchemaVersion() = 0;
    virtual bool setSchemaVersion(int version) = 0;
    virtual bool runMigrations() = 0;
};
```

迁移流程：
```
connect() → getSchemaVersion() → 执行未完成的迁移 → setSchemaVersion()
```

**理由：**
- 支持增量迁移
- 可追溯数据库状态
- 支持回滚（可选）

**备选：CREATE TABLE IF NOT EXISTS**
- 每次连接执行全部建表语句
- 缺点：不支持字段变更，无版本追踪

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| SQL 生成错误导致表创建失败 | 单元测试覆盖所有表定义，验证生成的 SQL |
| 性能开销（运行时拼接 SQL） | 仅在连接时执行一次，性能影响微乎其微 |
| 与 sqlpp11 冲突 | adapter 仅用于 DDL，DAO 继续使用 sqlpp11 查询 |
| 迁移脚本遗漏 | 完整测试矩阵：新安装 + 旧数据库升级 |

## Migration Plan

1. 新增 DatabaseTypes.h、IDatabaseAdapter.h
2. 实现 SQLiteAdapter.h/cpp
3. 实现 MySQLAdapter.h/cpp
4. 新增 SchemaDefinitions.h
5. 实现 AdapterFactory.h/cpp
6. 重构 Connection.cpp 使用 SQLiteAdapter
7. 重构 MySQLConnection.cpp 使用 MySQLAdapter
8. 更新 CMakeLists.txt
9. 单元测试
10. 集成测试

**回滚策略：**
- Git revert 恢复原 Connection.cpp 和 MySQLConnection.cpp
- 删除新增的 Adapter 文件

## Open Questions

无待解决问题。