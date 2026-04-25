# 微信公众号扫码登录设计方案

## 概述

在 Web 前端实现微信公众号扫码登录，用户扫码关注公众号后自动绑定账号，实现引流获客和用户身份绑定。

## 需求背景

- **目的**: 引流获客 + 用户绑定，便于后续推送分析消息
- **公众号类型**: 已认证服务号（支持带参数二维码 API）
- **现状**: Web 前端无登录系统，小程序已有微信登录流程可参考

## 整体流程

```
1. 用户访问 Web 前端
   ↓
2. 后端生成带参数二维码（scene_id = 随机 session_id）
   ↓
3. 前端显示二维码 + 轮询登录状态
   ↓
4. 用户扫码 → 关注公众号
   ↓
5. 微信服务器推送 SUBSCRIBE 事件到后端（包含 openid + scene_id）
   ↓
6. 后端：创建/绑定用户账号，标记 session 登录成功
   ↓
7. 前端轮询发现登录成功 → 获取 JWT token → 进入系统
```

## 技术架构

### 系统组件

| 组件 | 负责内容 |
|------|----------|
| Web 前端 | 登录页面、二维码展示、轮询状态、token 管理 |
| Node.js 后端 | 生成二维码、处理微信事件、用户绑定、JWT颁发 |
| 微信服务器 | 带参数二维码 API、事件推送 |

### 数据库表设计

**users 表** - 用户基础信息
```sql
CREATE TABLE users (
  id INT PRIMARY KEY AUTO_INCREMENT,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

**wechat_bindings 表** - 用户与公众号绑定
```sql
CREATE TABLE wechat_bindings (
  id INT PRIMARY KEY AUTO_INCREMENT,
  user_id INT NOT NULL,
  openid VARCHAR(50) NOT NULL UNIQUE,
  subscribed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  unsubscribed_at TIMESTAMP NULL,
  FOREIGN KEY (user_id) REFERENCES users(id)
);
```

**login_sessions 表** - 登录会话管理
```sql
CREATE TABLE login_sessions (
  session_id VARCHAR(36) PRIMARY KEY,
  status ENUM('pending', 'success', 'expired') DEFAULT 'pending',
  user_id INT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  expires_at TIMESTAMP NOT NULL,
  FOREIGN KEY (user_id) REFERENCES users(id)
);
```

## API 接口设计

### 前端调用接口

| 接口 | 方法 | 说明 | 参数 | 返回 |
|------|------|------|------|------|
| `/api/auth/qrcode` | GET | 生成带参数二维码 | 无 | `{session_id, qr_url, expires_in}` |
| `/api/auth/status` | GET | 轮询登录状态 | `session_id` | `{status, user_id?}` |
| `/api/auth/token` | POST | 获取 JWT token | `session_id` | `{token, expires_in}` |

### 微信回调接口

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/wechat/event` | POST | 微信事件推送回调（需配置为公众号服务器 URL） |

## 文件结构

### Web 前端新增文件

```
web-frontend/src/
├─ pages/
│  └─ Login/
│     └─ index.tsx          # 登录页面（二维码展示）
├─ services/
│  └─ authApi.ts            # 认证相关 API
├─ hooks/
│  └─ useAuth.ts            # 认证状态管理
└─ utils/
   └─ tokenStorage.ts       # token 存取
```

### Node.js 后端新增文件

```
nodejs/src/
├─ services/
│  ├─ wechatService.js      # 微信公众号 API 封装（二维码生成、access_token 管理）
│  └─ authService.js        # 认证逻辑（session 管理、JWT 生成）
├─ routes/
│  └─ auth.js               # 认证路由
└─ webhook/
   └─ wechatEventHandler.js # 微信事件处理（关注/扫码事件）
```

## 关键技术细节

### 1. 带参数二维码生成

调用微信 API 创建临时二维码：
- API: `https://api.weixin.qq.com/cgi-bin/qrcode/create?access_token=TOKEN`
- 参数: `{expire_seconds: 300, action_name: "QR_STR_SCENE", action_info: {scene: {scene_str: "session_id"}}}`
- 返回: ticket + expire_seconds
- 二维码 URL: `https://mp.weixin.qq.com/cgi-bin/showqrcode?ticket=TICKET`

### 2. 微信事件推送处理

用户扫码关注时，微信推送 XML 到 webhook：
```xml
<xml>
  <ToUserName>公众号ID</ToUserName>
  <FromUserName>用户openid</FromUserName>
  <CreateTime>时间戳</CreateTime>
  <MsgType>event</MsgType>
  <Event>subscribe</Event>
  <EventKey>qrscene_session_id</EventKey>
</xml>
```

处理逻辑：
1. 解析 XML，提取 openid + session_id（去掉 `qrscene_` 前缀）
2. 查询是否已有绑定 → 有则关联用户，无则创建新用户
3. 更新 login_sessions.status = "success"
4. 返回欢迎消息（可选）

### 3. 轮询与超时处理

前端轮询策略：
- 间隔: 2秒
- 最大轮询次数: 150次 (5分钟)
- 状态流转: pending → success → 获取 token
- 超时处理: 提示"二维码已过期，请刷新"

### 4. 安全考虑

| 安全点 | 处理方式 |
|--------|----------|
| session_id 随机性 | UUID v4，不可预测 |
| 二维码过期 | 5分钟自动失效 |
| token 有效期 | 7天，支持刷新机制 |
| webhook 验证 | 微信签名校验（msg_signature + timestamp + nonce） |
| 重复关注处理 | 查询已有绑定记录，直接返回登录成功 |

## 环境变量配置

```env
# 微信公众号配置
WECHAT_APP_ID=wxxxxxxxxxxx
WECHAT_APP_SECRET=xxxxxxxxxxxxxxxx
WECHAT_TOKEN=your_token              # 服务器配置 Token
WECHAT_ENCODING_AES_KEY=xxx          # 消息加密密钥（可选，安全模式需要）

# JWT 配置
JWT_SECRET=your_jwt_secret
JWT_EXPIRES_IN=7d
```

## 公众号配置步骤

1. 登录微信公众平台 → 设置与开发 → 基本配置
2. 配置服务器地址: `https://your-domain/api/wechat/event`
3. 设置 Token 和 EncodingAESKey
4. 启用消息加密（推荐）
5. 验证服务器配置成功

## 实现优先级

1. **P0 - 核心功能**
   - 后端: 二维码生成 + 事件处理 + session 管理
   - 前端: 登录页面 + 轮询 + token 存储

2. **P1 - 增强**
   - token 刷新机制
   - 登录状态持久化
   - 退出登录

3. **P2 - 可选**
   - 欢迎消息自定义
   - 用户信息获取（昵称/头像）
   - 未关注用户扫码处理

## 参考资源

- [微信公众号带参数二维码文档](https://developers.weixin.qq.com/doc/offiaccount/Account_Management/Generating_a_Customized_QR_Code.html)
- [微信公众号事件推送文档](https://developers.weixin.qq.com/doc/offiaccount/Message_Management/Receiving_event_pushes.html)
- 现有小程序登录流程: `miniprogram/app.js`