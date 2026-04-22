## 1. 代码修改

- [x] 1.1 修改 MySQLConnection.cpp 中 analysis_process_records 表的 SQL 定义，移除 JSON DEFAULT 值
- [x] 1.2 检查 AnalysisProcessRecordDAO 的插入逻辑，确认是否需要提供默认 JSON 值（结论：不需要修改）
- [x] 1.3 如需要，修改 AnalysisProcessRecordDAO 的插入逻辑以提供默认值 `'{"strategies":[]}'`（无需修改）

## 2. 验证

- [x] 2.1 本地编译验证（macOS）- 成功
- [ ] 2.2 CI 编译验证（Ubuntu）
- [ ] 2.3 Docker 容器启动验证（观察日志确认表创建成功）
- [ ] 2.4 使用 adminer 检查 MySQL 数据库表结构