# Web Frontend - 股票分析前端

基于 React + Vite 构建的股票分析系统 Web 前端，提供分析过程可视化和图表展示。

## 技术栈

- **React 18** - UI 框架
- **Vite 5** - 构建工具
- **TypeScript 5** - 类型安全
- **Tailwind CSS 3** - 样式框架
- **lightweight-charts** - 金融图表库 (TradingView)
- **Zustand** - 状态管理
- **React Router 7** - 路由管理
- **Axios** - HTTP 请求
- **Radix UI** - 组件库基础

## 项目结构

```
web-frontend/
├── src/
│   ├── components/      # UI 组件
│   ├── pages/           # 页面
│   │   ├── Analysis/    # 分析页面
│   │   └── Login/       # 登录页面
│   ├── services/        # API 服务
│   ├── stores/          # Zustand 状态
│   ├── contexts/        # React Context
│   ├── types/           # TypeScript 类型
│   ├── utils/           # 工具函数
│   ├── lib/             # 库配置
│   ├── App.tsx          # 应用入口
│   └── main.tsx         # 渲染入口
├── public/              # 静态资源
├── index.html           # HTML 模板
├── vite.config.ts       # Vite 配置
├── tailwind.config.js   # Tailwind 配置
├── tsconfig.json        # TypeScript 配置
├── Dockerfile           # Docker 构建
├── nginx.conf           # Nginx 配置
└── package.json
```

## 功能模块

### 分析页面 (Analysis)

- 分析进度实时展示
- 分析信号列表
- 股票详情卡片
- K 线图表展示
- EMA 指标线显示
- 策略筛选

### 登录页面 (Login)
- 账号密码登录
- 用户名密码验证
- JWT Token 管理

### 注册页面 (Register)
- 新用户注册
- 格式提示和验证

## 快速开始

### 本地开发

```bash
# 1. 安装依赖
npm install

# 2. 启动开发服务器
npm run dev

# 3. 访问
# http://localhost:5173
```

### 生产构建

```bash
# 构建
npm run build

# 预览
npm run preview
```

## API 代理配置

开发环境自动代理 API 请求：

| 前端路径 | 后端地址 |
|----------|----------|
| `/api/*` | `http://127.0.0.1:3001` |

## Docker 部署

```bash
# 构建镜像
docker build -t stock-web-frontend .

# 运行
docker run -p 8880:80 stock-web-frontend
```

生产环境使用 Nginx 作为静态文件服务器，端口 80。

## 环境变量

| 变量 | 说明 | 默认值 |
|------|------|--------|
| VITE_API_URL | API 服务地址 | `/api` |

## 组件说明

### 图表组件

使用 `lightweight-charts` 实现 K 线图：

```tsx
import { createChart } from 'lightweight-charts';

// 创建图表
const chart = createChart(container, {
  width: 800,
  height: 400,
});

// 添加 K 线系列
const candlestickSeries = chart.addCandlestickSeries();
candlestickSeries.setData(data);

// 添加 EMA 线
const lineSeries = chart.addLineSeries({
  color: 'blue',
  title: 'EMA17',
});
```

### 状态管理

使用 Zustand 管理全局状态：

```tsx
// stores/analysisStore.ts
import { create } from 'zustand';

const useAnalysisStore = create((set) => ({
  progress: null,
  signals: [],
  setProgress: (progress) => set({ progress }),
  setSignals: (signals) => set({ signals }),
}));
```

## 许可证

MIT License