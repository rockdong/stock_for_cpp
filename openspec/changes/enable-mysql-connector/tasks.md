## 1. 修改 CMakeLists.txt

- [x] 1.1 删除第63行强制禁用 MySQL connector 的代码
- [x] 1.2 在 add_subdirectory(thirdparty/sqlpp11) 之前添加 CMAKE_MODULE_PATH + 条件性 BUILD_MYSQL_CONNECTOR
- [x] 1.3 修改第67-74行：使用 `if(TARGET sqlpp11::mysql)` 替代手动 find_package(MySQL)
- [x] 1.4 本地编译验证（macOS 无 MySQL，编译成功，HAS_MYSQL=false）

## 2. CI 验证

- [ ] 2.1 提交变更，等待 CI 运行
- [ ] 2.2 检查 CI 日志：确认 "MySQL connector 已启用" 或类似消息
- [ ] 2.3 确认 HAS_MYSQL=true（Ubuntu 环境）

## 3. 归档

- [ ] 3.1 运行 openspec archive 归档变更
- [ ] 3.2 推送归档变更到远程仓库