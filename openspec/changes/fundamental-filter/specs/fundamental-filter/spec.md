## ADDED Requirements

### Requirement: 获取财务指标数据
系统 SHALL 从 Tushare fina_indicator API 获取全市场股票的财务指标数据。

#### Scenario: 成功获取财务指标
- **WHEN** 调用 getFinancialIndicators() 方法
- **THEN** 返回包含 PE/PB/ROE/毛利率/负债率等指标的 FinancialIndicator 结构列表

#### Scenario: API限流保护
- **WHEN** 连续调用超过500次/分钟
- **THEN** RateLimiter 自动延迟请求，确保不超过限流阈值

#### Scenario: API调用失败
- **WHEN** Tushare API返回错误
- **THEN** 记录错误日志并返回空列表，不中断后续流程

### Requirement: 计算基本面评分
系统 SHALL 对每只股票计算4维度综合评分（估值+盈利+增长+健康）。

#### Scenario: 计算估值得分
- **WHEN** 股票 PE<30 且 PB<5 且 PEG<1.5
- **THEN** 估值得分 = 10 + 8 + 7 = 25分（满分）

#### Scenario: 计算盈利得分
- **WHEN** 股票 ROE≥10 且 毛利率≥20 且 净利率>10
- **THEN** 盈利得分 = 15 + 10 + 5 = 30分（满分）

#### Scenario: 计算增长得分
- **WHEN** 股票 营收增长≥5 且 利润增长≥5 且 双>15
- **THEN** 增长得分 = 10 + 10 + 5 = 25分（满分）

#### Scenario: 计算健康得分
- **WHEN** 股票 负债率<60 且 流动比率≥1 且 现金流健康
- **THEN** 健康得分 = 8 + 4 + 3 = 15分（满分）

#### Scenario: 确定评级
- **WHEN** 总分≥80
- **THEN** 评级为 A（优质）
- **WHEN** 总分≥60
- **THEN** 评级为 B（良好）
- **WHEN** 总分≥40
- **THEN** 评级为 C（一般）
- **WHEN** 总分<40
- **THEN** 评级为 D（较差）

### Requirement: 筛选优质股票池
系统 SHALL 根据评分结果筛选出优质股票列表。

#### Scenario: 标准筛选
- **WHEN** 总分≥60 且 ROE≥10
- **THEN** 股票加入优质池

#### Scenario: 自定义阈值筛选
- **WHEN** 用户指定 min_score=70 和 min_roe=15
- **THEN** 只返回满足条件的股票

#### Scenario: 空结果处理
- **WHEN** 筛选条件过严格导致无结果
- **THEN** 返回空列表并记录日志提示

### Requirement: 配置筛选阈值
系统 SHALL 支持通过环境变量配置筛选阈值。

#### Scenario: 默认阈值
- **WHEN** 未配置环境变量
- **THEN** 使用默认阈值: PE_MAX=30, ROE_MIN=10, TOTAL_SCORE_MIN=60

#### Scenario: 自定义阈值
- **WHEN** 配置 FUNDAMENTAL_PE_MAX=25 和 FUNDAMENTAL_ROE_MIN=15
- **THEN** 筛选使用自定义阈值