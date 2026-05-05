# API 接口文档

本文档描述股票分析系统的所有 REST API 接口。

## 服务概述

| 服务 | 端口 | 基础路径 | 说明 |
|------|------|----------|------|
| Node.js 飞书服务 | 3000 | `/api` | 飞书推送 + REST API |
| NestJS API 服务 | 3001 | `/api` | NestJS API 服务 |

---

## Node.js API 服务 (端口 3000)

### 股票模块

#### 获取股票列表

```
GET /api/stocks
```

**响应示例：**
```json
{
  "success": true,
  "data": [
    {
      "ts_code": "000001.SZ",
      "name": "平安银行",
      "industry": "银行",
      "market": "深交所"
    }
  ]
}
```

#### 搜索股票

```
GET /api/stocks/search
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| keyword | string | 否 | 搜索关键词（股票名称或代码） |

**响应示例：**
```json
{
  "success": true,
  "data": [
    {
      "ts_code": "000001.SZ",
      "name": "平安银行",
      "industry": "银行",
      "market": "深交所"
    }
  ]
}
```

#### 获取股票详情

```
GET /api/stocks/:code
```

**路径参数：**
| 参数 | 类型 | 说明 |
|------|------|------|
| code | string | 股票代码 (如 `000001.SZ`) |

**响应示例：**
```json
{
  "success": true,
  "data": {
    "ts_code": "000001.SZ",
    "name": "平安银行",
    "industry": "银行",
    "market": "深交所",
    "price": 10.50,
    "high": 10.80,
    "low": 10.20,
    "volume": 1000000
  }
}
```

---

### 分析模块

#### 获取分析信号列表

```
GET /api/analysis/signals
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| label | string | 否 | 信号标签 (如 `BUY`, `SELL`) |
| limit | number | 否 | 返回数量，默认 50 |

**响应示例：**
```json
{
  "success": true,
  "data": [
    {
      "ts_code": "000001.SZ",
      "name": "平安银行",
      "strategy_name": "EMA25_GREATER_17_PRICE_MATCH",
      "label": "BUY",
      "freq": "d",
      "trade_date": "2024-01-15"
    }
  ]
}
```

#### 获取分析进度

```
GET /api/analysis/progress
```

**响应示例：**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "phase1": {
      "status": "completed",
      "total": 5000,
      "completed": 5000,
      "qualified": 120
    },
    "phase2": {
      "status": "running",
      "total": 120,
      "completed": 50,
      "failed": 2
    },
    "started_at": "2024-01-15T20:00:00Z",
    "phase1_completed_at": "2024-01-15T20:30:00Z",
    "updated_at": "2024-01-15T21:00:00Z",
    "execute_time": "20:00"
  }
}
```

#### 获取分析过程记录列表

```
GET /api/analysis/process
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| ts_code | string | 否 | 股票代码 |
| start_date | string | 否 | 开始日期 (YYYY-MM-DD) |
| end_date | string | 否 | 结束日期 (YYYY-MM-DD) |
| signal | string | 否 | 信号类型 (BUY/SELL) |
| strategy | string | 否 | 策略名称（逗号分隔多个） |
| freq | string | 否 | 频率（逗号分隔多个，如 `d,w`） |
| limit | number | 否 | 返回数量，默认 100 |

