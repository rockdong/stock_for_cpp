## Context

当前项目使用 Docker Compose 管理 6 个微服务容器。运维依赖两个 shell 脚本：

- `restart.sh` - 重启所有服务，但缺乏错误处理和健康验证
- `cleanDocker.sh` - 清理所有容器和镜像（过于激进，已废弃）

开发团队需要更可靠的运维脚本来支持日常更新和故障恢复。

**约束条件：**
- 脚本必须在 macOS 和 Linux 上运行
- 不能依赖额外工具（仅使用 docker CLI）
- MySQL 数据必须保留（volume 不能清除）
- 必须与现有 docker-compose.yml 兼容

## Goals / Non-Goals

**Goals:**
- 提供可靠的服务重启流程，包含健康验证
- 提供便捷的镜像更新流程，包含版本追踪
- 正确显示服务端口信息
- 详细输出便于故障排查

**Non-Goals:**
- 不改变 docker-compose.yml 配置
- 不引入新的监控依赖
- 不支持回滚机制（可后续扩展）
- 不处理多环境部署（dev/prod）

## Decisions

### 1. 脚本独立性 vs 共享函数

**选择：脚本独立**
- **理由：** 两个脚本逻辑清晰，每个约 60-80 行。共享函数增加维护复杂度，收益不大。
- **备选：** 创建 `common-functions.sh` 被 source。但多文件依赖使部署更复杂。

### 2. 健康检查策略

**选择：仅等待 MySQL 健康**
- **理由：** MySQL 是核心依赖，其他服务在 docker-compose.yml 中已配置 `depends_on: mysql: condition: service_healthy`。
- **备选：** 逐个等待所有服务。但 docker-compose 已处理依赖顺序，过度检查冗余。

### 3. 镜像清理时机

**选择：启动成功后自动清理**
- **理由：** `docker image prune -f` 仅清理悬空镜像，不影响运行容器。自动化减少手动步骤。
- **备选：** 询问用户确认。增加交互打断自动化流程，CI/CD 不友好。

### 4. 错误处理策略

**选择：`set -e` + 关键步骤验证**
- **理由：** 任何命令失败立即中止，避免部分成功导致的半启动状态。
- **备选：** 继续执行并汇总错误。可能导致服务不一致状态。

### 5. 输出格式

**选择：详细输出 + 时间戳**
- **理由：** 便于排查问题，日志可追溯。
- **备选：** 简洁输出。但运维场景需要详细信息。

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| Docker Hub 连接失败导致拉取中断 | 中止流程，保留旧镜像可继续运行 |
| MySQL 健康检查超过 90 秒 | 继续等待，依赖 docker-compose 配置（最长 80s） |
| 监控脚本不存在 | 静默跳过 + 提示，不阻断主流程 |
| 用户误删数据（使用 -v 参数） | 默认不使用 -v，文档明确说明 |

## Migration Plan

1. 创建 `update.sh` 新文件
2. 重写 `restart.sh`（保留原文件名）
3. 测试验证两个脚本功能
4. 提交变更

**回滚策略：** Git checkout 恢复原 restart.sh，删除 update.sh

## Open Questions

无待解决问题。