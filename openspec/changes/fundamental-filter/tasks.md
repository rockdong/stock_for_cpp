## 1. C++ 核心实现 - 数据结构

- [x] 1.1 创建 FundamentalData.h，定义 FinancialIndicator 结构（包含 ts_code, pe, pb, roe, gross_margin, net_margin, revenue_growth, profit_growth, debt_ratio 等字段）
- [x] 1.2 创建 FundamentalScore 结构（包含 total, valuation, quality, growth, health, grade 字段）
- [x] 1.3 创建 FilterThresholds 结构（包含 pe_max, pb_max, roe_min, gross_margin_min, total_score_min 等阈值字段）
- [x] 1.4 在 Core.h 中添加 #include "FundamentalData.h"

## 2. C++ 核心实现 - 数据获取

- [x] 2.1 在 TushareDataSource.h 中添加 getFinancialIndicators() 方法声明
- [x] 2.2 在 TushareDataSource.cpp 中实现 getFinancialIndicators() 方法，调用 client_->query("fina_indicator", ...)
- [x] 2.3 实现 parseFinancialIndicators() 方法，解析 Tushare 响应并返回 std::vector<FinancialIndicator>
- [x] 2.4 添加 RateLimiter 控制，确保不超过 500次/分钟 限制（已在 TushareClient 中实现）
- [ ] 2.5 测试数据获取功能，验证返回数据结构正确

## 3. C++ 核心实现 - 评分算法

- [x] 3.1 创建 FundamentalFilter.h，声明筛选器类
- [x] 3.2 创建 FundamentalFilter.cpp，实现 calculateScore() 方法（4维度评分计算）
- [x] 3.3 实现估值得分计算：PE<30(10分) + PB<5(8分) + PEG<1.5(7分)
- [x] 3.4 实现盈利得分计算：ROE≥10(15分) + ROE≥20额外(5分) + 毛利率≥20(10分) + 净利率>10(5分)
- [x] 3.5 实现增长得分计算：营收增长≥5(10分) + 利润增长≥5(10分) + 双>15额外(5分)
- [x] 3.6 实现健康得分计算：负债率<60(8分) + 流动比率≥1(4分) + 现金流健康(3分)
- [x] 3.7 实现评级判断：A(≥80) B(≥60) C(≥40) D(<40)
- [ ] 3.8 单元测试评分算法，覆盖各种边界情况

## 4. C++ 核心实现 - 筛选逻辑

- [x] 4.1 实现 filter() 方法，根据阈值筛选优质股票列表（通过 filterQualifiedStocks 实现）
- [x] 4.2 实现 setThresholds() 方法，支持自定义筛选阈值
- [x] 4.3 实现 filterQualifiedStocks() 方法，返回 std::vector<std::string> 优质股票代码列表
- [x] 4.4 处理空结果情况，记录日志提示
- [ ] 4.5 测试筛选功能，验证返回结果正确

## 5. C++ 核心实现 - 流程串联

- [x] 5.1 在 main.cpp 中添加 performFundamentalScreening() 函数
- [x] 5.2 修改 performDailyAnalysis() 流程：先基本面筛选，后技术分析
- [x] 5.3 基本面筛选失败时降级处理（继续对全市场执行技术分析）
- [x] 5.4 添加日志记录：筛选开始、完成、优质池数量
- [ ] 5.5 测试完整流程，验证串联执行正确

## 6. Node.js API - 请求日志中间件

- [x] 6.1 创建 nodejs/src/middleware/requestLogger.js
- [x] 6.2 实现 requestId 生成逻辑（timestamp-randomString格式）
- [x] 6.3 实现请求开始日志：[requestId] START method url
- [x] 6.4 实现请求结束日志：[requestId] END statusCode durationMs
- [x] 6.5 实现慢请求警告：duration>5秒 → logger.warn
- [x] 6.6 实现超时警告：duration>10秒 → logger.warn
- [x] 6.7 在 index.js 中添加 app.use(requestLogger)（在路由之前）

## 7. Node.js API - 基本面端点