**响应示例：**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "ts_code": "000001.SZ",
      "stock_name": "平安银行",
      "trade_date": "2024-01-15",
      "data": {
        "strategies": [
          {
            "name": "EMA25_GREATER_17_PRICE_MATCH",
            "freqs": [
              {
                "freq": "d",
                "signal": "BUY",
                "candles": [...]
              }
            ]
          }
        ]
      },
      "created_at": "2024-01-15T20:00:00Z",
      "expires_at": "2024-01-22T20:00:00Z"
    }
  ]
}
```

#### 获取策略列表

```
GET /api/analysis/process/strategies
```

**响应示例：**
```json
{
  "success": true,
  "data": [
    "EMA25_GREATER_17_PRICE_MATCH",
    "EMA17TO25",
    "EMA17_BREAKOUT"
  ]
}
```

#### 获取分析图表数据

```
GET /api/analysis/process/chart/:ts_code
```

**路径参数：**
| 参数 | 类型 | 说明 |
|------|------|------|
| ts_code | string | 股票代码 |

**查询参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| strategy | string | 否 | 策略名称 |
| freq | string | 否 | 频率，默认 `d` |

**响应示例：**
```json
{
  "success": true,
  "data": [
    {
      "time": "2024-01-15",
      "open": 10.00,
      "high": 10.50,
      "low": 9.80,
      "close": 10.20,
      "volume": 1000000,
      "ema17": 10.15,
      "ema25": 10.10
    }
  ],
  "record": {
    "id": 1,
    "ts_code": "000001.SZ",
    "stock_name": "平安银行",
    "trade_date": "2024-01-15"
  }
}
```

#### 获取单条分析记录详情

```
GET /api/analysis/process/:id
```

**路径参数：**
| 参数 | 类型 | 说明 |
|------|------|------|
| id | number | 记录 ID |

**响应示例：**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "ts_code": "000001.SZ",
    "stock_name": "平安银行",
    "trade_date": "2024-01-15",
    "data": {
      "strategies": [...]
    },
    "created_at": "2024-01-15T20:00:00Z",
    "expires_at": "2024-01-22T20:00:00Z"
  }
}
```

#### 写入分析过程记录

```
POST /api/analysis/process
```

**请求体：**
```json
{
  "ts_code": "000001.SZ",
  "stock_name": "平安银行",
  "trade_date": "2024-01-15",
  "data": {
    "strategies": [...]
  }
}
```

**响应示例：**
```json
{
  "success": true,
  "id": 123
}
```

#### 清理过期记录

```
DELETE /api/analysis/process/expired
```

**响应示例：**
```json
{
  "success": true,
  "deleted": 50,
  "message": "清理了 50 条过期记录"
}
```

#### 获取单股分析结果

```
GET /api/analysis/:code
```

**路径参数：**
| 参数 | 类型 | 说明 |
|------|------|------|
| code | string | 股票代码 |

**响应示例：**
```json
{
  "success": true,
  "data": {
    "ts_code": "000001.SZ",
    "name": "平安银行",
    "strategy_name": "EMA25_GREATER_17_PRICE_MATCH",
    "label": "BUY",
    "freq": "d",
    "trade_date": "2024-01-15"
  }
}
```

---

### 图表模块

#### 获取 K 线图表数据

```
GET /api/charts/:code
```

**路径参数：**
| 参数 | 类型 | 说明 |
|------|------|------|
| code | string | 股票代码 |

**查询参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| freq | string | 否 | 频率 (`d`=日线, `w`=周线)，默认 `d` |
| limit | number | 否 | 返回数量，默认 100 |

**响应示例：**
```json
{
  "success": true,
  "data": [
    {
      "time": "2024-01-15",
      "open": 10.00,
      "high": 10.50,
      "low": 9.80,
      "close": 10.20,
      "volume": 1000000,
      "ema17": 10.15,
      "ema25": 10.10
    }
  ]
}
```

---

### 认证模块 - 微信登录

#### 生成微信登录二维码

```
GET /api/auth/qrcode
```

**响应示例：**
```json
{
  "success": true,
  "data": {
    "session_id": "uuid-string",
    "qr_url": "https://wechat.qr.code.url",
    "qr_type": "qrcode",
    "expires_at": "2024-01-15T21:00:00Z",
    "expires_in": 60
  }
}
```

#### 查询登录状态

```
GET /api/auth/status
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| sessionId | string | 是 | 会话 ID |

**响应示例：**
```json
{
  "success": true,
  "data": {
    "session_id": "uuid-string",
    "status": "pending",
    "user_id": null,
    "is_expired": false
  }
}
```

**状态值：**
- `pending` - 等待扫码
- `success` - 登录成功
- `expired` - 会话过期

#### 获取登录 Token

```
POST /api/auth/token
```

**请求体：**
```json
{
  "sessionId": "uuid-string"
}
```

**响应示例：**
```json
{
  "success": true,
  "data": {
    "token": "jwt-token-string",
    "user_id": 1
  }
}
```

---

### 认证模块 - 飞书登录

#### 生成飞书登录二维码

```
GET /api/auth/feishu/qrcode
```

**响应示例：**
```json
{
  "success": true,
  "data": {
    "session_id": "uuid-string",
    "auth_url": "https://feishu.auth.url",
    "qr_url": "https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=...",
    "expires_at": "2024-01-15T21:00:00Z",
    "expires_in": 300
  }
}
```

#### 飞书登录回调

```
GET /api/auth/feishu/callback
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| code | string | 是 | 飞书授权码 |
| state | string | 是 | 会话状态 |

