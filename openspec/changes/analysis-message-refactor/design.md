## Context

当前"分析结果"命令在 `reply.js` 中格式化为单条消息返回。数据从 `database.js` 的 `findLatestAnalysisResults()` 获取，返回最新分析日期的所有记录。

消息格式化流程：
```
用户 → "分析结果" → getReply() → formatLatestAnalysisAsTable() → 单条文本消息
```

## Goals / Non-Goals

**Goals:**
- 按策略分组发送多条消息
- 优化股票代码显示（去除后缀）
- 简化表格列（移除信号列）

**Non-Goals:**
- 不改变数据库查询逻辑
- 不添加新的过滤参数
- 不修改飞书 API 调用方式

## Decisions

### 决策 1: 消息分组策略

**选择**: 在 `getReply()` 中按 `strategy_name` 分组，返回消息数组

**理由**:
- 最小改动范围，仅修改 `reply.js` 和 `handler.js`
- 不影响其他命令的处理逻辑
- 单策略时返回数组长度为 1，向下兼容

**实现**:
```javascript
// reply.js
function formatLatestAnalysisAsTable(results) {
  // 按策略分组
  const groups = {};
  results.forEach(r => {
    const key = r.strategy_name;
    if (!groups[key]) groups[key] = [];
    groups[key].push(r);
  });
  
  // 返回消息数组
  return Object.entries(groups).map(([strategy, items]) => ({
    text: formatStrategyTable(strategy, items)
  }));
}
```

### 决策 2: 代码格式化

**选择**: 在显示时去除 `.SZ`/`.SH` 后缀

**理由**:
- 保持数据库原始数据不变
- 仅影响用户展示层
- 后缀信息对用户无实际价值

**实现**:
```javascript
const shortCode = ts_code.split('.')[0];  // 000001.SZ → 000001
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
|------|----------|
| 多条消息发送失败部分成功 | 当前无事务机制，接受最终一致性 |
| 策略数量过多（10+）导致消息刷屏 | 后续可添加分页或摘要机制 |