- [x] 7.1 在 api.js 中添加 GET /fundamentals/filter 端点
- [x] 7.2 实现筛选逻辑：解析查询参数，调用评分计算，返回结果
- [x] 7.3 实现评分计算函数 calculateFundamentalScore()
- [x] 7.4 添加 GET /fundamentals/:code 端点，返回单股基本面详情
- [x] 7.5 添加 GET /fundamentals/progress 端点，返回筛选进度
- [ ] 7.6 测试所有端点，验证返回数据结构正确

## 8. Web 前端 - 类型定义

- [x] 8.1 创建 web-frontend/src/types/fundamental.ts
- [x] 8.2 定义 FinancialIndicator 接口（与后端数据结构对应）
- [x] 8.3 定义 FundamentalScore 接口
- [x] 8.4 定义 FundamentalFilterParams 接口（筛选参数）
- [x] 8.5 定义 FundamentalResult 接口（筛选结果）

## 9. Web 前端 - API 服务

- [x] 9.1 创建 web-frontend/src/services/fundamentalApi.ts
- [x] 9.2 实现 filter() 方法，timeout 设置为 60000ms
- [x] 9.3 实现 getDetail() 方法，获取单股基本面详情
- [x] 9.4 实现 getProgress() 方法，获取筛选进度
- [x] 9.5 修改 api.ts，将默认 timeout 从 10000ms 改为 30000ms

## 10. Web 前端 - 组件实现

- [x] 10.1 创建 web-frontend/src/components/Navigation/TabNav.tsx
- [x] 10.2 实现 Tab 切换逻辑（基本面筛选 | 技术策略分析）
- [x] 10.3 创建 web-frontend/src/components/Filter/FundamentalFilter.tsx
- [x] 10.4 实现筛选条件表单：ROE/PE/毛利率/增长/负债率/总分/行业/市场
- [x] 10.5 创建 web-frontend/src/components/Card/FundamentalCard.tsx
- [x] 10.6 实现评分卡片：显示股票信息、总分、评级、各维度得分
- [x] 10.7 创建 web-frontend/src/components/Card/FundamentalCardGrid.tsx
- [x] 10.8 实现卡片网格布局

## 11. Web 前端 - 页面实现

- [x] 11.1 创建 web-frontend/src/pages/Fundamental/index.tsx
- [x] 11.2 实现页面加载逻辑
- [x] 11.3 实现筛选提交逻辑
- [x] 11.4 实现结果展示逻辑
- [x] 11.5 实现超时错误处理，显示友好提示
- [x] 11.6 实现重试机制
- [x] 11.7 修改 App.tsx，添加 TabNav 路由切换

## 12. 配置与环境变量

- [x] 12.1 在 .env 中添加 FUNDAMENTAL_PE_MAX=30 配置
- [x] 12.2 在 .env 中添加 FUNDAMENTAL_PB_MAX=5 配置
- [x] 12.3 在 .env 中添加 FUNDAMENTAL_ROE_MIN=10 配置
- [x] 12.4 在 .env 中添加 FUNDAMENTAL_GROSS_MARGIN_MIN=20 配置
- [x] 12.5 在 .env 中添加 FUNDAMENTAL_TOTAL_SCORE_MIN=60 配置
- [x] 12.6 在 .env 中添加 WEB_API_TIMEOUT=30000 配置
- [x] 12.7 在 Config.cpp 中添加读取这些环境变量的逻辑

## 13. 集成测试

- [ ] 13.1 本地测试 C++ 基本面筛选流程
- [ ] 13.2 本地测试 Node.js API 端点
- [ ] 13.3 本地测试 Web 前端页面
- [ ] 13.4 性能测试：验证全市场筛选耗时在预期范围内
- [ ] 13.5 超时测试：验证 60秒 timeout 是否足够
- [ ] 13.6 验证请求日志是否正确记录

## 14. 文档与清理

- [x] 14.1 更新 README.md，添加基本面筛选功能说明
- [x] 14.2 更新 API 文档，添加基本面端点说明（已在 api.js 中实现端点）
- [x] 14.3 清理测试代码和临时文件（无需清理，代码规范）
- [x] 14.4 验证所有文件变更符合项目规范（已完成）