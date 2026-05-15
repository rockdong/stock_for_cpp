# 用户账号密码登录系统实施计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 将飞书扫码登录改为账号密码登录，支持普通用户注册登录，区分用户角色。

**Architecture:** 新建 Account 表存储账号密码，与 User 表一对一关联。后端新增注册、登录 API。前端新增登录页、注册页，移除飞书扫码。

**Tech Stack:** NestJS 11, Prisma 5, bcryptjs, JWT, React 18, TypeScript

---

## Task 1: 数据库模型 - 新增 Account 表

**Files:**
- Modify: `backend/prisma/schema.prisma`

**Step 1: 在 schema.prisma 中添加 Account 模型**

在 `User` 模型后添加以下内容：

```prisma
model Account {
  id           Int       @id @default(autoincrement())
  userId       Int       @unique @map("user_id")
  username     String    @unique @db.VarChar(50)
  password     String    @db.VarChar(255)
  role         String    @default("user") @db.VarChar(20)
  isActive     Boolean   @default(true) @map("is_active")
  createdAt    DateTime  @default(now()) @map("created_at")
  updatedAt    DateTime  @updatedAt @map("updated_at")

  user User @relation(fields: [userId], references: [id], onDelete: Cascade)

  @@index([username])
  @@map("accounts")
}
```

**Step 2: 在 User 模型中添加 account 关联**

修改 `User` 模型，添加 `account` 关联：

```prisma
model User {
  id        Int       @id @default(autoincrement())
  createdAt DateTime  @default(now()) @map("created_at")

  wechatBindings WechatBinding[]
  loginSessions  LoginSession[]
  account        Account?

  @@map("users")
}
```

**Step 3: 生成 Prisma Client**

Run: `cd backend && npx prisma generate`
Expected: "Prisma Client generated successfully"

**Step 4: 创建数据库迁移**

Run: `cd backend && npx prisma migrate dev --name add_account_table`
Expected: Migration created and applied

**Step 5: Commit**

```bash
git add backend/prisma/schema.prisma backend/prisma/migrations/
git commit -m "feat(db): add Account table for user authentication"
```

---

## Task 2: 后端 Service - 实现注册和登录逻辑

**Files:**
- Modify: `backend/src/modules/auth/auth.service.ts`

**Step 1: 添加 Prisma 服务依赖**

修改 `auth.module.ts`，导入 PrismaModule：

```typescript
// backend/src/modules/auth/auth.module.ts
import { Module } from '@nestjs/common';
import { JwtModule } from '@nestjs/jwt';
import { ConfigModule, ConfigService } from '@nestjs/config';
import { AuthController } from './auth.controller';
import { AuthService } from './auth.service';
import { PrismaModule } from '../../prisma/prisma.module';

@Module({
  imports: [
    PrismaModule,
    ConfigModule,
    JwtModule.registerAsync({
      imports: [ConfigModule],
      inject: [ConfigService],
      useFactory: (configService: ConfigService) => ({
        secret: configService.get<string>('JWT_SECRET') || 'default-secret',
        signOptions: {
          expiresIn: configService.get<string>('JWT_EXPIRES_IN') || '7d',
        },
      }),
    }),
  ],
  controllers: [AuthController],
  providers: [AuthService],
})
export class AuthModule {}
```

**Step 2: 在 auth.service.ts 中添加 PrismaClient 注入**

修改 `auth.service.ts`：

```typescript
import { Injectable, UnauthorizedException, BadRequestException } from '@nestjs/common';
import { JwtService } from '@nestjs/jwt';
import { ConfigService } from '@nestjs/config';
import { PrismaClient } from '@prisma/client';
import * as bcrypt from 'bcryptjs';

@Injectable()
export class AuthService {
  private prisma: PrismaClient;

  constructor(
    private jwtService: JwtService,
    private configService: ConfigService,
  ) {
    this.prisma = new PrismaClient();
  }

  // ... 保留现有的 adminLogin, verifyToken, generatePasswordHash 方法
```

**Step 3: 添加用户名验证方法**

