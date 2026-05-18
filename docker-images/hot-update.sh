#!/bin/bash

echo "============================================"
echo "  热更新 Node.js 认证服务代码"
echo "============================================"
echo ""

IMAGES_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "解压代码文件..."
cd /tmp
tar -xzvf "$IMAGES_DIR/nodejs-code.tar.gz"

echo ""
echo "复制文件到容器..."
docker cp services/authService.js stock-feishu-bot:/app/src/services/authService.js
docker cp services/wechatService.js stock-feishu-bot:/app/src/services/wechatService.js
docker cp routes/auth.js stock-feishu-bot:/app/src/routes/auth.js
docker cp index.js stock-feishu-bot:/app/src/index.js

echo ""
echo "重启容器..."
docker restart stock-feishu-bot

sleep 5

echo ""
echo "测试 API..."
curl http://localhost:8880/api/auth/qrcode

echo ""
echo "============================================"
echo "  热更新完成！"
echo "============================================"