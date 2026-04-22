## Context

当前 `ConnectionFactory.cpp` 在编译时通过 `#ifdef HAS_MYSQL` 检查 MySQL 支持是否可用。如果 `HAS_MYSQL` 未定义（MySQL 开发库未找到），代码会在运行时检测 `DB_TYPE=mysql` 后自动降级使用 SQLite：

```cpp
#else
    if (dbType == "mysql") {
        LOG_WARN("MySQL 支持未编译，降级使用 SQLite");  // ← 要移除的降级逻辑
        LOG_WARN("请安装 MySQL 库并重新编译以启用 MySQL 支持");
    }
#endif
```

用户期望这种情况下直接失败，而非静默降级。

## Goals / Non-Goals

**Goals:**
- 移除降级 fallback，当 MySQL 配置但不支持时抛出异常
- 提供清晰可操作的错误信息
- 确保用户能明确知道系统使用的数据库类型

**Non-Goals:**
- 不改变 MySQL/SQLite 的正常连接逻辑
- 不影响 `DB_TYPE=sqlite` 或未配置时的默认行为
- 不增加自动检测或更复杂的编译配置

## Decisions

### Decision 1: 使用异常而非返回错误

**选择**: 抛出 `std::runtime_error` 终止启动

**理由**:
- `ConnectionFactory::createConnection()` 返回 `IConnection&` 引用，无法返回 nullptr 表示失败
- 启动阶段数据库连接失败通常意味着系统无法工作，终止是合理行为
- 异常能携带详细错误信息，便于用户诊断

**备选方案**:
- 返回 `std::unique_ptr<IConnection>` 并允许 nullptr → 需要修改接口签名，影响所有调用方
- 返回特殊错误连接对象 → 过度复杂，增加新类

### Decision 2: 错误信息内容

**选择**: 包含具体的安装和编译指令

**理由**:
- 用户可能不清楚如何启用 MySQL 支持
- 直接提供命令示例减少用户搜索成本

**错误信息格式**:
```
DB_TYPE=mysql 但 MySQL 支持未编译
请安装 MySQL 开发库：
  - Ubuntu: sudo apt-get install libmysqlclient-dev
  - macOS: brew install mysql-client
然后重新编译：cmake -S cpp -B cpp/build && cmake --build cpp/build
```

### Decision 3: 最小化代码改动

**选择**: 仅修改 `ConnectionFactory.cpp` 第 33-37 行

**理由**:
- 改动范围小，风险可控
- 不影响其他模块的连接逻辑
- 便于测试和验证

## Risks / Trade-offs

### Risk: 用户误配置导致启动失败
- **Risk**: 用户配置了 `DB_TYPE=mysql` 但未安装 MySQL 库，程序直接退出
- **Mitigation**: 错误信息提供清晰的安装指令；README 中说明 MySQL 配置要求

### Risk: CI/CD 编译环境差异
- **Risk**: macOS 本地可能没有 MySQL 库，CI Ubuntu 已安装 `libmysqlclient-dev`
- **Mitigation**: CI 已配置 MySQL 编译支持（build.yml 第 34 行）；本地开发按错误提示安装即可

### Trade-off: 破坏性变更
- **Trade-off**: 原本降级运行的配置现在会启动失败
- **Acceptable**: 这是用户明确要求的行为；降级本身就是误导性的设计