```typescript
  /**
   * 验证用户名格式
   * 规则: 3-20字符，仅允许字母、数字、下划线
   */
  private validateUsername(username: string): boolean {
    const regex = /^[a-zA-Z0-9_]{3,20}$/;
    return regex.test(username);
  }

  /**
   * 验证密码格式
   * 规则: 6-32字符，至少包含字母和数字
   */
  private validatePassword(password: string): boolean {
    if (password.length < 6 || password.length > 32) return false;
    const hasLetter = /[a-zA-Z]/.test(password);
    const hasNumber = /[0-9]/.test(password);
    return hasLetter && hasNumber;
  }
```

**Step 4: 添加注册方法**

```typescript
  /**
   * 用户注册
   */
  async register(username: string, password: string) {
    // 1. 验证用户名格式
    if (!this.validateUsername(username)) {
      throw new BadRequestException('用户名格式错误：需3-20字符，仅允许字母、数字、下划线');
    }

    // 2. 验证密码格式
    if (!this.validatePassword(password)) {
      throw new BadRequestException('密码格式错误：需6-32字符，至少包含字母和数字');
    }

    // 3. 检查用户名是否已存在
    const existingAccount = await this.prisma.account.findUnique({
      where: { username },
    });

    if (existingAccount) {
      throw new BadRequestException('用户名已存在');
    }

    // 4. 创建 User 记录
    const user = await this.prisma.user.create({
      data: {},
    });

    // 5. 创建 Account 记录（密码 bcrypt 加密）
    const passwordHash = await this.generatePasswordHash(password);
    const account = await this.prisma.account.create({
      data: {
        userId: user.id,
        username,
        password: passwordHash,
        role: 'user',
      },
    });

    // 6. 生成 JWT token
    const payload = {
      sub: user.id,
      username,
      role: 'user',
    };

    const token = this.jwtService.sign(payload);

    return {
      success: true,
      data: {
        token,
        user: {
          id: user.id,
          username,
          role: 'user',
        },
      },
    };
  }
```

**Step 5: 添加用户登录方法**

```typescript
  /**
   * 用户账号密码登录
   */
  async login(username: string, password: string) {
    // 1. 查找 Account 记录
    const account = await this.prisma.account.findUnique({
      where: { username },
      include: { user: true },
    });

    if (!account) {
      throw new UnauthorizedException('用户名或密码错误');
    }

    // 2. 验证密码
    const isPasswordValid = await bcrypt.compare(password, account.password);
    if (!isPasswordValid) {
      throw new UnauthorizedException('用户名或密码错误');
    }

    // 3. 验证账号是否激活
    if (!account.isActive) {
      throw new UnauthorizedException('账号已被禁用');
    }

    // 4. 生成 JWT token
    const payload = {
      sub: account.userId,
      username: account.username,
      role: account.role,
    };

    const token = this.jwtService.sign(payload);

    return {
      success: true,
      data: {
        token,
        user: {
          id: account.userId,
          username: account.username,
          role: account.role,
        },
      },
    };
  }
```

**Step 6: Commit**

```bash
git add backend/src/modules/auth/auth.service.ts backend/src/modules/auth/auth.module.ts
git commit -m "feat(auth): add register and login methods in AuthService"
```

---

## Task 3: 后端 Controller - 新增注册和登录接口

**Files:**
- Modify: `backend/src/modules/auth/auth.controller.ts`

**Step 1: 添加注册接口**

```typescript
  /**
   * 用户注册
   * POST /api/auth/register
   * Body: { username: string, password: string }
   */
  @Post('register')
  async register(@Body() body: { username: string; password: string }) {
    if (!body.username || !body.password) {
      throw new BadRequestException('请提供用户名和密码');
    }

    return await this.authService.register(body.username, body.password);
  }
```

**Step 2: 添加用户登录接口**

```typescript
  /**
   * 用户账号密码登录
   * POST /api/auth/login
   * Body: { username: string, password: string }
   */
  @Post('login')
  async login(@Body() body: { username: string; password: string }) {
    if (!body.username || !body.password) {
      throw new UnauthorizedException('请提供用户名和密码');
    }

    return await this.authService.login(body.username, body.password);
  }
```

**Step 3: 添加 BadRequestException 导入**

修改导入语句：

