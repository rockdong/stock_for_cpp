# 微信小程序部署文档

## 一、部署前准备

### 1. 微信公众平台配置

在 [微信公众平台](https://mp.weixin.qq.com) 完成：

1. **配置服务器域名**（开发 > 开发管理 > 开发设置 > 服务器域名）：
   ```
   request 合法域名：
   https://your-domain.com
   ```

2. **获取 AppID**：
   - 位置：开发 > 开发管理 > 开发设置
   - 更新 `miniprogram/project.config.json` 中的 `appid`

### 2. 服务器要求

- Node.js 20+
- MySQL 8.0 (生产环境)
- 内存：512MB+
- 存储：1GB+

### 3. 域名配置

小程序需要配置以下域名白名单：

| 类型 | 域名 |
|------|------|
| request 合法域名 | `https://your-domain.com` |
| socket 合法域名 | 如需 WebSocket |

**注意**: 域名必须使用 HTTPS，不支持 HTTP。

## 二、Node.js 服务部署

### 1. 环境变量配置

创建 `/env/.env` 或设置环境变量：

```bash
# 飞书配置
FEISHU_APP_ID=your_app_id
FEISHU_APP_SECRET=your_app_secret

# API 配置
HTTP_PORT=3000
DB_PATH=/path/to/stock.db
```

### 2. 启动命令

```bash
cd nodejs
npm install
npm start
```

### 3. PM2 配置（推荐）

```javascript
// ecosystem.config.js
module.exports = {
  apps: [{
    name: 'stock-api',
    script: 'src/index.js',
    cwd: '/path/to/nodejs',
    instances: 1,
    autorestart: true,
    watch: false,
    max_memory_restart: '500M',
    env: {
      NODE_ENV: 'production',
      HTTP_PORT: 3000
    }
  }]
}
```

启动：
```bash
pm2 start ecosystem.config.js
pm2 save
pm2 startup
```

## 三、Docker 部署（推荐）

### 更新 docker-compose.yml

```yaml
version: '3.8'
services:
  stock-api:
    build:
      context: .
      dockerfile: Dockerfile
    ports:
      - "3000:3000"
    volumes:
      - ./data:/app/data
    environment:
      - HTTP_PORT=3000
      - FEISHU_APP_ID=${FEISHU_APP_ID}
      - FEISHU_APP_SECRET=${FEISHU_APP_SECRET}
    restart: unless-stopped
```

### 启动服务

```bash
docker-compose up -d
docker-compose logs -f stock-api
```

## 四、健康检查

### API 端点验证

```bash
# 检查股票列表
curl http://localhost:3000/api/stocks

# 检查分析信号
curl http://localhost:3000/api/analysis/signals

# 检查 K 线数据
curl http://localhost:3000/api/charts/000001.SZ?freq=d
```

### 预期响应

```json
{
  "success": true,
  "data": [...]
}
```

## 五、微信小程序发布

### 1. 上传代码

使用微信开发者工具：
1. 打开 `miniprogram/` 目录
2. 点击「上传」按钮
3. 填写版本号和备注

### 2. 提交审核

在微信公众平台：
1. 开发管理 > 开发版本
2. 点击「提交审核」
3. 填写审核信息

### 3. 发布上线

审核通过后：
1. 开发管理 > 审核版本
2. 点击「发布」
3. 选择发布比例（全量/灰度）

## 六、微信登录配置

### 1. 公众号配置

在微信公众平台配置：

1. **设置服务器地址**（设置与开发 > 基本配置 > 服务器配置）：
   ```
   URL: https://your-domain.com/api/wechat/event
   Token: your_token
   EncodingAESKey: 自动生成或手动设置
   消息加解密方式: 安全模式（推荐）
   ```

2. **获取 AppID 和 AppSecret**：
   - 位置：设置与开发 > 基本配置
   - 配置到 `env/.env`：
   ```bash
   WECHAT_APP_ID=wxxxxxxxxxxx
   WECHAT_APP_SECRET=xxxxxxxxxxxxxxxx
   WECHAT_TOKEN=your_token
   ```

### 2. 登录流程

小程序登录流程：
1. 小程序调用 `/api/auth/qrcode` 生成二维码
2. 用户扫码关注公众号
3. 公众号推送事件到 `/api/wechat/event`
4. 小程序轮询 `/api/auth/status` 查询登录状态
5. 登录成功后调用 `/api/auth/token` 获取 JWT

## 七、飞书登录配置

### 1. 创建飞书应用

在飞书开放平台创建企业自建应用：

1. **配置 OAuth**：
   - 重定向 URL: `https://your-domain.com/api/auth/feishu/callback`
   
2. **获取配置**：
   ```bash
   FEISHU_APP_ID=your_app_id
   FEISHU_APP_SECRET=your_app_secret
   ```

### 2. 登录流程

飞书登录流程：
1. 前端调用 `/api/auth/feishu/qrcode` 生成二维码
2. 用户使用飞书 App 扫码
3. 飞书回调 `/api/auth/feishu/callback`
4. 前端获取 JWT Token

## 八、监控配置

### 日志

```bash
# PM2 日志
pm2 logs stock-api

# Docker 日志
docker-compose logs -f stock-api
```

### 告警

建议配置：
- 服务异常告警
- 内存使用率告警（>80%）
- 响应时间告警（>1s）

## 九、回滚方案

### 服务回滚

```bash
# PM2 回滚
pm2 stop stock-api
pm2 start stock-api --update-env

# Docker 回滚
docker-compose down
docker-compose up -d
```

### 小程序回滚

在微信公众平台：
1. 开发管理 > 已发布版本
2. 点击「回退」

---

**更新时间：2026年5月4日**