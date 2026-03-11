#!/bin/bash

# 停止所有运行中的容器
docker stop $(docker ps -aq) 2>/dev/null

# 删除所有容器
docker rm $(docker ps -aq) 2>/dev/null

# 删除所有镜像
docker rmi $(docker images -q) 2>/dev/null

# 删除所有网络
docker network prune -f 2>/dev/null

echo "清理完成"

docker pull --platform linux/amd64 rockdong13/stock_for_cpp:v2

echo "下载完成"
