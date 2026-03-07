# 快速参考 - 重要技术决策

## ⚠️ HTTP-Only 架构

**本项目只使用 HTTP，不支持 HTTPS，不依赖 OpenSSL**

### 为什么？
- Tushare Pro API 使用 HTTP 协议（`http://api.waditu.com`）
- 避免 OpenSSL 依赖和架构冲突问题
- 简化编译和部署

### 如何实现？
```cmake
# CMakeLists.txt
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT=0)
```

### 注意事项
- ⚠️ 不能访问 HTTPS API
- ⚠️ 如需 HTTPS，需重新启用 SSL 并链接 OpenSSL

## 📋 快速开始

### 1. 配置 API Token
```bash
cp env.example .env
# 编辑 .env，设置 DATA_SOURCE_API_KEY
```

### 2. 编译项目
```bash
cmake -S . -B build
cmake --build build
```

### 3. 运行程序
```bash
./build/stock_for_cpp
```

## 🔧 已完成模块

1. **日志系统** (`logger::`) - 完整的日志功能
2. **配置系统** (`config::`) - 统一配置管理
3. **网络层** (`network::`) - HTTP 客户端 + Tushare API

## 📚 文档位置

- 设计文档：`DESIGN.md`
- 日志系统：`log/README.md`
- 配置系统：`config/README.md`
- 网络层：`network/README.md`

## 🎯 下一步开发

- 数据层（数据库、缓存）
- 核心业务层（股票管理）
- 分析层（技术指标）

