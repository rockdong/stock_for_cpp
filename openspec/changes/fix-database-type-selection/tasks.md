## 1. 接口抽象层

- [x] 1.1 创建 IConnection 纯虚接口（Connection.h），定义 initialize、connect、disconnect、isConnected、getDb、execute、beginTransaction、commit、rollback、createTables 方法
- [x] 1.2 修改 Connection 类继承 IConnection 接口，保持现有实现不变
- [x] 1.3 修改 MySQLConnection 类继承 IConnection 接口，保持现有实现不变

## 2. 工厂模式实现

- [x] 2.1 创建 ConnectionFactory.h，声明 createConnection() 方法返回 IConnection& 引用
- [x] 2.2 实现 ConnectionFactory.cpp，根据 Config::getDbType() 返回对应的连接实例（sqlite → Connection，mysql → MySQLConnection）
- [x] 2.3 确保工厂返回单例实例（调用 getInstance()）

## 3. 配置层扩展

- [x] 3.1 在 Config.h 中添加 getDbType() getter 方法声明
- [x] 3.2 在 Config.cpp 中实现 getDbType()，返回 db_type_ 成员值

## 4. MySQL 表结构补充

- [x] 4.1 在 MySQLConnection::createTables() 中添加 trades 表定义（id INT AUTO_INCREMENT PRIMARY KEY、stock_id INT、trade_type VARCHAR(20)、price DECIMAL(18,4)、quantity INT、amount DECIMAL(20,4)、trade_time DATETIME、FOREIGN KEY）
- [x] 4.2 确保 MySQL 创建的表数量与 SQLite 版本一致（7个表）

## 5. 主程序修改

- [x] 5.1 修改 main.cpp initializeDatabase() 函数，使用 ConnectionFactory::createConnection() 获取连接
- [x] 5.2 移除硬编码的 "stock.db" 参数，改为从 Config 获取（SQLite: db_path 或文件名；MySQL: 从 Config 获取 host/port/database/user/password）

## 6. 启动日志增强

- [x] 6.1 在 initializeDatabase() 开始时输出日志：数据库类型（Config::getDbType()）
- [x] 6.2 在数据库连接建立后输出日志：连接参数（SQLite: db_path；MySQL: host:port/database）
- [x] 6.3 在 createTables() 完成后输出日志：表创建结果和表数量

## 7. 编译与测试

- [x] 7.1 更新 CMakeLists.txt，添加 ConnectionFactory.cpp 到编译列表
- [x] 7.2 编译验证：无语法错误、无链接错误
- [x] 7.3 运行测试：默认配置（SQLite）启动成功，日志输出正确
- [x] 7.4 运行测试：MySQL 配置（需要 MySQL 服务）启动成功，日志输出正确