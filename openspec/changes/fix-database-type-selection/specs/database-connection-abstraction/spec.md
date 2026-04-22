## ADDED Requirements

### Requirement: 数据库连接抽象接口

系统 SHALL 提供统一的数据库连接接口 `IConnection`，抽象 SQLite 和 MySQL 的共同操作。

#### Scenario: 接口定义一致性
- **WHEN** 定义数据库连接接口
- **THEN** 接口 SHALL 包含以下方法：initialize、connect、disconnect、isConnected、getDb、execute、beginTransaction、commit、rollback、createTables

#### Scenario: 多数据库实现支持
- **WHEN** 系统提供多种数据库实现
- **THEN** 每种实现 SHALL 继承并实现 IConnection 接口的全部方法

### Requirement: 数据库连接工厂

系统 SHALL 提供数据库连接工厂 `ConnectionFactory`，根据配置动态创建正确的数据库连接实例。

#### Scenario: SQLite 配置选择
- **WHEN** DB_TYPE 配置为 "sqlite" 或未设置
- **THEN** 工厂 SHALL 返回 SQLite 连接实例（Connection）

#### Scenario: MySQL 配置选择
- **WHEN** DB_TYPE 配置为 "mysql"
- **THEN** 工厂 SHALL 返回 MySQL 连接实例（MySQLConnection）

#### Scenario: 单例模式保持
- **WHEN** 多次调用工厂获取连接
- **THEN** 工厂 SHALL 返回相同的连接实例（单例模式）

### Requirement: 配置驱动的数据库切换

系统 SHALL 通过 Config 类提供数据库类型配置的访问能力。

#### Scenario: 配置 getter 提供
- **WHEN** 需要获取数据库类型配置
- **THEN** Config 类 SHALL 提供 getDbType() 方法返回 db_type_ 值

#### Scenario: 配置默认值
- **WHEN** DB_TYPE 环境变量未设置
- **THEN** getDbType() SHALL 返回默认值 "sqlite"

### Requirement: MySQL 表结构完整性

MySQL 实现 SHALL 包含与 SQLite 版本相同的所有数据库表。

#### Scenario: trades 表存在
- **WHEN** MySQL createTables() 执行
- **THEN** SHALL 创建 trades 表（包含 id、stock_id、trade_type、price、quantity、amount、trade_time 字段）

#### Scenario: 表数量一致
- **WHEN** 比较 SQLite 和 MySQL 的表结构
- **THEN** 两者 SHALL 包含相同数量的表（7个：stocks、prices、trades、analysis_results、analysis_progress、chart_data、analysis_process_records）

### Requirement: 启动时数据库信息日志

系统 SHALL 在数据库初始化阶段输出明确的数据库配置信息。

#### Scenario: 数据库类型日志
- **WHEN** initializeDatabase() 执行
- **THEN** SHALL 输出日志显示当前使用的数据库类型（sqlite/mysql）

#### Scenario: 连接参数日志
- **WHEN** 数据库连接建立
- **THEN** SHALL 输出日志显示连接参数（SQLite: db_path；MySQL: host、port、database）

#### Scenario: 表创建结果日志
- **WHEN** createTables() 完成
- **THEN** SHALL 输出日志显示表创建状态和表数量