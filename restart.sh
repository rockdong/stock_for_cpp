#!/bin/bash

echo "停止服务..."
docker-compose down

echo "启动所有服务..."
docker-compose --profile tools --profile db up -d

echo ""
echo "========================================"
echo "服务启动完成！"
echo "========================================"
docker-compose ps
echo ""
echo "访问地址："
echo "  主服务:     http://localhost:3000"
echo "  数据库:     http://localhost:8080"
echo "  日志监控:   http://localhost:8888"
echo "========================================"