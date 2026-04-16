## ADDED Requirements

### Requirement: 基本面筛选API
系统 SHALL 提供 GET /api/fundamentals/filter 端点用于基本面筛选。

#### Scenario: 成功筛选
- **WHEN** GET /api/fundamentals/filter?min_roe=10&max_pe=30&min_score=60&limit=100
- **THEN** 返回 { success: true, data: [筛选结果列表], meta: { total: N } }

#### Scenario: 长时间请求
- **WHEN** 全市场筛选耗时超过30秒
- **THEN** API正常返回结果，前端timeout设置为60秒

#### Scenario: 参数缺失
- **WHEN** 未提供筛选参数
- **THEN** 使用默认阈值筛选，返回结果

#### Scenario: 筛选失败
- **WHEN** Tushare API调用失败
- **THEN** 返回 { success: false, error: "错误信息" }，状态码500

### Requirement: 单股基本面详情API
系统 SHALL 提供 GET /api/fundamentals/:code 端点获取单只股票基本面详情。

#### Scenario: 成功获取详情
- **WHEN** GET /api/fundamentals/000001.SZ
- **THEN** 返回 { success: true, data: { ts_code, name, indicators, score } }

#### Scenario: 股票不存在
- **WHEN** GET /api/fundamentals/invalid_code
- **THEN** 返回 { success: false, error: "无基本面数据" }，状态码404

### Requirement: 筛选进度API
系统 SHALL 提供 GET /api/fundamentals/progress 端点获取筛选进度。

#### Scenario: 获取进度
- **WHEN** GET /api/fundamentals/progress
- **THEN** 返回 { success: true, data: { status: "running/completed", progress: 50, total: 5000 } }

#### Scenario: 未开始筛选
- **WHEN** 尚未执行筛选
- **THEN** 返回 { success: true, data: { status: "idle" } }

### Requirement: 返回数据结构
系统 SHALL 返回标准化的基本面数据结构。

#### Scenario: 筛选结果数据结构
- **WHEN** 返回筛选结果
- **THEN** 每条记录包含: ts_code, name, industry, pe, pb, roe, gross_margin, net_margin, revenue_growth, profit_growth, debt_ratio, score(total, valuation, quality, growth, health), grade

#### Scenario: 详情数据结构
- **WHEN** 返回单股详情
- **THEN** 包含完整财务指标和评分详情