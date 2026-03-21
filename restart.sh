#!/bin/bash

echo "停止服务..."
docker-compose down

echo "启动所有服务..."
docker-compose --profile tools --profile db up -d

echo "完成！"
docker-compose ps