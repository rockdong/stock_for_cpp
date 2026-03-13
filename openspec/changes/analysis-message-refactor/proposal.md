## Why

当前"分析结果"命令返回单条消息，包含所有策略结果混合在一起。随着策略数量增加，消息会变得冗长难以阅读。用户希望按策略分组，每个策略单独发送一条消息，便于快速定位和阅读。

同时优化显示格式：股票代码去除后缀（.SZ/.SH），移除冗余的"信号"列（策略名称已隐含信号含义）。

## What Changes

- "分析结果"命令返回多条消息，按策略分组
- 股票代码显示格式优化：`000001.SZ` → `000001`
- 移除"信号"列，简化表格
- handler.js 支持发送多条消息

## Capabilities

### New Capabilities

- `multi-strategy-messages`: 分析结果按策略分组发送多条消息

### Modified Capabilities

- `feishu-reply`: 修改飞书回复格式，支持消息数组

## Impact

**修改文件：**
- `nodejs/src/reply.js` - formatLatestAnalysisAsTable() 重构
- `nodejs/src/handler.js` - handleMessage() 支持消息数组

**兼容性：**
- 保持现有命令入口不变
- 单策略时仍发送一条消息（无感知）
- 多策略时发送多条消息