## Why

现有 `restart.sh` 缺乏错误处理和健康验证，重启后无法确认服务是否真正可用。端口显示信息与实际不符，造成使用困惑。缺少 `update.sh` 导致每次更新镜像需要手动执行多个命令，流程繁琐且易遗漏步骤。

## What Changes

### restart.sh 优化
- 添加 `set -e` 错误处理机制
- 增加服务停止前的状态显示
- 实现 MySQL 健康检查等待（最长 90 秒）
- 修正端口信息显示（3000=飞书Bot, 3001=API, 80=Web前端, 8080=Adminer）
- 增加监控脚本存在性检查
- 添加详细输出和时间戳

### update.sh 新增
- 显示当前镜像版本（镜像 ID 和创建时间）
- 执行 `docker-compose down` 停止服务
- 执行 `docker-compose pull` 拉取最新镜像
- 对比显示更新前后版本变化
- 执行 `docker-compose up -d` 启动服务
- MySQL 健康检查等待
- 执行 `docker image prune -f` 清理悬空镜像
- 启动 Docker 监控（脚本存在时）
- 显示最终状态和访问地址

## Capabilities

### New Capabilities
- `docker-update`: Docker 镜像更新脚本，支持版本对比、健康验证和自动清理

### Modified Capabilities
- `docker-restart`: 增强重启脚本的可靠性和信息准确性

## Impact

- **文件变更**: 
  - `restart.sh` - 重写，保留功能但增强可靠性
  - `update.sh` - 新建
  
- **依赖**: 无新增依赖，使用现有 docker-compose 和 docker CLI
  
- **向后兼容**: 完全兼容，不影响现有部署流程