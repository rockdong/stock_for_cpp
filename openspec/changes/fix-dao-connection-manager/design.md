## Context

当前数据库连接架构：

```
ConnectionFactory::createConnection()
    → MySQLConnection::getInstance()  [MySQL 模式]
    → Connection::getInstance()        [SQLite 模式]

DAO 类直接调用：
    → Connection::getInstance()        [总是 SQLite！]

问题：DAO 类不知道应该用哪个连接
```

约束：
- 线程安全：多线程分析任务并发访问数据库
- 单例模式：Connection 和 MySQLConnection 已是单例，需保持
- 最小改动：尽量复用现有架构

## Goals / Non-Goals

**Goals:**
- DAO 类能获取正确的数据库连接（MySQL 或 SQLite）
- 保持线程安全
- 最小改动现有代码

**Non-Goals:**
- 不重构 Connection 和 MySQLConnection 单例模式
- 不引入连接池（超出当前需求）
- 不修改 IConnection 接口

## Decisions

### 决策1: 创建 ConnectionManager 单例

**选择**: 新增 ConnectionManager 类，持有当前连接的引用

**理由**:
- 最小改动：只需修改 DAO 的 getInstance() 调用
- 线程安全：使用 mutex 保护
- 单例模式：与现有架构一致

**备选方案**:
- 方案A: 修改所有 DAO 类直接使用 ConnectionFactory - 改动过大，每次调用都创建新连接
- 方案B: 将 Connection 合并为统一接口 - 需要重构两个单例类，改动过大
- 方案C（选择）: ConnectionManager 持有连接引用 - 改动最小

### 决策2: ConnectionManager 存储 IConnection 引用

**选择**: 使用 `IConnection&` 引用，不持有所有权

**理由**:
- Connection 和 MySQLConnection 已是单例，生命周期由自己管理
- ConnectionManager 只是提供访问入口

### 决策3: ConnectionFactory 注册连接

**选择**: `createConnection()` 返回连接后，调用 `ConnectionManager::registerConnection()`

**理由**:
- 集中式管理：所有连接创建都通过 ConnectionFactory
- 简单可靠：创建后立即注册

## Risks / Trade-offs

**[风险] ConnectionManager 与连接生命周期不一致**
→ ConnectionManager 只持有引用，连接由单例管理，生命周期一致

**[风险] 多线程并发访问**
→ ConnectionManager 使用 mutex 保护所有方法

**[Trade-off] 引入新类增加复杂度**
→ 相比修改 30+ 个 DAO 调用点，一个 ConnectionManager 类更简洁