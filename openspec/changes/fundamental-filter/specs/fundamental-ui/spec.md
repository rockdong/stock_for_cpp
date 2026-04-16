## ADDED Requirements

### Requirement: Tab导航
系统 SHALL 提供Tab导航组件用于切换基本面筛选和技术策略分析页面。

#### Scenario: Tab切换
- **WHEN** 点击"基本面筛选"Tab
- **THEN** 显示基本面筛选页面
- **WHEN** 点击"技术策略分析"Tab
- **THEN** 显示技术策略分析页面

### Requirement: 基本面筛选页面
系统 SHALL 提供独立的基本面筛选页面。

#### Scenario: 页面加载
- **WHEN** 进入基本面筛选页面
- **THEN** 显示筛选条件表单和结果展示区域

#### Scenario: 筛选条件表单
- **WHEN** 页面渲染
- **THEN** 显示筛选条件: 最小ROE、最大PE、最小毛利率、最小增长、最大负债率、最小总分、行业、市场

#### Scenario: 提交筛选
- **WHEN** 点击"开始筛选"按钮
- **THEN** 调用 API 执行筛选，显示加载状态，返回结果后更新展示

### Requirement: 基本面评分卡片
系统 SHALL 提供评分卡片组件展示单只股票的基本面信息。

#### Scenario: 卡片信息
- **WHEN** 渲染评分卡片
- **THEN** 显示: 股票代码、名称、行业、总分、评级(A/B/C/D)、各维度得分（估值/盈利/增长/健康）

#### Scenario: 查看详情
- **WHEN** 点击卡片"查看详情"按钮
- **THEN** 打开详情模态框显示完整指标

### Requirement: 超时错误处理
系统 SHALL 正确处理API超时错误。

#### Scenario: 显示超时提示
- **WHEN** API请求超时
- **THEN** 显示友好提示"请求超时，请稍后重试"，不显示原始错误堆栈

#### Scenario: 重试机制
- **WHEN** 超时后用户点击重试
- **THEN** 重新发起请求

### Requirement: Timeout配置
系统 SHALL 对不同API使用不同的timeout配置。

#### Scenario: 默认timeout
- **WHEN** 普通API请求
- **THEN** timeout = 30秒

#### Scenario: 基本面筛选timeout
- **WHEN** 基本面筛选API请求
- **THEN** timeout = 60秒