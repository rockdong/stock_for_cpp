## Why

`commands/index.js` 和 `commands/stock.js` 等模块存在循环依赖：
- `index.js` 导入 `./stock` 在导出 `register` 之前
- `stock.js` 导入 `./index` 获取 `register`
- 结果：`register` 在 `stock.js` 执行时还是 `undefined`

导致 Node.js 服务启动失败，supervisor 连续重试后进入 FATAL 状态。

## What Changes

- 将命令模块加载移到 `module.exports` 之后
- 确保 `register` 函数在命令模块加载前已导出

## Capabilities

### New Capabilities

无

### Modified Capabilities

无

## Impact

- 修改: `nodejs/src/commands/index.js`