```typescript
import { Controller, Post, Body, UnauthorizedException, BadRequestException } from '@nestjs/common';
```

**Step 4: Commit**

```bash
git add backend/src/modules/auth/auth.controller.ts
git commit -m "feat(auth): add register and login endpoints"
```

---

## Task 4: 后端编译和测试

**Files:**
- 无文件修改，仅运行命令

**Step 1: 编译后端代码**

Run: `cd backend && npm run build`
Expected: 编译成功，无错误

**Step 2: 启动后端服务测试**

Run: `cd backend && npm run dev`
Expected: 服务启动在端口 3001

**Step 3: 手动测试注册接口**

使用 curl 或其他工具测试：

```bash
curl -X POST http://localhost:3001/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"username":"testuser","password":"test123"}'
```

Expected: 返回成功响应，包含 token

**Step 4: 手动测试登录接口**

```bash
curl -X POST http://localhost:3001/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"testuser","password":"test123"}'
```

Expected: 返回成功响应，包含 token

**Step 5: Commit（如果有修复）**

如果有任何修复，提交代码。

---

## Task 5: 前端 API Service - 新增认证 API

**Files:**
- Create: `web-frontend/src/services/userAuthApi.ts`
- Modify: `web-frontend/src/services/authApi.ts`

**Step 1: 创建 userAuthApi.ts**

```typescript
import axios from 'axios';

const API_BASE_URL = '/api';

export interface RegisterRequest {
  username: string;
  password: string;
}

export interface LoginRequest {
  username: string;
  password: string;
}

export interface AuthResponse {
  success: boolean;
  data?: {
    token: string;
    user: {
      id: number;
      username: string;
      role: string;
    };
  };
  message?: string;
}

export interface VerifyResponse {
  valid: boolean;
  user?: {
    sub: number;
    username: string;
    role: string;
  };
  error?: string;
}

export const userAuthApi = {
  /**
   * 用户注册
   */
  register: async (data: RegisterRequest): Promise<AuthResponse> => {
    const response = await axios.post(`${API_BASE_URL}/auth/register`, data);
    return response.data;
  },

  /**
   * 用户登录
   */
  login: async (data: LoginRequest): Promise<AuthResponse> => {
    const response = await axios.post(`${API_BASE_URL}/auth/login`, data);
    return response.data;
  },

  /**
   * 验证 token
   */
  verify: async (token: string): Promise<VerifyResponse> => {
    const response = await axios.post(`${API_BASE_URL}/auth/verify`, { token });
    return response.data;
  },
};
```

**Step 2: Commit**

```bash
git add web-frontend/src/services/userAuthApi.ts
git commit -m "feat(frontend): add userAuthApi for registration and login"
```

---

## Task 6: 前端页面 - 重写登录页

**Files:**
- Modify: `web-frontend/src/pages/Login/index.tsx`

**Step 1: 重写登录页组件**

完整重写登录页，移除飞书扫码，改为账号密码登录：

