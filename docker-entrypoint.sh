#!/bin/bash
# Docker容器启动脚本
# 确保挂载目录存在并具有正确权限

set -e

echo "=== Docker容器初始化脚本 ==="

# 设置文件描述符限制，防止 "too many open files" 错误
# 默认设置为 65536，与 docker-compose.yml 保持一致
echo "设置文件描述符限制..."
ulimit -n 65536 2>/dev/null || true
ulimit -Hn 65536 2>/dev/null || true
echo "当前 ulimit -n: $(ulimit -n)"
echo "当前 ulimit -Hn: $(ulimit -Hn)"

# 获取当前用户的UID/GID以进行适当的权限管理
CURRENT_UID=${CURRENT_UID:-$(id -u)}
CURRENT_GID=${CURRENT_GID:-$(id -g)}

# 检查并创建logs目录
if [ ! -d "/app/logs" ]; then
    echo "创建logs目录: /app/logs"
    mkdir -p /app/logs
    chmod 755 /app/logs
else
    echo "logs目录已存在"
fi

# 检查stock.db文件权限
if [ -f "/app/stock.db" ]; then
    echo "检查stock.db文件权限"
    
    # 当在容器内运行时，无需使用chown（因为USER appuser已经设定）
    # 如果外部挂载卷有问题，则调整权限
    chmod 644 /app/stock.db
    
    # 检查当前用户是否可以读写数据库文件
    if [ -r "/app/stock.db" ] && [ -w "/app/stock.db" ]; then
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
fi

# 验证关键目录权限 - 移除对sudo的依赖
echo "=== 权限验证 ==="
for dir in "/app" "/app/logs" "/app/output" "/app/nodejs"; do
    if [ -d "$dir" ]; then
        owner=$(stat -c "%U:%G" "$dir")
        perms=$(stat -c "%a" "$dir")
        echo "$dir - 所有者: $owner, 权限: $perms"
        
        # 简化权限检查逻辑，不再依赖sudo
        if [ ! -r "$dir" ]; then
            echo "  ⚠️  当前用户无法读取 $dir，修复权限..."
            chmod -R 755 "$dir" 2>/dev/null || true
        fi
    else
        echo "❌ 目录不存在: $dir"
    fi
done

echo "=== 初始化完成 ==="
echo "启动supervisor..."

# 执行传递给容器的CMD命令
exec "$@"