#!/bin/bash
set -e

# ========================================
# 股票分析系统镜像更新脚本
# ========================================

# 时间戳输出函数
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# 显示镜像版本
show_image_versions() {
    log "当前镜像版本:"
    docker images --format "table {{.Repository}}\t{{.Tag}}\t{{.ID}}\t{{.CreatedAt}}" | grep -E "rockdong13|REPOSITORY" || log "  无相关镜像"
}

# 保存镜像 ID 用于对比
save_image_ids() {
    docker images --format "{{.Repository}}:{{.Tag}} {{.ID}}" | grep "rockdong13" > /tmp/old_image_ids.txt 2>/dev/null || true
}

# 显示版本变化
show_version_changes() {
    log "镜像版本对比:"
    
    if [ ! -f /tmp/old_image_ids.txt ]; then
        log "  无旧镜像记录"
        return
    fi
    
    echo "┌─────────────────────────────────────────────────────────────────────────────────────────────┐"
    echo "│ 镜像                          │ 旧 ID          │ 新 ID          │ 状态       │"
    echo "├─────────────────────────────────────────────────────────────────────────────────────────────┤"
    
    while IFS= read -r line; do
        image_name=$(echo "$line" | cut -d' ' -f1)
        old_id=$(echo "$line" | cut -d' ' -f2)
        
        new_id=$(docker images --format "{{.ID}}" "$image_name" 2>/dev/null | head -1)
        
        if [ "$old_id" = "$new_id" ]; then
            status="SAME"
        else
            status="CHANGED"
        fi
        
        printf "│ %-30s │ %-15s │ %-15s │ %-10s │\n" "$image_name" "$old_id" "$new_id" "$status"
    done < /tmp/old_image_ids.txt
    
    echo "└─────────────────────────────────────────────────────────────────────────────────────────────┘"
    
    rm -f /tmp/old_image_ids.txt
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
log "股票分析系统镜像更新"
log "========================================"

# 显示当前镜像版本
show_image_versions

# 保存旧镜像 ID
save_image_ids

echo ""

log "停止所有服务..."
docker-compose down

log "清理冲突容器..."
for container in stock-mysql stock-cpp-engine stock-feishu-bot stock-api-server stock-web-frontend stock-db-admin stock-log-monitor; do
    if docker ps -a --format '{{.Names}}' | grep -q "^${container}$"; then
        docker rm -f "$container" 2>/dev/null || true
        log "  ✓ 已清理: $container"
    fi
done
log "✓ 服务已停止"

echo ""

# 拉取最新镜像
log "拉取最新镜像..."
if ! docker-compose pull; then
    log "❌ 镜像拉取失败，保留旧镜像可继续运行"
    rm -f /tmp/old_image_ids.txt
    exit 1
fi
log "✓ 镜像拉取完成"

echo ""

# 显示版本变化
show_version_changes

echo ""

# 启动服务
log "启动所有服务..."
docker-compose up -d

echo ""

# 等待 MySQL 健康
wait_mysql_healthy

echo ""

# 清理悬空镜像
log "清理悬空镜像..."
pruned=$(docker image prune -f 2>&1 | grep "Total reclaimed space" || echo "清理完成")
log "✓ $pruned"

echo ""

# 启动 Docker 监控
start_docker_monitor

echo ""
log "========================================"
log "更新完成！"
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