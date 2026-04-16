## Why

当前系统仅支持纯技术面分析（EMA/MACD/RSI等），对全市场5000+股票每日运行所有策略，导致：
1. 分析量大、耗时长，信号质量参差不齐
2. 基本面差的股票也可能产生买入信号，缺少风险过滤
3. 用户希望"优先选择基本面优的股票"，但系统缺少基本面维度

本变更引入基本面筛选作为技术分析的前置过滤器，先筛选优质股票池，再对优质池进行技术分析，大幅提升分析效率与信号质量。

## What Changes

- **新增基本面筛选功能**：每日获取全市场财务指标，计算基本面评分（估值+盈利+增长+健康），筛选优质股票池
- **优化技术分析流程**：只对基本面优质股票进行技术策略分析，减少约96%计算量
- **新增Web前端基本面筛选页**：独立页面展示基本面筛选结果和评分详情
- **新增Node.js API端点**：基本面筛选、评分计算、详情查询等API
- **添加API请求日志**：所有Web端请求添加详细日志，便于诊断超时问题
- **优化前端timeout**：API timeout从10秒调整为30秒，基本面筛选特殊处理为60秒

## Capabilities

### New Capabilities
- `fundamental-filter`: 基本面筛选引擎，获取财务指标数据，计算综合评分，返回优质股票列表（内存计算，不持久化）
- `fundamental-api`: Node.js REST API端点，提供基本面筛选、详情查询、评分计算等接口
- `fundamental-ui`: Web前端基本面筛选页面，展示筛选结果、评分卡片、指标图表
- `request-logging`: API请求日志中间件，记录所有HTTP请求的完整生命周期（请求参数、响应状态、耗时等）

### Modified Capabilities
- `technical-analysis`: 技术策略分析流程改为只对基本面优质股票执行，输入范围从全市场变为筛选后的优质池

## Impact

**C++ 端**:
- 新增 `cpp/core/FundamentalData.h`: 财务指标数据结构（内存使用）
- 新增 `cpp/core/FundamentalFilter.h/cpp`: 基本面筛选器（计算评分，返回优质池）
- 修改 `cpp/network/TushareDataSource.h/cpp`: 添加 `getFinancialIndicators()` 方法
- 修改 `cpp/main.cpp`: 基本面筛选 → 技术分析流程串联

**Node.js 端**:
- 新增 `nodejs/src/middleware/requestLogger.js`: 请求日志中间件
- 修改 `nodejs/src/api.js`: 新增 `/api/fundamentals/*` 端点
- 修改 `nodejs/src/index.js`: 添加请求日志中间件

**Web 前端**:
- 新增 `web-frontend/src/pages/Fundamental/index.tsx`: 基本面筛选页
- 新增 `web-frontend/src/components/Filter/FundamentalFilter.tsx`: 筛选条件组件
- 新增 `web-frontend/src/components/Card/FundamentalCard.tsx`: 评分卡片组件
- 新增 `web-frontend/src/components/Navigation/TabNav.tsx`: 页面切换导航
- 新增 `web-frontend/src/services/fundamentalApi.ts`: 基本面API服务
- 新增 `web-frontend/src/types/fundamental.ts`: 基本面类型定义
- 修改 `web-frontend/src/App.tsx`: Tab路由切换
- 修改 `web-frontend/src/services/api.ts`: timeout从10秒改为30秒

**配置**:
- 新增环境变量 `FUNDAMENTAL_PE_MAX`, `FUNDAMENTAL_ROE_MIN` 等筛选阈值配置
- 新增环境变量 `WEB_API_TIMEOUT` 控制前端超时时间

**数据依赖**:
- Tushare API `fina_indicator`: 获取财务指标数据（PE/PB/ROE/毛利率/负债率等）
- 无新增数据库表（基本面数据不持久化，只在内存中使用）