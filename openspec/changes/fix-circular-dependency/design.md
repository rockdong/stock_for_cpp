## Context

当前代码执行顺序：

```javascript
// commands/index.js (当前)
const logger = require('../logger');

require('./stock');  // ← 此时 register 还未定义！

const commands = new Map();
// ...
function register() { }
module.exports = { register };
```

循环依赖链：
```
index.js ──require──▶ stock.js
    ▲                      │
    │                      │ require('./index')
    └──────────────────────┘
```

## Goals / Non-Goals

**Goals:**
- 打破循环依赖
- 确保 register 在命令模块加载前可用

**Non-Goals:**
- 不改变命令注册机制

## Decisions

**方案**: 将 `require('./stock')` 移到 `module.exports` 之后

```javascript
// commands/index.js (修复后)
const logger = require('../logger');

const commands = new Map();
const commandsList = [];

function register(prefix, handler, description = '') {
  commands.set(prefix, handler);
  commandsList.push({ prefix, description });
  logger.debug(`注册命令: ${prefix}`);
}

function route(text) {
  const trimmedText = text.trim();
  for (const [prefix, handler] of commands) {
    if (trimmedText === prefix || trimmedText.startsWith(prefix + ' ')) {
      logger.debug(`匹配命令: ${prefix}`);
      return handler(trimmedText);
    }
  }
  return null;
}

function getCommandsList() {
  return commandsList;
}

module.exports = {
  register,
  route,
  getCommandsList,
};

// 导出完成后才加载命令模块
require('./stock');
require('./analysis');
require('./chart');
require('./progress');
require('./help');
```

## Risks / Trade-offs

无