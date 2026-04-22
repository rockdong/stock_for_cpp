## 1. 创建 ConnectionManager

- [x] 1.1 创建 ConnectionManager.h 头文件
- [x] 1.2 创建 ConnectionManager.cpp 实现文件
- [x] 1.3 实现 getInstance() 单例方法
- [x] 1.4 实现 registerConnection(IConnection&) 方法
- [x] 1.5 实现 getConnection() 方法
- [x] 1.6 实现 isConnected() 方法
- [x] 1.7 实现 getConnectionForDb() 和 getDb() 方法

## 2. 修改 ConnectionFactory

- [x] 2.1 在 ConnectionFactory.cpp 中调用 ConnectionManager::registerConnection()
- [x] 2.2 添加 ConnectionManager.h 头文件引用

## 3. 修改 DAO 类

- [x] 3.1 修改 StockDAO.cpp - 替换所有 Connection::getInstance() 为 ConnectionManager::getConnection()
- [x] 3.2 修改 PriceDAO.cpp - 替换所有 Connection::getInstance() 为 ConnectionManager::getConnection()
- [x] 3.3 修改 AnalysisResultDAO.cpp - 替换所有 Connection::getInstance() 为 ConnectionManager::getConnection()
- [x] 3.4 修改 ChartDataDAO.cpp - 替换所有 Connection::getInstance() 为 ConnectionManager::getConnection()
- [x] 3.5 修改 AnalysisProcessRecordDAO.cpp - 替换所有 Connection::getInstance() 为 ConnectionManager::getConnection()
- [x] 3.6 所有 DAO 使用 ConnectionManager::getDb() 获取 sqlpp11 连接

## 4. 验证

- [x] 4.1 本地编译验证（macOS） - 成功
- [x] 4.2 CI 编译验证（Ubuntu） - 成功 (run 24783920803)
- [x] 4.3 Docker MySQL 模式验证 - 分析任务执行成功，MySQL SQL 执行成功
- [x] 4.4 Docker SQLite 模式验证 - 本地编译成功，条件编译兼容