```typescript
import { useState } from 'react';
import { useNavigate, Link } from 'react-router-dom';
import { userAuthApi } from '../../services/userAuthApi';
import { tokenStorage } from '../../utils/tokenStorage';

export default function LoginPage() {
  const navigate = useNavigate();
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');
  const [isLoading, setIsLoading] = useState(false);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError('');
    setIsLoading(true);

    try {
      const result = await userAuthApi.login({ username, password });

      if (result.success && result.data?.token) {
        tokenStorage.save(result.data.token);
        navigate('/analysis');
      } else {
        setError(result.message || '登录失败，请检查用户名和密码');
      }
    } catch (err: any) {
      console.error('登录错误:', err);
      setError(err.response?.data?.message || '网络错误，请稍后重试');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-base flex items-center justify-center relative overflow-hidden">
      {/* 背景装饰 */}
      <div className="absolute inset-0 overflow-hidden pointer-events-none">
        <div className="absolute -top-40 -right-40 w-80 h-80 bg-accent-blue/5 rounded-full blur-3xl" />
        <div className="absolute -bottom-40 -left-40 w-80 h-80 bg-accent-blue/3 rounded-full blur-3xl" />
      </div>

      <div className="bg-surface rounded-xl border border-border-default p-8 max-w-md w-full mx-4 relative z-10">
        <h1 className="text-2xl font-semibold text-text-primary text-center mb-2">
          登录
        </h1>
        <p className="text-text-tertiary text-center text-sm mb-6">
          登录后查看股票分析结果
        </p>

        {error && (
          <div className="mb-4 p-3 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm">
            {error}
          </div>
        )}

        <form onSubmit={handleSubmit} className="space-y-4">
          <div>
            <label
              htmlFor="username"
              className="block text-sm font-medium text-text-secondary mb-1.5"
            >
              用户名
            </label>
            <input
              id="username"
              type="text"
              value={username}
              onChange={(e) => setUsername(e.target.value)}
              className="w-full px-4 py-3 bg-elevated border border-border-default rounded-lg text-text-primary placeholder:text-text-tertiary focus:outline-none focus:border-accent-blue focus:ring-1 focus:ring-accent-blue/30 transition-colors"
              placeholder="请输入用户名"
              required
              disabled={isLoading}
              autoComplete="username"
            />
          </div>

          <div>
            <label
              htmlFor="password"
              className="block text-sm font-medium text-text-secondary mb-1.5"
            >
              密码
            </label>
            <input
              id="password"
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              className="w-full px-4 py-3 bg-elevated border border-border-default rounded-lg text-text-primary placeholder:text-text-tertiary focus:outline-none focus:border-accent-blue focus:ring-1 focus:ring-accent-blue/30 transition-colors"
              placeholder="请输入密码"
              required
              disabled={isLoading}
              autoComplete="current-password"
            />
          </div>

          <button
            type="submit"
            disabled={isLoading || !username || !password}
            className="w-full btn--primary py-3 mt-2 disabled:opacity-50 disabled:cursor-not-allowed"
          >
            {isLoading ? (
              <span className="flex items-center justify-center gap-2">
                <svg className="animate-spin h-4 w-4" viewBox="0 0 24 24">
                  <circle
                    className="opacity-25"
                    cx="12"
                    cy="12"
                    r="10"
                    stroke="currentColor"
                    strokeWidth="4"
                    fill="none"
                  />
                  <path
                    className="opacity-75"
                    fill="currentColor"
                    d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"
                  />
                </svg>
                登录中...
              </span>
            ) : (
              '登录'
            )}
          </button>
        </form>

        {/* 注册链接 */}
        <div className="mt-6 text-center">
          <span className="text-text-tertiary text-sm">没有账号？</span>
          <Link
            to="/register"
            className="text-sm text-accent-blue hover:text-accent-blue/80 ml-1 transition-colors"
          >
            点击注册
          </Link>
        </div>

        {/* 底部提示 */}
        <div className="mt-6 pt-4 border-t border-border-default text-center">
          <p className="text-xs text-text-tertiary">
            需要管理员登录？
            <Link
              to="/admin-login"
              className="text-accent-blue hover:text-accent-blue/80 ml-1 transition-colors"
            >
              点击这里
            </Link>
          </p>
        </div>
      </div>
    </div>
  );
}
```

**Step 2: Commit**

```bash
git add web-frontend/src/pages/Login/index.tsx
git commit -m "feat(frontend): rewrite login page with username/password form"
```

---

## Task 7: 前端页面 - 新建注册页

**Files:**
- Create: `web-frontend/src/pages/Register/index.tsx`

**Step 1: 创建注册页组件**

