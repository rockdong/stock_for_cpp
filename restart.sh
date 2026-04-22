#!/bin/bash
set -e

# ========================================
# 股票分析系统服务重启脚本
# ========================================

# 时间戳输出函数
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# 等待 MySQL 健康检查
wait_mysql_healthy() {
    local max_wait=90
    local waited=0
    local interval=5
    
    log "等待 MySQL 健康检查..."
    
    while [ $waited -lt $max_wait ]; do
        if docker exec stock-mysql mysqladmin ping -h localhost -u root -pstock_root_password --silent 2>/dev/null; then
            log "✓ MySQL 已就绪 (等待 ${waited}s)"
            return 0
        fi
        sleep $interval
        waited=$((waited + interval))
        log "  等待中... (${waited}s/${max_wait}s)"
    done
    
    log "⚠️ MySQL 健康检查超时 (${max_wait}s)，继续等待 docker-compose 配置生效..."
    return 0
}

# 启动 Docker 监控（如果脚本存在）
start_docker_monitor() {
    local monitor_script="$HOME/scripts/docker-monitor.sh"
    
    if [ -f "$monitor_script" ] && [ -x "$monitor_script" ]; then
        log "启动 Docker 事件监控..."
        "$monitor_script" start
        log "✓ 监控已启动"
    else
        log "监控脚本未找到，跳过"
    fi
}

# ========================================
# 主流程
# ========================================

log "========================================"
log "股票分析系统服务重启"
log "========================================"

# 显示当前状态
log "当前容器状态:"
docker-compose ps 2>/dev/null || log "  无运行容器"
echo ""

# 停止服务
log "停止所有服务..."
docker-compose down

# 停止旧的监控进程
pkill -f "docker-events-monitor.sh" 2>/dev/null || true
log "✓ 已停止旧监控进程"

echo ""

# 启动所有服务
log "启动所有服务..."
docker-compose up -d

echo ""

# 等待 MySQL 健康
wait_mysql_healthy

# 启动 Docker 监控
start_docker_monitor

echo ""
log "========================================"
log "服务启动完成！"
log "========================================"

# 显示最终状态
docker-compose ps

echo ""
log "访问地址:"
log "  飞书 Bot:   http://localhost:3000"
log "  REST API:  http://localhost:3001"
log "  Web前端:   http://localhost:80"
log "  Adminer:   http://localhost:8080"
log "  Docker日志: ~/docker-logs/"
log "========================================"