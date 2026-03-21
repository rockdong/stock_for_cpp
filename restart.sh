#!/bin/bash

ALL_SERVICES=false

while getopts "a" opt; do
    case $opt in
        a)
            ALL_SERVICES=true
            ;;
        \?)
            echo "用法: $0 [-a]"
            echo "  -a  启动所有服务（包括 tools 和 db）"
            exit 1
            ;;
    esac
done

echo "停止服务..."
docker-compose down

echo "启动服务..."
if [ "$ALL_SERVICES" = true ]; then
    echo "启动所有服务..."
    docker-compose --profile tools --profile db up -d
else
    echo "启动主服务..."
    docker-compose up -d stock-analysis
fi

echo "完成！"
docker-compose ps