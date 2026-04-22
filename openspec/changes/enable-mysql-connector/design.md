## Context

项目使用 sqlpp11 库处理数据库操作。sqlpp11 自 0.61 版本后，MySQL connector 是 header-only 库，通过 `BUILD_MYSQL_CONNECTOR` 选项控制是否创建 `sqlpp11::mysql` target。

当前问题：
- CMakeLists.txt 强制禁用 `BUILD_MYSQL_CONNECTOR=OFF`
- 手动 `find_package(MySQL)` 找不到库（缺少 FindMySQL.cmake 模块）
- CI 已安装 libmysqlclient-dev 但未被检测到

## Goals / Non-Goals

**Goals:**
- 启用 sqlpp11 的 MySQL connector 构建逻辑
- 利用 sqlpp11 的 FindMySQL.cmake 正确检测 MySQL 库
- CI 环境正确启用 MySQL 支持
- 本地无 MySQL 时仍能正常编译（不启用 MySQL）

**Non-Goals:**
- 不改变运行时的严格失败模式（上一个变更已实现）
- 不强制要求所有环境安装 MySQL
- 不修改 MySQL 连接逻辑

## Decisions

### Decision 1: 使用 sqlpp11 的 MySQL connector 构建

**选择**: 设置 `BUILD_MYSQL_CONNECTOR=ON` 而非手动检测

**理由**:
- sqlpp11 有完整的 FindMySQL.cmake 模块，知道如何搜索 MySQL 头文件和库
- sqlpp11 会自动创建 `sqlpp11::mysql` target 并链接正确依赖
- connector 是 header-only，不需要额外编译步骤

**备选方案**:
- 设置 `CMAKE_MODULE_PATH` 包含 sqlpp11 模块目录 → 需要 extra 配置，不如直接用 sqlpp11 的逻辑
- 完全移除 MySQL 支持 → 用户需要 MySQL 功能

### Decision 2: 设置 DEPENDENCY_CHECK=OFF

**选择**: 非严格依赖检查模式

**理由**:
- sqlpp11 默认 `DEPENDENCY_CHECK=ON`，找不到 MySQL 会 CMake 失败
- 我们希望本地无 MySQL 时也能编译（只是不启用 MySQL connector）
- `DEPENDENCY_CHECK=OFF` 允许可选 connector 不存在时继续构建

**风险**: 所有 connector 依赖检查都禁用，包括 SQLite3
**Acceptable**: SQLite3 在 Ubuntu 上通常已安装，CI 已验证；本地 macOS 需确认

### Decision 3: 使用 TARGET 检测替代 find_package 结果

**选择**: `if(TARGET sqlpp11::mysql)` 替代 `if(MySQL_FOUND)`

**理由**:
- Target 存在是最终结果，比中间变量更可靠
- 避免依赖 find_package 的具体行为
- 与 sqlpp11 内部逻辑一致

## Risks / Trade-offs

### Risk: DEPENDENCY_CHECK=OFF 影响 SQLite3 检测
- **Risk**: 关闭依赖检查可能导致其他 connector 检测也被跳过
- **Mitigation**: CI 环境已验证 SQLite3 存在；可在 CMake 输出中确认 SQLite3 connector 状态

### Risk: macOS 本地开发无 MySQL 时编译行为
- **Risk**: 本地 macOS 没有 MySQL，但 CMake 不会失败
- **Mitigation**: 这正是期望行为；运行时严格失败模式会阻止错误使用