## Context

当前系统配置层已支持 `DB_TYPE` 环境变量（默认 sqlite），但数据层 `Connection.cpp` 硬编码使用 SQLite 连接。存在两套独立实现：
- `Connection.cpp` - SQLite 实现（当前被使用）
- `MySQLConnection.cpp` - MySQL 实现（未被调用）

**问题根源**：
1. 缺少统一的数据库接口（IConnection）
2. 缺少根据配置选择连接的工厂机制
3. `main.cpp` 第 222 行硬编码调用 `Connection::getInstance()`
4. MySQL 实现缺少 `trades` 表定义（与 SQLite 版本不一致）

**约束**：
- 必须保持现有 SQLite 行为的向后兼容
- DAO 层无需修改（继续使用单例连接）
- 配置系统已有 `db_type_` 成员但无 getter

## Goals / Non-Goals

**Goals**:
- 实现配置驱动的数据库类型切换（SQLite ↔ MySQL）
- 统一两种数据库的接口，便于扩展
- 增强启动日志，帮助用户确认数据库配置
- 补齐 MySQL 实现缺失的 `trades` 表

**Non-Goals**:
- 不引入 ORM 框架（继续使用 sqlpp11）
- 不修改 DAO 层的访问方式
- 不增加连接池功能（保持简单）
- 不支持其他数据库类型（如 PostgreSQL）

## Decisions

### Decision 1: 接口抽象方式

**选择**: 添加纯虚接口 `IConnection` + 工厂 `ConnectionFactory`

**理由**:
- 最小改动原则：只需修改 Connection.h/cpp 和 MySQLConnection.h/cpp
- 现有 DAO 代码无需修改：继续通过工厂获取单例
- 易于测试：可 mock IConnection 接口

**备选方案**:
1. ❌ 在 Connection.cpp 内部根据 DB_TYPE 分支 → 耦合度高，不易扩展
2. ❌ 创建 DatabaseManager 类管理连接 → 过度设计，当前需求简单
3. ✅ 接口 + 工厂模式 → SOLID，扩展性好

### Decision 2: 工厂返回类型

**选择**: 工厂返回 `IConnection&` 引用（单例）

**理由**:
- 保持现有代码风格（`getInstance()` 返回引用）
- 避免智能指针管理复杂性
- DAO 层无需修改获取方式

**备选方案**:
1. ❌ 返回 `std::unique_ptr<IConnection>` → 每次创建新实例，浪费资源
2. ✅ 返回 `IConnection&` → 单例模式，与现有代码一致

### Decision 3: trades 表补充时机

**选择**: 在 MySQLConnection::createTables() 中补充 trades 表

**理由**:
- SQLite 版本已有 trades 表，保持一致性
- MySQL 版本表数量应为 7 个（与 SQLite 相同）

### Decision 4: 启动日志增强内容

**选择**: 在 `initializeDatabase()` 中输出：
- 数据库类型（sqlite/mysql）
- 连接参数（host/port 或 db_path）
- 表创建结果（成功/失败，表数量）

**理由**:
- 帮助用户快速确认实际配置
- 便于排查数据库问题

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| MySQL 连接失败导致程序无法启动 | 保持 SQLite 为默认值；MySQL 需显式配置并确保服务可用 |
| 接口变更破坏现有代码 | Connection/MySQLConnection 继续提供 `getInstance()`；工厂为新增入口 |
| 配置读取时机问题 | Config::initialize() 在 initializeDatabase() 之前调用，确保配置已加载 |
| 多线程访问单例安全 | 保持现有 mutex 保护机制不变 |