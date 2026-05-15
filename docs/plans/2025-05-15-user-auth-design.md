# 用户账号密码登录系统设计文档

## 概述

将现有的飞书扫码登录改为账号密码登录，支持普通用户注册登录，并区分用户角色（普通用户和管理员）。

## 设计目标

1. 移除飞书扫码登录，改为账号密码登录
2. 支持普通用户注册和登录
3. 区分用户角色：`user`（普通用户）和 `admin`（管理员）
4. 使用 JWT Token 进行身份认证
5. 密码使用 bcrypt 加密存储

## 数据库设计

### Account 表（新增）

```prisma
model Account {
  id           Int       @id @default(autoincrement())
  userId       Int       @unique @map("user_id")  // 关联 User 表，一对一
  username     String    @unique @db.VarChar(50)  // 用户名，唯一
  password     String    @db.VarChar(255)         // bcrypt 加密后的密码
  role         String    @default("user") @db.VarChar(20)  // 角色: user / admin
  isActive     Boolean   @default(true) @map("is_active")  // 账号是否激活
  createdAt    DateTime  @default(now()) @map("created_at")
  updatedAt    DateTime  @updatedAt @map("updated_at")

  user User @relation(fields: [userId], references: [id], onDelete: Cascade)

  @@index([username])
  @@map("accounts")
}
```

### User 表（调整）

```prisma
model User {
  id        Int       @id @default(autoincrement())
  createdAt DateTime  @default(now()) @map("created_at")

  wechatBindings WechatBinding[]
  loginSessions  LoginSession[]
  account        Account?          // 新增：一对一关联

  @@map("users")
}
```

### 设计说明

| 设计点 | 说明 |
|------|------|
| `userId` 唯一 | 一个 User 只能有一个 Account，一对一关系 |
| `username` 唯一 | 用户名不能重复 |
| `password` 加密 | 使用 bcrypt 存储，不可逆向 |
| `role` 字段 | `user`（普通用户）或 `admin`（管理员） |
| `isActive` 字段 | 支持账号禁用功能（管理员可封号） |

## API 设计

### 接口列表

| 接口 | 方法 | 说明 |
|------|------|------|
| `/auth/register` | POST | 用户注册 |
| `/auth/login` | POST | 用户登录（账号密码） |
| `/auth/admin/login` | POST | 管理员登录（保留现有） |
| `/auth/verify` | POST | 验证 Token |

### 注册接口 `POST /auth/register`

**请求**：
```json
{
  "username": "john123",
  "password": "password123"
}
```

**响应（成功）**：
```json
{
  "success": true,
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIs...",
    "user": {
      "id": 1,
      "username": "john123",
      "role": "user"
    }
  }
}
```

**响应（失败）**：
```json
{
  "success": false,
  "message": "用户名已存在"
}
```

**验证规则**：
- 用户名：3-20字符，仅允许字母、数字、下划线
- 密码：6-32字符，至少包含字母和数字

### 登录接口 `POST /auth/login`

**请求**：
```json
{
  "username": "john123",
  "password": "password123"
}
```

**响应（成功）**：
```json
{
  "success": true,
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIs...",
    "user": {
      "id": 1,
      "username": "john123",
      "role": "user"
    }
  }
}
```

**响应（失败）**：
```json
{
  "success": false,
  "message": "用户名或密码错误"
}
```

### 验证接口 `POST /auth/verify`

**请求**：
```json
{
  "token": "eyJhbGciOiJIUzI1NiIs..."
}
```

**响应（成功）**：
```json
{
  "valid": true,
  "user": {
    "sub": 1,
    "username": "john123",
    "role": "user"
  }
}
```

**响应（失败）**：
```json
{
  "valid": false,
  "error": "Token 已过期"
}
```

## 前端设计

### 页面结构

| 页面 | 路由 | 说明 |
|------|------|------|
| 登录页 | `/login` | 账号密码登录（替代飞书扫码） |
| 注册页 | `/register` | 新用户注册 |
| 管理员登录页 | `/admin-login` | 管理员登录（保留现有） |

### 登录页 `/login`

**布局**：
- 顶部：标题 "登录"
- 用户名输入框
- 密码输入框
- 登录按钮
- 底部：链接 "没有账号？点击注册"

**交互**：
- 输入用户名、密码后点击登录
- 登录成功 → 保存 token → 跳转到 `/analysis`
- 登录失败 → 显示错误提示

### 注册页 `/register`

**布局**：
- 顶部：标题 "注册"
- 用户名输入框（带格式提示：3-20字符，字母/数字/下划线）
- 密码输入框（带格式提示：6-32字符，至少包含字母和数字）
- 注册按钮
- 底部：链接 "已有账号？点击登录"

**交互**：
- 输入用户名、密码后点击注册
- 注册成功 → 保存 token → 跳转到 `/analysis`
- 注册失败 → 显示错误提示

### 前端 API Service

```typescript
// authApi.ts

export const authApi = {
  // 用户注册
  register: async (username: string, password: string) => {
    return axios.post('/api/auth/register', { username, password });
  },
  
  // 用户登录
  login: async (username: string, password: string) => {
    return axios.post('/api/auth/login', { username, password });
  },
  
  // 验证 token
  verify: async (token: string) => {
    return axios.post('/api/auth/verify', { token });
  },
};
```

### 路由调整

- 删除飞书登录相关代码（`feishuAuthApi.ts`）
- 新增 `/register` 路由指向注册页

## 认证流程设计

### JWT Token 结构

```json
{
  "sub": 1,           // 用户 ID
  "username": "john123",
  "role": "user",     // user / admin
  "iat": 1700000000,  // 签发时间
  "exp": 1700604800   // 过期时间
}
```

### Token 配置

| 配置项 | 值 | 说明 |
|------|------|------|
| `JWT_SECRET` | 环境变量 | 签名密钥 |
| `JWT_EXPIRES_IN` | `7d` | Token 有效期 7 天 |

### 注册流程

```
用户输入用户名、密码
        ↓
前端 POST /api/auth/register
        ↓
后端验证用户名格式
        ↓
后端检查用户名是否已存在
        ↓
后端创建 User + Account 记录（密码 bcrypt 加密）
        ↓
后端生成 JWT token
        ↓
前端保存 token → 跳转到 /analysis
```

### 登录流程

```
用户输入用户名、密码
        ↓
前端 POST /api/auth/login
        ↓
后端查找 Account 记录
        ↓
后端验证密码（bcrypt.compare）
        ↓
后端验证 isActive = true
        ↓
后端生成 JWT token
        ↓
前端保存 token → 跳转到 /analysis
```

### Token 存储

使用现有的 `tokenStorage` 工具：

```typescript
export const tokenStorage = {
  save: (token: string) => localStorage.setItem('token', token),
  get: () => localStorage.getItem('token'),
  clear: () => localStorage.removeItem('token'),
};
```

## 安全考虑

1. **密码加密**：使用 bcrypt 加密，不可逆向解密
2. **Token 安全**：JWT Token 存储在 localStorage，前端可清除
3. **用户名验证**：限制字符范围，防止注入攻击
4. **密码强度**：要求至少包含字母和数字
5. **账号禁用**：支持 `isActive` 字段，管理员可封号

## 实施计划

详见后续 implementation plan。