```typescript
import { useState } from 'react';
import { useNavigate, Link } from 'react-router-dom';
import { userAuthApi } from '../../services/userAuthApi';
import { tokenStorage } from '../../utils/tokenStorage';

export default function RegisterPage() {
  const navigate = useNavigate();
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');
  const [isLoading, setIsLoading] = useState(false);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError('');
    setIsLoading(true);

    try {
      const result = await userAuthApi.register({ username, password });

      if (result.success && result.data?.token) {
        tokenStorage.save(result.data.token);
        navigate('/analysis');
      } else {
        setError(result.message || '注册失败');
      }
    } catch (err: any) {
      console.error('注册错误:', err);
      setError(err.response?.data?.message || '网络错误，请稍后重试');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-base flex items-center justify-center relative overflow-hidden">
      {/* 背景装饰 */}
      <div className="absolute inset-0 overflow-hidden pointer-events-none">
        <div className="absolute -top-40 -right-40 w-80 h-80 bg-accent-blue/5 rounded-full blur-3xl" />
        <div className="absolute -bottom-40 -left-40 w-80 h-80 bg-accent-blue/3 rounded-full blur-3xl" />
      </div>

      <div className="bg-surface rounded-xl border border-border-default p-8 max-w-md w-full mx-4 relative z-10">
        <h1 className="text-2xl font-semibold text-text-primary text-center mb-2">
          注册
        </h1>
        <p className="text-text-tertiary text-center text-sm mb-6">
          注册账号后即可查看股票分析结果
        </p>

        {error && (
          <div className="mb-4 p-3 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm">
            {error}
          </div>
        )}

        <form onSubmit={handleSubmit} className="space-y-4">
          <div>
            <label
              htmlFor="username"
              className="block text-sm font-medium text-text-secondary mb-1.5"
            >
              用户名
            </label>
            <input
              id="username"
              type="text"
              value={username}
              onChange={(e) => setUsername(e.target.value)}
              className="w-full px-4 py-3 bg-elevated border border-border-default rounded-lg text-text-primary placeholder:text-text-tertiary focus:outline-none focus:border-accent-blue focus:ring-1 focus:ring-accent-blue/30 transition-colors"
              placeholder="3-20字符，字母/数字/下划线"
              required
              disabled={isLoading}
              autoComplete="username"
            />
            <p className="text-xs text-text-tertiary mt-1">
              3-20字符，仅允许字母、数字、下划线
            </p>
          </div>

          <div>
            <label
              htmlFor="password"
              className="block text-sm font-medium text-text-secondary mb-1.5"
            >
              密码
            </label>
            <input
              id="password"
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              className="w-full px-4 py-3 bg-elevated border border-border-default rounded-lg text-text-primary placeholder:text-text-tertiary focus:outline-none focus:border-accent-blue focus:ring-1 focus:ring-accent-blue/30 transition-colors"
              placeholder="6-32字符，至少包含字母和数字"
              required
              disabled={isLoading}
              autoComplete="new-password"
            />
            <p className="text-xs text-text-tertiary mt-1">
              6-32字符，至少包含字母和数字
            </p>
          </div>

          <button
            type="submit"
            disabled={isLoading || !username || !password}
            className="w-full btn--primary py-3 mt-2 disabled:opacity-50 disabled:cursor-not-allowed"
          >
            {isLoading ? (
              <span className="flex items-center justify-center gap-2">
                <svg className="animate-spin h-4 w-4" viewBox="0 0 24 24">
                  <circle
                    className="opacity-25"
                    cx="12"
                    cy="12"
                    r="10"
                    stroke="currentColor"
                    strokeWidth="4"
                    fill="none"
                  />
                  <path
                    className="opacity-75"
                    fill="currentColor"
                    d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"
                  />
                </svg>
                注册中...
              </span>
            ) : (
              '注册'
            )}
          </button>
        </form>

        {/* 登录链接 */}
        <div className="mt-6 text-center">
          <span className="text-text-tertiary text-sm">已有账号？</span>
          <Link
            to="/login"
            className="text-sm text-accent-blue hover:text-accent-blue/80 ml-1 transition-colors"
          >
            点击登录
          </Link>
        </div>
      </div>
    </div>
  );
}
```

**Step 2: Commit**

```bash
git add web-frontend/src/pages/Register/index.tsx
git commit -m "feat(frontend): add Register page component"
```

---

## Task 8: 前端路由 - 添加注册路由

**Files:**
- Modify: `web-frontend/src/App.tsx`

**Step 1: 添加注册路由**

在路由配置中添加 `/register` 路径：

```typescript
// 在路由数组中添加
<Route path="/register" element={<RegisterPage />} />
```

**Step 2: 导入注册页组件**

```typescript
import RegisterPage from './pages/Register';
```

**Step 3: Commit**

```bash
git add web-frontend/src/App.tsx
git commit -m "feat(frontend): add register route to App"
```

