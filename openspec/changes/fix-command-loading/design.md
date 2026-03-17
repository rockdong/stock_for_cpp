## Context

当前架构：
```
index.js
├── 导入 handler.js
└── handler.js
    └── 导入 commands/index.js (route 函数)
        └── commands Map = 空 (没有任何命令被注册)

commands/stock.js, analysis.js, etc.
└── register('股票', ...) ← 从未执行，因为模块未被导入
```

问题：命令模块从未被 `index.js` 导入，`register()` 调用从未执行。

## Goals / Non-Goals

**Goals:**
- 确保所有命令模块在服务启动时被加载
- 命令注册在 route() 调用前完成

**Non-Goals:**
- 不改变命令注册机制
- 不添加新功能

## Decisions

### 方案选择：在 commands/index.js 中集中导入

**选择方案**：在 `commands/index.js` 中导入所有命令模块

**原因**：
- 单一入口，易于维护
- 添加新命令只需创建文件，无需修改 index.js
- 符合模块化设计原则

**备选方案**：
| 方案 | 优点 | 缺点 |
|------|------|------|
| 在 index.js 中导入 | 直接 | 分散，每添加命令需修改 |
| 在 commands/index.js 中导入 | 集中管理 | 需手动添加导入 |
| 自动扫描目录 | 完全自动化 | 增加复杂度 |

**实现**：

```javascript
// commands/index.js
const logger = require('../logger');

// 导入所有命令模块（触发 register）
require('./stock');
require('./analysis');
require('./chart');
require('./progress');
require('./help');

// ... 其余代码不变
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
|------|----------|
| 忘记添加新命令导入 | 添加注释提醒 |
| 循环依赖 | 命令模块不依赖 commands/index.js |