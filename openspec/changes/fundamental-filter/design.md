## Context

当前系统是纯技术面分析引擎，每日对全市场5000+股票运行多种技术策略（EMA/MACD/RSI等）。主要问题：
- 分析量大、耗时长，信号质量参差不齐
- 基本面差的股票也可能产生买入信号，缺少风险过滤
- Web端API请求超时（10秒timeout不够）
- 缺少请求日志，难以诊断问题

约束条件：
- 不新增数据库表（基本面数据不持久化）
- 不修改定时任务（在现有流程中串联）
- 需遵守Tushare API限流（500次/分钟）
- 需兼容现有技术分析框架

## Goals / Non-Goals

**Goals:**
- 实现基本面筛选引擎，每日获取财务指标并计算评分
- 优化技术分析流程，只对优质股票执行（减少96%计算量）
- 新增Web前端基本面筛选页面
- 添加API请求日志，便于诊断超时问题
- 优化前端timeout，解决超时错误

**Non-Goals:**
- 不持久化基本面数据到数据库
- 不新增定时任务（在现有流程中串联）
- 不实现行业对比归一化（第一期先用固定阈值）
- 不实现历史回测（第二期功能）

## Decisions

### 1. 数据获取策略

**决策**: 从 Tushare `fina_indicator` API 获取财务指标

**备选方案**:
| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| fina_indicator API | 数据全面，包含PE/PB/ROE等核心指标 | 需积分权限 | ✅ 采用 |
| income/balance API | 数据更详细 | 需多次调用，增加API次数 | ❌ 不采用 |
| 本地缓存 | 减少API调用 | 需维护缓存更新机制 | ❌ 不采用（第一期简化） |

**理由**: `fina_indicator` 提供所有需要的指标，一次调用即可获取全市场数据

### 2. 评分模型设计

**决策**: 4维度加权评分模型

**评分公式**:
```
TotalScore = ValuationScore(25分) + QualityScore(35分) 
            + GrowthScore(25分) + HealthScore(15分)

各维度计算:
- 估值(25): PE<30(10分) + PB<5(8分) + PEG<1.5(7分)
- 盈利(35): ROE≥10(15分) + ROE≥20额外(5分) + 毛利率≥20(10分) + 净利率>10(5分)
- 增长(25): 营收增长≥5(10分) + 利润增长≥5(10分) + 双>15额外(5分)
- 健康(15): 负债率<60(8分) + 流动比率≥1(4分) + 现金流健康(3分)

评级: A(≥80) B(≥60) C(≥40) D(<40)
筛选阈值: 总分≥60 且 ROE≥10
```

**备选方案**:
| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| 固定阈值评分 | 简单易实现，可快速落地 | 不同行业标准不同 | ✅ 采用（第一期） |
| 行业对比评分 | 更科学，考虑行业差异 | 实现复杂，需行业数据 | ❌ 第二期 |
| 机器学习评分 | 自适应优化 | 需大量训练数据 | ❌ 未来考虑 |

### 3. 执行流程设计

**决策**: 在 main.cpp 中串联执行，先基本面筛选，后技术分析

**流程**:
```
main.cpp 执行流程:
1. 获取全市场财务指标 (Tushare API)
2. 计算基本面评分，筛选优质池 (内存计算)
3. 对优质池中的股票获取行情数据
4. 运行所有技术策略
5. 保存结果到数据库
6. 飞书通知
```

**备选方案**:
| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| 串联执行 | 简单，无额外调度 | 基本面失败影响技术分析 | ✅ 采用 |
| 定时任务分离 | 解耦，独立运行 | 需修改Scheduler，增加复杂度 | ❌ 不采用 |
| Node.js调用 | Web端可控 | C++与Node.js通信复杂 | ❌ 不采用 |

### 4. API设计

**决策**: Node.js 新增 `/api/fundamentals/*` 端点

**端点设计**:
| 端点 | 功能 | Timeout |
|------|------|---------|
| GET /fundamentals/filter | 基本面筛选 | 60秒 |
| GET /fundamentals/:code | 单股详情 | 30秒 |
| GET /fundamentals/progress | 筛选进度 | 10秒 |

