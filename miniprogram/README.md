# 微信小程序 - 股票分析

股票分析系统的微信小程序客户端，提供移动端分析结果查看。

## 功能

- 股票分析信号查看
- K 线图表展示
- 分析进度实时更新
- 股票搜索
- 用户登录绑定

## 项目结构

```
miniprogram/
├── pages/
│   ├── index/      # 首页 - 股票列表
│   ├── stock/      # 股票详情 - 图表展示
│   ├── analysis/   # 分析 - 信号列表
│   └── mine/       # 我的 - 用户设置
├── utils/          # 工具函数
├── app.js          # 应用入口
├── app.json        # 应用配置
├── app.wxss        # 全局样式
├── project.config.json  # 项目配置
├── sitemap.json    # 小程序索引配置
└── assets/         # 静态资源
    └── icons/      # TabBar 图标
```

## 页面说明

| 页面 | 路径 | 功能 |
|------|------|------|
| 首页 | pages/index/index | 股票列表、搜索 |
| 股票详情 | pages/stock/stock | K 线图、指标展示 |
| 分析 | pages/analysis/analysis | 分析信号、进度 |
| 我的 | pages/mine/mine | 用户信息、设置 |

## TabBar 配置

```json
{
  "tabBar": {
    "list": [
      { "pagePath": "pages/index/index", "text": "首页" },
      { "pagePath": "pages/analysis/analysis", "text": "分析" },
      { "pagePath": "pages/mine/mine", "text": "我的" }
    ]
  }
}
```

## API 接口

小程序调用 Node.js 服务 API：

| 接口 | 说明 |
|------|------|
| `/api/stocks` | 获取股票列表 |
| `/api/stocks/search` | 搜索股票 |
| `/api/stocks/:code` | 获取股票详情 |
| `/api/analysis/signals` | 获取分析信号 |
| `/api/analysis/progress` | 获取分析进度 |
| `/api/charts/:code` | 获取 K 线数据 |
| `/api/auth/qrcode` | 获取登录二维码 |
| `/api/auth/status` | 查询登录状态 |

## 快速开始

### 1. 导入项目

使用微信开发者工具导入 `miniprogram/` 目录。

### 2. 配置 AppID

在 `project.config.json` 中设置：

```json
{
  "appid": "your_appid_here"
}
```

### 3. 配置 API 地址

在 `utils/config.js` 中设置：

```javascript
const API_BASE = 'https://your-domain/api';
```

### 4. 编译运行

点击微信开发者工具的"编译"按钮。

## 发布流程

1. 在微信开发者工具中点击"上传"
2. 在微信公众平台提交审核
3. 审核通过后发布

## 权限说明

| 权限 | 说明 |
|------|------|
| 用户信息 | 用于登录绑定 |
| 相册 | 保存图表截图（可选） |

## 注意事项

- 小程序域名需要在小程序后台配置白名单
- API 地址必须使用 HTTPS
- 需要配置 request 合法域名

## 许可证

MIT License