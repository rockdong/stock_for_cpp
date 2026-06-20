# HTTPS 协议支持问题修复指南

**问题**: 数据源初始化失败 - 只支持 HTTP 协议，不支持 HTTPS
**日期**: 2026-06-20
**修复状态**: ✅ 已修复

---

## 问题描述

### 错误日志

```
[error] 只支持 HTTP 协议，不支持: https
[error] 数据源初始化异常: Only HTTP protocol is supported, got: https
[warning] 系统将在降级模式下运行（无数据源）
```

### 根本原因

1. **MainScreen.cpp 硬编码 HTTPS URL**（第585行）
   ```cpp
   envFile << "DATA_SOURCE_URL=https://api.tushare.pro\n";
   ```

2. **HttpClient.cpp 只支持 HTTP 协议**（不支持 HTTPS）
   ```cpp
   // network/http/HttpClient.cpp 第26-29行
   if (protocol != "http") {
       LOG_ERROR("只支持 HTTP 协议，不支持: " + protocol);
       throw std::invalid_argument("Only HTTP protocol is supported, got: " + protocol);
   }
   ```

3. **配置文件被覆盖**
   - 用户在 TUI 中保存配置时，`.env` 文件被改成了 HTTPS URL
   - 即使源码 `.env` 配置为 HTTP，build 目录下的 `.env` 文件是 HTTPS

---

## 修复方案

### ✅ 已修复的文件

1. **MainScreen.cpp**（第585行）
   ```cpp
   // 修改前（硬编码 HTTPS）
   envFile << "DATA_SOURCE_URL=https://api.tushare.pro\n";

   // 修改后（使用 HTTP）
   envFile << "DATA_SOURCE_URL=http://api.waditu.com\n";  // 使用 HTTP 协议（HttpClient 不支持 HTTPS）
   ```

2. **源码 .env 配置文件**
   ```env
   # 修改前
   DATA_SOURCE_URL=http://api.tushare.pro

   # 修改后
   DATA_SOURCE_URL=http://api.waditu.com  # 使用 HTTP 协议（HttpClient 不支持 HTTPS）
   ```

3. **build 目录下的 .env 配置文件**
   ```env
   # 修改前（用户保存配置后生成）
   DATA_SOURCE_URL=https://api.tushare.pro

   # 修改后（手动修正）
   DATA_SOURCE_URL=http://api.waditu.com
   ```

### 为什么选择 `http://api.waditu.com`？

- Tushare API 官方提供 HTTP 接口：`http://api.waditu.com`
- 所有文档和示例都使用这个地址
- HttpClient 当前不支持 HTTPS，使用 HTTP 是正确选择

---

## 验证修复

### 测试步骤

1. **修改配置文件**（已完成）
   ```bash
   # 源码目录
   vim cpp/.env
   # DATA_SOURCE_URL=http://api.waditu.com

   # build 目录（重要！）
   vim cpp/build/stock_tui/.env
   # DATA_SOURCE_URL=http://api.waditu.com
   ```

2. **重新编译**（已完成）
   ```bash
   cd cpp/build
   cmake --build . --target stock_tui
   ```

3. **运行验证**（已完成）
   ```bash
   cd build/stock_tui
   ./stock_tui
   ```

### 成功日志

```
[info] 数据源配置:
[info]   URL: http://api.waditu.com
[info]   超时: 30秒
...
[info] TushareClient 初始化完成（从配置）
[info] TushareDataSource 初始化完成（从配置）
[info] 数据源初始化成功
```

**关键标志**: 不再出现 "只支持 HTTP 协议，不支持: https" 错误

---

## 技术说明

### 为什么 HttpClient 不支持 HTTPS？

**当前实现**:
- HttpClient.cpp 使用简单的 socket 编程
- 只实现了 HTTP 协议解析（明文传输）
- HTTPS 需要 SSL/TLS 加密，需要第三方库支持

**可能的改进方案**（未来可选）:
1. **使用 libcurl**（成熟方案，支持 HTTPS）
2. **使用 cpp-httplib**（现代 C++ HTTP 库，支持 HTTPS）
3. **使用 Boost.Asio + OpenSSL**（需要额外依赖）

**当前选择**:
- 保持简单：使用 HTTP 接口（`http://api.waditu.com`）
- Tushare API 官方支持 HTTP，无需 HTTPS
- 避免引入额外依赖，保持系统轻量

---

## 配置文件说明

### .env 文件位置

**重要**: 程序运行时会读取当前目录下的 `.env` 文件

```bash
# 源码目录（开发时配置）
cpp/.env

# build 目录（运行时实际读取）
cpp/build/stock_tui/.env  ← 程序启动时读取这个文件
```

### 配置优先级

1. **环境变量**（最高优先级）
   ```bash
   export DATA_SOURCE_API_KEY=your_api_key
   ```

2. **build 目录下的 .env 文件**（运行时读取）
   ```bash
   cpp/build/stock_tui/.env
   ```

3. **源码目录下的 .env 文件**（开发时配置）
   ```bash
   cpp/.env
   ```

### 配置编辑注意事项

**在 TUI 中保存配置时**:
- MainScreen.cpp 会生成新的 `.env` 文件
- **覆盖当前目录下的 `.env` 文件**
- 确保 MainScreen.cpp 使用正确的 URL（HTTP）

---

## 后续优化建议

### 1. 配置文件路径管理

**问题**: build 目录和源码目录各有一个 `.env` 文件

**建议**:
- 统一配置文件路径
- 在 CMake 中配置 `.env` 文件路径
- 或者在代码中使用绝对路径

### 2. HTTPS 支持（可选）

**如果需要 HTTPS**:
- 修改 HttpClient 使用 libcurl 或 cpp-httplib
- 添加 SSL/TLS 支持
- 支持 HTTPS 协议

**评估**:
- Tushare API 官方支持 HTTP（`http://api.waditu.com`）
- HTTPS 改动较大，引入额外依赖
- 当前 HTTP 方案可正常工作，无需立即改动

---

## 常见问题

### Q1: 为什么修改源码 `.env` 文件无效？

**原因**: 程序运行在 build 目录，读取的是 `build/stock_tui/.env`

**解决**: 同时修改两个 `.env` 文件，或配置文件路径

### Q2: 如何确认数据源正常工作？

**检查日志**:
```bash
cd build/stock_tui
./stock_tui 2>&1 | grep "数据源初始化"
```

**成功标志**: `数据源初始化成功`（无 HTTPS 错误）

### Q3: Tushare API 地址应该用哪个？

**推荐**: `http://api.waditu.com`（官方 HTTP 接口）

**原因**:
- HttpClient 不支持 HTTPS
- Tushare 官方文档使用此地址
- 所有示例配置都使用 HTTP

---

## 相关文件

### 修改的文件

- `cpp/stock_tui/components/MainScreen.cpp`（第585行）
- `cpp/.env`（配置文件）
- `cpp/build/stock_tui/.env`（运行时配置）

### 相关代码

- `cpp/network/http/HttpClient.cpp`（HTTP 客户端实现）
- `cpp/config/Config.cpp`（配置读取）
- `cpp/stock_core/StockCoreContext.cpp`（数据源初始化）

---

**最后更新**: 2026-06-20 11:43
**维护者**: Senior Developer（高级开发工程师）
**技术栈**: C++17 + FTXUI + HttpClient（HTTP only）