# fix-dao-connection-manager

DAO 类直接调用 Connection::getInstance()（SQLite 单例），绕过了 ConnectionFactory，导致 MySQL 配置时 DAO 使用未连接的 SQLite 连接而非已连接的 MySQL 连接
