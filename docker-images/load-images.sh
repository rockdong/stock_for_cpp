#!/bin/bash

set -e

echo "============================================"
echo "  Docker 镜像加载脚本"
echo "============================================"
echo ""

IMAGES_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "镜像目录: $IMAGES_DIR"
echo ""

echo "============================================"
echo "  步骤 1: 清除现有镜像"
echo "============================================"
echo ""

IMAGES_TO_REMOVE=(
    "rockdong13/stock_for_cpp:master"
    "rockdong13/stock_for_cpp_api:master"
    "rockdong13/stock_for_cpp_bot:master"
    "rockdong13/stock_for_cpp_web:master"
    "mysql:8.0"
    "adminer:latest"
    "amir20/dozzle:latest"
)

for image in "${IMAGES_TO_REMOVE[@]}"; do
    if docker images --format "{{.Repository}}:{{.Tag}}" | grep -q "^${image}$"; then
        echo "删除镜像: $image"
        docker rmi "$image" || echo "跳过: $image (可能被容器使用)"
    else
        echo "不存在: $image"
    fi
done

echo ""
echo "清理悬空镜像..."
docker image prune -f || true

echo ""
echo "============================================"
echo "  步骤 2: 加载新镜像"
echo "============================================"
echo ""

for image_file in "$IMAGES_DIR"/*.tar.gz; do
    if [ -f "$image_file" ]; then
        filename=$(basename "$image_file")
        echo "正在加载: $filename"
        docker load -i "$image_file"
        echo "完成: $filename"
        echo ""
    fi
done

echo "============================================"
echo "  所有镜像加载完成！"
echo "============================================"
echo ""
echo "当前已加载的镜像:"
docker images --format "table {{.Repository}}\t{{.Tag}}\t{{.Size}}" | grep -E "stock_for_cpp|mysql|adminer|dozzle" || docker images
echo ""
echo "启动服务命令:"
echo "  cd /path/to/project && docker-compose up -d"