**请求日志设计**:
```
日志格式: [requestId] START/END method url statusCode durationMs
示例: [1704123456-a7b8c9] START GET /fundamentals/filter
      [1704123456-a7b8c9] END 200 15234ms
慢请求警告: duration > 5秒 → logger.warn
超时警告: duration > 10秒 → logger.warn
```

### 5. 前端设计

**决策**: 新增独立基本面筛选页面，Tab导航切换

**页面结构**:
```
App.tsx
├─ TabNav (基本面筛选 | 技术策略分析)
├─ FundamentalPage
│   ├─ FundamentalFilter (筛选条件)
│   ├─ FundamentalCardGrid (结果展示)
│   └─ FundamentalChartModal (详情图表)
└─ AnalysisPage (现有技术分析页)
```

**组件设计**:
- FundamentalFilter: 支持PE/ROE/毛利率等筛选条件
- FundamentalCard: 显示股票基本信息、评分、评级、各维度得分
- FundamentalChartModal: 显示基本面指标趋势图表

### 6. Timeout优化

**决策**: 默认30秒，基本面筛选60秒

**配置**:
```
web-frontend/src/services/api.ts:
- 默认 timeout: 30000ms
- 基本面筛选 timeout: 60000ms

环境变量:
- WEB_API_TIMEOUT=30000
```

## Risks / Trade-offs

### 风险1: Tushare API限流
**风险**: 获取全市场财务指标可能触发500次/分钟限制
**缓解**: 
- 使用 RateLimiter 控制请求频率
- 分批获取（每批500只，间隔1分钟）
- 失败时记录日志并跳过

### 风险2: 基本面数据时效性
**风险**: 财务指标是季报数据，不是每日更新
**缓解**: 
- 使用最近一期数据
- 在UI上显示数据日期提醒用户
- 未来可接入预告数据

### 风险3: 超时问题
**风险**: 全市场筛选可能超时
**缓解**: 
- 增加timeout到60秒
- 添加请求日志便于诊断
- 后端优化查询性能

### 风险4: 固定阈值不适配所有行业
**风险**: 不同行业PE/ROE标准不同
**缓解**: 
- 第一期先用固定阈值，简化实现
- 第二期实现行业对比归一化
- 配置文件支持调整阈值

### Trade-offs
- **简化 vs 科学**: 第一期用固定阈值，牺牲科学性换取快速落地
- **内存 vs 存储**: 不持久化基本面数据，牺牲历史查询换取简化
- **串联 vs 并行**: 流程串联执行，牺牲独立可控换取简化

## Migration Plan

### 阶段1: C++核心实现（第1-2天）
1. 创建 FundamentalData.h 数据结构
2. 创建 FundamentalFilter.h/cpp 筛选器
3. 扩展 TushareDataSource 添加 getFinancialIndicators
4. 修改 main.cpp 串联流程
5. 本地测试

### 阶段2: Node.js API实现（第1天）
1. 创建 requestLogger.js 中间件
2. 扩展 api.js 添加 fundamentals 端点
3. 修改 index.js 添加请求日志
4. 测试 API

### 阶段3: Web前端实现（第2天）
1. 创建 fundamentalApi.ts 服务
2. 创建 fundamental.ts 类型
3. 创建 FundamentalFilter 组件
4. 创建 FundamentalCard 组件
5. 创建 FundamentalPage 页面
6. 创建 TabNav 导航
7. 修改 App.tsx 路由
8. 修改 api.ts timeout

### 阶段4: 集成测试（第1天）
1. 端到端测试
2. 性能测试（全市场筛选时间）
3. 超时测试
4. 验证飞书通知

### 回滚策略
- 如有问题，删除新增文件即可
- 不修改现有核心逻辑，回滚简单
- 配置开关可关闭基本面筛选

## Open Questions

1. **Tushare积分权限**: 是否有权限调用 fina_indicator API？需确认
2. **筛选结果保存**: 第一期不保存，后续是否需要保存历史优质池？
3. **行业对比**: 第二期实现行业归一化的具体算法？
4. **图表展示**: 基本面指标图表用什么形式？（趋势图 vs 柱状图）