#!/bin/bash
# Docker容器启动脚本
# 确保挂载目录存在并具有正确权限

set -e

echo "=== Docker容器初始化脚本 ==="

# 检查并创建logs目录
if [ ! -d "/app/logs" ]; then
    echo "创建logs目录: /app/logs"
    mkdir -p /app/logs
    chown appuser:appgroup /app/logs
    chmod 755 /app/logs
else
    echo "logs目录已存在"
fi

# 检查stock.db文件权限
if [ -f "/app/stock.db" ]; then
    echo "检查stock.db文件权限"
    chown appuser:appgroup /app/stock.db
    chmod 644 /app/stock.db
    
    # 检查appuser是否可以读写数据库文件
    if sudo -u appuser test -r "/app/stock.db" && sudo -u appuser test -w "/app/stock.db"; then
        echo "✓ stock.db权限检查通过"
    else
        echo "❌ stock.db权限检查失败，尝试修复"
        chmod 666 /app/stock.db
    fi
else
    echo "stock.db文件不存在，将在首次运行时自动创建"
fi

# 检查并创建其他必要目录
if [ ! -d "/app/output" ]; then
    echo "创建output目录: /app/output"
    mkdir -p /app/output
    chown appuser:appgroup /app/output
fi

# 验证关键目录权限
echo "=== 权限验证 ==="
for dir in "/app" "/app/logs" "/app/output" "/app/nodejs"; do
    if [ -d "$dir" ]; then
        owner=$(stat -c "%U:%G" "$dir")
        perms=$(stat -c "%a" "$dir")
        echo "$dir - 所有者: $owner, 权限: $perms"
        
        # 确保appuser可以访问
        if ! sudo -u appuser test -r "$dir"; then
            echo "  ⚠️  appuser无法读取 $dir，修复权限..."
            chown -R appuser:appgroup "$dir"
        fi
    else
        echo "❌ 目录不存在: $dir"
    fi
done

echo "=== 初始化完成 ==="
echo "启动supervisor..."