## Why

当前 CI 环境已安装 `libmysqlclient-dev`，但 CMake 无法检测到 MySQL 库，导致 `HAS_MYSQL` 未定义，程序运行时遇到 `DB_TYPE=mysql` 会抛出 "MySQL 支持未编译" 异常。

根本原因：CMakeLists.txt 第63行强制禁用了 sqlpp11 的 MySQL connector (`BUILD_MYSQL_CONNECTOR=OFF`)，导致 sqlpp11 不执行 MySQL 检测逻辑。同时项目第67行手动调用 `find_package(MySQL)` 时，`CMAKE_MODULE_PATH` 不包含 sqlpp11 的 `FindMySQL.cmake` 模块路径，所以检测失败。

## What Changes

- 启用 sqlpp11 的 MySQL connector：`BUILD_MYSQL_CONNECTOR=ON`
- 设置 `DEPENDENCY_CHECK=OFF` 避免找不到 MySQL 时 CMake 配置失败
- 简化 MySQL 检测逻辑：使用 `if(TARGET sqlpp11::mysql)` 替代手动 `find_package(MySQL)`
- 删除第63行强制禁用 MySQL connector 的代码
- 删除第67-74行的手动 MySQL 检测代码

## Capabilities

### New Capabilities
- 无（这是修复现有功能，而非新增能力）

### Modified Capabilities
- `mysql-connection`: 修复 MySQL 编译支持检测逻辑，确保 CI 环境能正确启用 MySQL connector

## Impact

- **代码**：`cpp/CMakeLists.txt` 第 63-74 行
- **CI 环境**：Ubuntu + libmysqlclient-dev → MySQL connector 启用 → 程序可用 MySQL
- **本地开发**：macOS 无 MySQL → MySQL connector 未启用 → 程序遇 `DB_TYPE=mysql` 正常抛异常
- **运行时行为**：无变化（严格失败模式已在上一个变更实现）