## Why

Web 前端点击"查看图表"按钮没有效果。根因是前端代码使用旧的数据结构（期望 `strategy_name`, `freq`, `data[]`），但后端已重构为新结构（`data.strategies[]`）。

## What Changes

- 更新前端类型定义 `AnalysisProcessRecord` 以匹配新的后端数据结构
- 修改 `Analysis/index.tsx` 中的图表数据处理逻辑
- 更新 `RecordTable.tsx` 以正确显示策略和频率信息
- 从嵌套的 `data.strategies[].freqs[]` 中提取图表数据

## Capabilities

### Modified Capabilities
- `analysis-process-display`: 前端显示适配新的嵌套 JSON 结构

## Impact

**前端代码变更**:
- `web-frontend/src/types/analysis.ts` - 更新类型定义
- `web-frontend/src/pages/Analysis/index.tsx` - 更新图表数据处理
- `web-frontend/src/components/Table/RecordTable.tsx` - 更新表格显示

**API 兼容**: 后端 API 已返回正确的新结构，无需修改