## Why

当前Docker容器存在数据持久化问题：
1. **日志无法持久化**：容器内的logs/app.log在容器删除后丢失，无法进行问题排查
2. **数据库无法备份**：stock.db文件存储在容器内，无法在宿主机直接备份和迁移
3. **维护困难**：无法在宿主机直接查看日志文件或操作数据库

同时必须保持**向后兼容性**：C++和Node.js服务在本地运行时必须继续使用现有相对路径配置。

## What Changes

- **挂载日志目录**：将容器内/app/logs挂载到宿主机./logs目录
- **挂载数据库文件**：将容器内/app/stock.db挂载到宿主机./stock.db
- **优化supervisor日志配置**：将supervisor管理的服务日志重定向到标准输出
- **保持相对路径**：不修改C++和Node.js代码中的相对路径配置

## 新增能力

- `data-persistence`: 容器重启和删除后数据保持完整
- `log-accessibility`: 宿主机可直接查看和备份日志文件
- `database-operations`: 宿主机可直接操作数据库文件
- `development-compatibility`: 保持本地开发环境的相对路径配置

## 修改能力

- `container-configuration`: 修改Docker运行时的挂载配置
- `supervisor-logging`: 优化supervisor日志收集策略

## 影响范围

- **Docker运行方式**：需要添加-v挂载参数，影响部署脚本
- **supervisor配置**：优化日志重定向配置，影响Dockerfile
- **容器启动流程**：需要确保挂载目录存在，可能涉及启动脚本
- **CI/CD配置**：GitHub Actions中的docker运行命令需要更新