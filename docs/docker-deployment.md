# Docker 挂载部署指南

## 🎯 概述

本指南说明如何使用新的Docker挂载配置，实现数据和日志的持久化。

## 📁 目录结构

在项目根目录准备以下目录结构：
```
stock_for_cpp/
├── logs/                   ← 日志文件目录（宿主机）
├── stock.db               ← 数据库文件（宿主机）
├── cpp/
├── nodejs/
├── Dockerfile
└── docker-entrypoint.sh
```

## 🚀 生产环境部署

### 基础运行命令

```bash
# 1. 确保目录存在
mkdir -p logs
touch stock.db

# 2. 运行容器（推荐）
docker run -d \
  --name stock-analysis \
  -v $(pwd)/stock.db:/app/stock.db \
  -v $(pwd)/logs:/app/logs \
  -p 3000:3000 \
  --restart unless-stopped \
  rockdong/stock_for_cpp

# 3. 验证运行状态
docker logs -f stock-analysis
```

### 高级配置选项

```bash
# 带资源限制的运行命令
docker run -d \
  --name stock-analysis \
  --memory=512m \
  --cpus=1.0 \
  -v $(pwd)/stock.db:/app/stock.db \
  -v $(pwd)/logs:/app/logs \
  -p 3000:3000 \
  --restart unless-stopped \
  --health-cmd="curl -f http://localhost:3000/health || exit 1" \
  --health-interval=30s \
  rockdong/stock_for_cpp
```

## 🛠️ 开发环境部署

### docker-compose 配置

创建 `docker-compose.yml` 文件：

```yaml
version: '3.8'

services:
  stock-analysis:
    image: rockdong/stock_for_cpp
    container_name: stock-analysis-dev
    volumes:
      - ./stock.db:/app/stock.db
      - ./logs:/app/logs
    ports:
      - "3000:3000"
    environment:
      - TZ=Asia/Shanghai
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:3000/health"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s

  # 可选：添加数据库管理工具
  sqlite-browser:
    image: coleifer/sqlite-web
    container_name: stock-db-browser
    volumes:
      - ./stock.db:/data/stock.db
    ports:
      - "8080:8080"
    environment:
      - SQLITE_DATABASE=stock.db
    restart: unless-stopped
```

### 开发环境快速启动

```bash
# 1. 启动开发环境
docker-compose up -d

# 2. 查看日志
docker-compose logs -f stock-analysis

# 3. 访问数据库管理界面
# 浏览器访问: http://localhost:8080

# 4. 停止服务
docker-compose down
```

## 📋 常用操作命令

### 日志管理
```bash
# 查看实时日志
docker logs -f stock-analysis

# 查看特定时间范围的日志
docker logs --since="2024-01-01T00:00:00" stock-analysis

# 查看宿主机日志文件
tail -f logs/app.log

# 清理历史日志
find logs/ -name "*.log" -mtime +7 -delete
```

### 数据库管理
```bash
# 备份数据库
cp stock.db stock.db.backup.$(date +%Y%m%d)

# 在宿主机直接操作数据库
sqlite3 stock.db
> .tables
> .quit

# 压缩历史数据库
gzip stock.db.backup.$(date +%Y%m%d)
```

### 容器维护
```bash
# 检查容器状态
docker ps -a
docker stats stock-analysis

# 重启容器
docker restart stock-analysis

# 进入容器调试
docker exec -it stock-analysis /bin/bash

# 更新镜像
docker pull rockdong/stock_for_cpp
docker stop stock-analysis
docker rm stock-analysis
# 然后重新运行部署命令
```

## 🔧 环境变量配置

### 可选环境变量

```bash
# 运行时添加环境变量
docker run -d \
  --name stock-analysis \
  -v $(pwd)/stock.db:/app/stock.db \
  -v $(pwd)/logs:/app/logs \
  -p 3000:3000 \
  -e TZ=Asia/Shanghai \
  -e LOG_LEVEL=INFO \
  -e NODE_ENV=production \
  rockdong/stock_for_cpp
```

## 👥 用户和权限

### 宿主机权限设置
```bash
# 设置适当的目录权限
chmod 755 logs
chmod 644 stock.db

# 如果遇到权限问题，可以调整
sudo chown -R $USER:$USER logs stock.db
```

### 容器内用户
- 容器内运行用户：`appuser`
- UID：1000，GID：1000
- 如需权限映射，可使用 `--user` 参数

## 📊 监控和维护

### 健康检查
```bash
# 检查容器健康状态
docker inspect stock-analysis | grep 'Health' -A 10

# 手动健康检查
curl -f http://localhost:3000/health
```

### 资源监控
```bash
# 查看资源使用情况
docker stats stock-analysis --no-stream

# 查看磁盘使用情况
docker exec stock-analysis df -h
```

### 日志轮转
Dockerfile中已配置：
- C++应用日志：10MB单文件，保留3个
- Supervisor日志：输出到标准输出，由Docker管理

## 🔒 安全考虑

```bash
# 以非root用户运行（默认）
docker run --user 1000:1000 ... 

# 只读挂载数据库文件
docker run -v $(pwd)/stock.db:/app/stock.db:ro ...

# 网络隔离
docker run --network none ...  # 如不需要外部访问
```

## 🚨 故障排除

### 常见问题解决方案

#### 1. 权限被拒绝
```bash
# 错误信息：Permission denied
# 解决方案：
sudo chown -R 1000:1000 logs stock.db
```

#### 2. 数据库锁定
```bash
# 错误信息：database is locked
# 解决方案：
docker restart stock-analysis
```

#### 3. 挂载目录不存在
```bash
# 容器无法启动
# 解决方案：确保目录存在
mkdir -p logs
touch stock.db
```

#### 4. 端口冲突
```bash
# 错误信息：port already in use
# 解决方案：使用不同端口
docker run -p 3001:3000 ...
```

---

## 📞 技术支持

如果遇到问题，请：
1. 检查容器日志：`docker logs stock-analysis`
2. 查看系统日志：`tail -f logs/app.log`
3. 验证配置：`docker inspect stock-analysis`
4. 提交Issue到项目仓库

**更新时间：2025年3月11日**