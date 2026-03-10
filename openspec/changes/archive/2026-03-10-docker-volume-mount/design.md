## 背景

当前Docker容器的数据存储完全在容器内部：
- 日志文件：/app/logs/app.log (容器内)
- 数据库文件：/app/stock.db (容器内)
- Supervisor日志：标准输出 (容器内)

这导致数据无法持久化，违反了"数据与代码分离"的容器化最佳实践。

## 目标/非目标

**目标**:
- 实现日志文件和数据库文件的宿主机持久化
- 保持C++和Node.js的相对路径配置不变
- 优化日志收集策略，便于运维管理
- 支持开发环境和生产环境的不同挂载需求

**非目标**:
- 修改应用代码内部的相对路径逻辑
- 改变数据库schema或数据结构
- 实现完整的日志聚合系统

## 技术决策

### 决策1: 挂载策略选择
**选择**: 使用Bind Mount而非Volume
**理由**: 
- 直接映射到项目目录，便于开发时访问
- 支持git跟踪目录结构变更
- 备份和管理更直观

**挂载映射**:
```
./logs/:/app/logs/
./stock.db:/app/stock.db
```

### 决策2: 日志管理策略
**选择**: Supervisor日志重定向到标准输出 + C++日志文件持久化
**理由**:
- 标准输出可被docker logs捕获，便于日志聚合
- C++应用日志文件独立存储，便于分析和备份
- 不同服务日志分离，便于问题定位

**配置方案**:
```bash
stdout_logfile=/dev/stdout
redirect_stderr=true
```

### 决策3: 路径兼容性保障
**选择**: 通过挂载点映射保持相对路径有效性
**理由**:
- C++工作目录:/app，相对路径./stock.db → /app/stock.db
- Node.js工作目录:/app/nodejs，相对路径../stock.db → /app/stock.db
- 挂载映射确保两者指向宿主机同一文件

### 决策4: 容器启动优化
**选择**: 创建启动脚本确保挂载目录存在
**理由**:
- 防止因挂载目录不存在导致启动失败
- 提供优雅的错误处理和日志记录

## 实施架构

```
宿主机目录结构:
project/
├── logs/              ← 宿主机日志目录
│   └── app.log        ← C++应用日志
├── stock.db           ← 共享数据库文件
└── docker-compose.yml (可选)

容器内映射:
/app/logs/     ←→ ./logs/
/app/stock.db  ←→ ./stock.db

服务访问:
- C++: /app/stock.db (相对路径 ./stock.db)
- Node.js: /app/stock.db (相对路径 ../stock.db)
```

## 风险评估

### 高风险
- **权限问题**: 容器内appuser可能无法访问宿主机文件
- **路径冲突**: 开发环境和容器环境的路径访问权限不一致

### 中风险
- **性能影响**: Bind mount可能影响I/O性能
- **备份复杂性**: 需要同时管理宿主机和容器内的数据

### 低风险
- **配置错误**: 挂载路径配置错误导致启动失败

## 监控和验证

### 成功指标
- 容器重启后数据完整性保持
- 宿主机可直接访问日志和数据库文件
- 本地开发环境相对路径正常工作

### 验证方法
```bash
# 验证数据持久化
docker restart <container>
ls -la ./logs/ && ls -la ./stock.db

# 验证相对路径兼容性
cd cpp && ./build/stock_for_cpp --test
cd nodejs && node src/index.js --test
```