**响应：**
- 飞书 App 内：返回登录成功 HTML 页面
- 浏览器：重定向到前端页面，携带 token 参数

#### 查询飞书登录状态

```
GET /api/auth/feishu/status
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| session_id | string | 是 | 会话 ID |

**响应示例：**
```json
{
  "success": true,
  "data": {
    "session_id": "uuid-string",
    "status": "success",
    "user_id": 1,
    "token": "jwt-token-string",
    "is_expired": false
  }
}
```

---

### 微信事件回调

#### 微信事件推送接收

```
POST /api/wechat/event
```

用于接收微信服务器推送的事件消息（扫码、订阅等）。

#### 微信服务器验证

```
GET /api/wechat/event
```

用于微信服务器配置时的签名验证。

---

### 用户模块

#### 获取用户关注列表

```
GET /api/user/watchlist
```

**响应示例：**
```json
{
  "success": true,
  "data": []
}
```

#### 添加关注股票

```
POST /api/user/watchlist
```

**响应示例：**
```json
{
  "success": true,
  "message": "添加成功"
}
```

#### 删除关注股票

```
DELETE /api/user/watchlist/:code
```

**响应示例：**
```json
{
  "success": true,
  "message": "删除成功"
}
```

#### 订阅股票通知

```
POST /api/user/subscribe
```

**响应示例：**
```json
{
  "success": true,
  "message": "订阅成功"
}
```

#### 取消订阅

```
DELETE /api/user/subscribe/:code
```

**响应示例：**
```json
{
  "success": true,
  "message": "取消成功"
}
```

---

### 基本面筛选模块

#### 基本面筛选

```
GET /api/fundamentals/filter
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| pe_max | number | 否 | PE 最大值，默认 30 |
| pb_max | number | 否 | PB 最大值，默认 5 |
| roe_min | number | 否 | ROE 最小值，默认 10 |
| gross_margin_min | number | 否 | 毛利率最小值，默认 20 |
| total_score_min | number | 否 | 总分最小值，默认 60 |

**响应示例：**
```json
{
  "success": true,
  "message": "基本面筛选功能正在开发中，请通过 C++ 后端执行",
  "thresholds": {
    "pe_max": 30.0,
    "pb_max": 5.0,
    "roe_min": 10.0,
    "gross_margin_min": 20.0,
    "total_score_min": 60.0
  }
}
```

#### 获取基本面详情

```
GET /api/fundamentals/:code
```

**响应示例：**
```json
{
  "success": true,
  "message": "基本面详情功能正在开发中",
  "ts_code": "000001.SZ"
}
```

#### 获取基本面筛选进度

```
GET /api/fundamentals/progress
```

**响应示例：**
```json
{
  "success": true,
  "data": {
    "total": 0,
    "completed": 0,
    "qualified": 0,
    "status": "idle",
    "started_at": null,
    "updated_at": "2024-01-15T20:00:00Z"
  }
}
```

---

## NestJS API 服务 (端口 3001)

### 股票模块

#### 获取股票列表

```
GET /api/stocks
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| page | number | 否 | 页码 |
| limit | number | 否 | 每页数量 |

**响应示例：**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "ts_code": "000001.SZ",
      "symbol": "000001",
      "name": "平安银行",
      "area": "深圳",
      "industry": "银行",
      "market": "主板",
      "exchange": "深交所",
      "list_status": "L",
      "created_at": "2024-01-15T00:00:00Z",
      "updated_at": "2024-01-15T00:00:00Z"
    }
  ]
}
```

#### 搜索股票

```
GET /api/stocks/search
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| keyword | string | 是 | 搜索关键词 |

**响应示例：**
```json
{
  "success": true,
  "data": [...]
}
```

#### 获取股票详情

```
GET /api/stocks/:code
```

**路径参数：**
| 参数 | 类型 | 说明 |
|------|------|------|
| code | string | 股票代码 |

**响应示例：**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "ts_code": "000001.SZ",
    "name": "平安银行",
    ...
  }
}
```

