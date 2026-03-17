## Why

Node.js 飞书机器人重构后，所有命令都返回帮助消息而非正确响应。原因是命令模块（stock.js, analysis.js 等）从未被 index.js 导入，导致 `register()` 调用从未执行，`commands` Map 保持为空，`route()` 总是返回 null。

## What Changes

- 在 `index.js` 或 `commands/index.js` 中添加命令模块导入
- 确保所有命令模块在服务启动时被加载并注册

## Capabilities

### New Capabilities

无

### Modified Capabilities

无（这是 bug 修复，不改变任何规范）

## Impact

- 修改: `nodejs/src/index.js` 或 `nodejs/src/commands/index.js`
- 影响: 所有命令功能（股票、分析、图表、进度、帮助）