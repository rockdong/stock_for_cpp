#!/bin/bash

ALL_SERVICES=false

while getopts "a" opt; do
    case $opt in
        a)
            ALL_SERVICES=true
            ;;
        \?)
            echo "用法: $0 [-a]"
            echo "  -a  启动所有服务（包括 tools）"
            exit 1
            ;;
    esac
done

echo "停止服务..."
docker-compose down

echo "启动服务..."
if [ "$ALL_SERVICES" = true ]; then
    echo "启动所有服务（包括 tools）..."
    docker-compose --profile tools up -d
else
    echo "只启动数据库服务..."
    docker-compose up -d db
fi

echo "完成！"
docker-compose ps