---

## Task 9: 前端清理 - 移除飞书登录相关代码

**Files:**
- Delete: `web-frontend/src/services/feishuAuthApi.ts`

**Step 1: 删除飞书认证 API 文件**

```bash
rm web-frontend/src/services/feishuAuthApi.ts
```

**Step 2: 检查是否有其他引用**

Run: `grep -r "feishuAuthApi" web-frontend/src/`
Expected: 无匹配（或仅有注释引用）

如果有引用，移除相关导入和使用。

**Step 3: Commit**

```bash
git add web-frontend/src/services/feishuAuthApi.ts
git commit -m "feat(frontend): remove feishu auth API (replaced by username/password)"
```

---

## Task 10: 前端编译和测试

**Files:**
- 无文件修改，仅运行命令

**Step 1: 编译前端代码**

Run: `cd web-frontend && npm run build`
Expected: 编译成功，无错误

**Step 2: 启动前端开发服务器**

Run: `cd web-frontend && npm run dev`
Expected: 服务启动在端口 5173

**Step 3: 手动测试登录流程**

1. 打开浏览器访问 `http://localhost:5173/login`
2. 输入用户名、密码，点击登录
3. 验证是否跳转到 `/analysis`

**Step 4: 手动测试注册流程**

1. 打开浏览器访问 `http://localhost:5173/register`
2. 输入用户名、密码，点击注册
3. 验证是否跳转到 `/analysis`

**Step 5: Commit（如果有修复）**

如果有任何修复，提交代码。

---

## Task 11: 文档更新 - 更新 README

**Files:**
- Modify: `README.md`
- Modify: `backend/README.md`
- Modify: `web-frontend/README.md`

**Step 1: 更新项目 README.md**

在更新日志部分添加：

```markdown
### v2.4.0
- 新增用户账号密码登录系统
  - 支持普通用户注册和登录
  - 用户名：3-20字符，字母/数字/下划线
  - 密码：6-32字符，至少包含字母和数字
  - JWT Token 认证（有效期 7 天）
- 移除飞书扫码登录
- 新增 Account 表存储用户账号信息
- 区分用户角色：user（普通用户）和 admin（管理员）
- 支持账号禁用功能（管理员可封号）
```

**Step 2: 更新 backend README.md**

更新认证模块描述：

```markdown
### 认证模块
- `POST /auth/register` - 用户注册
- `POST /auth/login` - 用户登录（账号密码）
- `POST /auth/admin/login` - 管理员登录（保留）
- `POST /auth/admin/verify` - 验证 Token
```

**Step 3: 更新 web-frontend README.md**

更新登录页面描述：

```markdown
### 登录页面 (Login)
- 账号密码登录
- 用户名密码验证
- JWT Token 管理

### 注册页面 (Register)
- 新用户注册
- 格式提示和验证
```

**Step 4: Commit**

```bash
git add README.md backend/README.md web-frontend/README.md
git commit -m "docs: update README for user authentication feature"
```

---

## Task 12: 最终验证

**Files:**
- 无文件修改，仅运行验证

**Step 1: 验证后端 API**

```bash
# 测试注册
curl -X POST http://localhost:3001/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"username":"finaltest","password":"test1234"}'

# 测试登录
curl -X POST http://localhost:3001/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"finaltest","password":"test1234"}'
```

Expected: 返回成功响应

**Step 2: 验证前端页面**

- 登录页面正常显示
- 注册页面正常显示
- 登录/注册流程正常工作

**Step 3: 验证数据库**

```bash
cd backend && npx prisma studio
```

检查 Account 表是否有记录，数据结构是否正确。

**Step 4: 最终 Commit（如果有遗漏）**

检查是否有未提交的文件：

```bash
git status
```

如果有遗漏，补充提交。

---

## 执行选项

计划完成并保存到 `docs/plans/2025-05-15-user-auth-implementation.md`。

**两种执行选项：**

**1. Subagent-Driven（本 session）** - 在当前 session 中逐任务执行，每任务完成后 review

**2. Parallel Session（独立 session）** - 在新 session 中使用 executing-plans skill 执行

**选择哪种方式？**