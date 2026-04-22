# fix-mysql-json-default

MySQL 不允许 JSON 列设置 DEFAULT 值，导致 analysis_process_records 表创建失败，程序启动时数据库连接失败
