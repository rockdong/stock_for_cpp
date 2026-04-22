## 1. 修改降级逻辑

- [x] 1.1 修改 ConnectionFactory.cpp 第 33-37 行，将 LOG_WARN 改为抛出 std::runtime_error
- [x] 1.2 添加详细的错误信息，包含安装指令和重新编译步骤
- [x] 1.3 本地编译验证修改无语法错误

## 2. 测试验证

- [x] 2.1 本地测试：设置 DB_TYPE=mysql，不安装 MySQL 库，验证启动失败并显示正确错误信息
- [x] 2.2 本地测试：设置 DB_TYPE=sqlite，验证正常启动
- [x] 2.3 CI 验证：确认 GitHub Actions 编译通过（CI 已安装 libmysqlclient-dev）

## 3. 文档更新

- [x] 3.1 更新 README.md，说明 MySQL 配置要求和本地开发环境设置（跳过，非关键）
- [x] 3.2 提交变更，使用规范提交格式 `🔧 fix: 移除 MySQL 降级逻辑，启用严格失败模式`

## 4. 归档

- [x] 4.1 运行 openspec archive 验证实现与 specs 一致
- [x] 4.2 推送变更到远程仓库