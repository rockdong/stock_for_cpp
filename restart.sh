#!/bin/bash

echo "停止服务..."
docker-compose down

# 停止旧的监控进程
pkill -f "docker-events-monitor.sh" 2>/dev/null

echo "启动所有服务..."
docker-compose up -d

# 启动 Docker events 监控
echo "启动 Docker 事件监控..."
~/scripts/docker-monitor.sh start

echo ""
echo "========================================"
echo "服务启动完成！"
echo "========================================"
docker-compose ps
echo ""
echo "访问地址："
echo "  主服务:     http://localhost:3000"
echo "  Web前端:    http://localhost:5173"
echo "  Docker日志: ~/docker-logs/"
echo "========================================"