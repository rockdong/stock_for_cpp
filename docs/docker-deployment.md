# Docker 部署指南

## 🎯 概述

本指南说明如何使用 Docker Compose 部署股票分析系统，包含 MySQL 数据库持久化。

## 📁 服务架构

系统包含以下服务：

| 服务名称 | 容器名称 | 说明 |
|----------|----------|------|
| stock-cpp | stock-cpp-engine | C++ 分析引擎 |
| mysql | stock-mysql | MySQL 8.0 数据库 |
| stock-bot | stock-feishu-bot | 飞书推送服务 (端口 3000) |
| api-server | stock-api-server | NestJS API 服务 (端口 3001) |
| web-frontend | stock-web-frontend | Web 前端 (端口 8880) |

## 🚀 生产环境部署

### 基础运行命令

```bash
# 1. 克隆项目
git clone https://github.com/rockdong/stock_for_cpp.git
cd stock_for_cpp

# 2. 配置环境变量
cp env/.env.example env/.env
vim env/.env  # 填写 Tushare API Key 和飞书配置

# 3. 启动所有服务
docker-compose up -d

# 4. 验证运行状态
docker-compose ps
docker-compose logs -f stock-cpp
```

### 服务访问

- **Web 前端**: http://localhost:8880
- **飞书推送 API**: http://localhost:3000
- **NestJS API**: http://localhost:3001
- **MySQL**: localhost:3306

## 🛠️ 配置说明

### 环境变量

主要配置项（在 `env/.env` 中设置）：

```bash
# 数据源配置 (Tushare Pro API)
DATA_SOURCE_URL=http://api.tushare.pro
DATA_SOURCE_API_KEY=your_tushare_token_here

# 飞书机器人配置
FEISHU_APP_ID=your_app_id_here
FEISHU_APP_SECRET=your_app_secret_here
FEISHU_VERIFICATION_TOKEN=your_verification_token_here

# 策略配置
STRATEGIES=EMA25_GREATER_17_PRICE_MATCH

# 调度配置
SCHEDULER_EXECUTE_TIME=20:00

# 数据库配置（Docker Compose 中已预设）
DB_TYPE=mysql
DB_HOST=mysql
DB_PORT=3306
DB_NAME=stock_db
DB_USER=stock_user
DB_PASSWORD=stock_password
```

### 数据库配置

Docker Compose 中预设的 MySQL 配置：

| 配置项 | 值 |
|--------|-----|
| 数据库名 | stock_db |
| 用户名 | stock_user |
| 密码 | stock_password |
| Root 密码 | stock_root_password |

## 📋 常用操作命令

### 服务管理

```bash
# 启动服务
docker-compose up -d

# 停止服务
docker-compose down

# 重启特定服务
docker-compose restart stock-cpp

# 查看服务状态
docker-compose ps

# 查看服务日志
docker-compose logs -f stock-cpp
docker-compose logs -f api-server
docker-compose logs -f stock-bot
```

### 数据库管理

```bash
# 进入 MySQL 容器
docker exec -it stock-mysql mysql -u root -pstock_root_password

# 备份数据库
docker exec stock-mysql mysqldump -u root -pstock_root_password stock_db > backup.sql

# 恢复数据库
docker exec -i stock-mysql mysql -u root -pstock_root_password stock_db < backup.sql

# 查看数据库表
docker exec -it stock-mysql mysql -u stock_user -pstock_password stock_db -e "SHOW TABLES;"
```

### 日志管理

```bash
# 查看实时日志
docker-compose logs -f

# 查看特定时间范围的日志
docker-compose logs --since="2024-01-01T00:00:00" stock-cpp

# 查看最近 100 行日志
docker-compose logs --tail=100 stock-cpp
```

## 🔧 资源配置

Docker Compose 中已配置资源限制：

| 服务 | 内存限制 | 内存预留 |
|------|----------|----------|
| stock-cpp | 128M | 32M |
| mysql | 256M | 128M |
| stock-bot | 64M | 16M |
| api-server | 96M | 32M |
| web-frontend | 32M | 8M |

可根据实际情况调整 `docker-compose.yml` 中的资源配置。

## 🚨 故障排除

### 常见问题解决方案

#### 1. MySQL 连接失败

```bash
# 检查 MySQL 是否健康
docker-compose ps mysql

# 查看 MySQL 日志
docker-compose logs mysql

# 等待 MySQL 完全启动（健康检查）
docker-compose up -d --wait
```

#### 2. 服务无法启动

```bash
# 检查容器状态
docker-compose ps -a

# 查看错误日志
docker-compose logs --tail=50 <service-name>

# 重新构建镜像
docker-compose build --no-cache
docker-compose up -d
```

#### 3. 端口冲突

```bash
# 查看端口占用
netstat -tlnp | grep -E '3000|3001|8880|3306'

# 修改 docker-compose.yml 中的端口映射
# 例如将 "3000:3000" 改为 "3002:3000"
```

#### 4. 数据库权限问题

```bash
# 重置 MySQL 用户权限
docker exec -it stock-mysql mysql -u root -pstock_root_password -e "
  GRANT ALL PRIVILEGES ON stock_db.* TO 'stock_user'@'%';
  FLUSH PRIVILEGES;
"
```

## 🔄 更新部署

```bash
# 1. 拉取最新镜像
docker-compose pull

# 2. 停止并删除旧容器
docker-compose down

# 3. 启动新容器
docker-compose up -d

# 4. 验证更新
docker-compose logs -f stock-cpp
```

## 📊 监控和维护

### 健康检查

```bash
# 检查所有服务健康状态
docker-compose ps

# 检查 MySQL 健康
docker exec stock-mysql mysqladmin ping -h localhost -u root -pstock_root_password

# 检查 API 服务
curl http://localhost:3001/health
```

### 资源监控

```bash
# 查看资源使用情况
docker stats --no-stream

# 查看特定服务资源
docker stats stock-cpp stock-mysql api-server --no-stream
```

## 🔒 安全建议

1. **修改默认密码**: 生产环境中修改 MySQL 密码
2. **限制端口暴露**: 仅暴露必要端口
3. **使用环境变量**: 不要在配置文件中硬编码敏感信息
4. **定期备份数据**: 定期备份 MySQL 数据

---

## 📞 技术支持

如果遇到问题，请：
1. 检查容器日志：`docker-compose logs <service-name>`
2. 验证配置：`docker-compose config`
3. 检查健康状态：`docker-compose ps`
4. 提交 Issue 到项目仓库

**更新时间：2026年5月4日**