---

### 分析模块

#### 获取分析信号

```
GET /api/analysis/signals
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| label | string | 否 | 信号标签 |
| limit | number | 否 | 返回数量，默认 50 |

**响应示例：**
```json
{
  "success": true,
  "data": [...]
}
```

#### 获取分析进度

```
GET /api/analysis/progress
```

**响应示例：**
```json
{
  "success": true,
  "data": {
    "phase1": {...},
    "phase2": {...},
    ...
  }
}
```

#### 获取分析过程记录

```
GET /api/analysis/process
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| ts_code | string | 否 | 股票代码 |
| start_date | string | 否 | 开始日期 |
| end_date | string | 否 | 结束日期 |
| signal | string | 否 | 信号类型 |
| strategy | string | 否 | 策略名称（逗号分隔） |
| freq | string | 否 | 频率（逗号分隔） |
| limit | number | 否 | 返回数量，默认 100 |

**响应示例：**
```json
{
  "success": true,
  "data": [...]
}
```

#### 获取策略列表

```
GET /api/analysis/process/strategies
```

**响应示例：**
```json
{
  "success": true,
  "data": ["EMA25_GREATER_17_PRICE_MATCH", "EMA17TO25", ...]
}
```

#### 获取分析图表数据

```
GET /api/analysis/process/chart/:tsCode
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| strategy | string | 否 | 策略名称 |
| freq | string | 否 | 频率，默认 `d` |

**响应示例：**
```json
{
  "success": true,
  "data": [...]
}
```

#### 获取单条分析记录

```
GET /api/analysis/process/:id
```

**响应示例：**
```json
{
  "success": true,
  "data": {...}
}
```

#### 获取单股分析结果

```
GET /api/analysis/:code
```

**响应示例：**
```json
{
  "success": true,
  "data": {...}
}
```

---

### 图表模块

#### 获取图表数据

```
GET /api/charts/:code
```

**参数：**
| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| freq | string | 否 | 频率，默认 `d` |
| limit | number | 否 | 返回数量，默认 100 |

**响应示例：**
```json
{
  "success": true,
  "data": [...]
}
```

---

### 认证模块 - 管理员登录

#### 管理员账号登录

```
POST /api/auth/admin/login
```

**请求体：**
```json
{
  "username": "admin",
  "password": "admin123"
}
```

**响应示例（成功）：**
```json
{
  "success": true,
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "user": "admin",
    "expiresIn": "7d"
  }
}
```

**响应示例（失败）：**
```json
{
  "statusCode": 401,
  "message": "用户名或密码错误",
  "error": "Unauthorized"
}
```

**默认账号信息：**
- 用户名：`admin`
- 默认密码：`admin123`

**注意事项：**
- 密码使用 bcrypt 加密存储在环境变量 `ADMIN_PASSWORD_HASH` 中
- Token 有效期为 7 天（可在环境变量 `JWT_EXPIRES_IN` 中配置）
- 修改密码方法：POST `/api/auth/admin/generate-hash` 生成新的密码 hash

#### 生成密码 hash（配置新密码）

```
POST /api/auth/admin/generate-hash
```

**请求体：**
```json
{
  "password": "your_new_password"
}
```

**响应示例：**
```json
{
  "success": true,
  "data": {
    "hash": "$2a$10$...",
    "message": "请将此 hash 设置到环境变量 ADMIN_PASSWORD_HASH 中"
  }
}
```

#### 验证 Token

```
POST /api/auth/admin/verify
```

**请求体：**
```json
{
  "token": "jwt-token-string"
}
```

**响应示例：**
```json
{
  "valid": true,
  "user": "admin",
  "expiresAt": "2024-01-22T00:00:00Z"
}
```

---

## 统一响应格式

所有 API 接口返回统一的 JSON 格式：

### 成功响应

```json
{
  "success": true,
  "data": { ... }
}
```

### 错误响应

```json
{
  "success": false,
  "error": "错误信息描述"
}
```

---

## 错误码说明

| HTTP 状态码 | 说明 |
|-------------|------|
| 200 | 成功 |
| 400 | 参数错误 |
| 404 | 资源不存在 |
| 500 | 服务器内部错误 |

---

**更新时间：2026年5月6日**