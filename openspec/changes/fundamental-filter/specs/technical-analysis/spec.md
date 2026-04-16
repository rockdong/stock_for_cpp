## MODIFIED Requirements

### Requirement: 技术策略分析输入范围
技术策略分析 SHALL 只对基本面优质股票池中的股票执行分析。

#### Scenario: 优质池输入
- **WHEN** 执行技术策略分析
- **THEN** 输入股票列表来自基本面筛选结果（100-300只优质股票）

#### Scenario: 减少计算量
- **WHEN** 基本面筛选返回200只优质股票
- **THEN** 技术策略分析只执行200×5=1000次策略分析，而非5000×5=25000次

#### Scenario: 无优质池时的处理
- **WHEN** 基本面筛选返回空列表
- **THEN** 技术策略分析跳过执行，记录日志提示

#### Scenario: 基本面筛选失败时的处理
- **WHEN** 基本面筛选失败（API错误）
- **THEN** 技术策略分析继续对全市场执行（降级处理）

### Requirement: 技术策略分析流程顺序
技术策略分析 SHALL 在基本面筛选完成后执行。

#### Scenario: 执行顺序
- **WHEN** 每日分析流程启动
- **THEN** 先执行基本面筛选，等待完成后执行技术策略分析

#### Scenario: 流程串联
- **WHEN** 基本面筛选完成
- **THEN** 立即开始技术策略分析（无额外等待时间）

### Requirement: 分析结果附加基本面信息
技术策略分析结果 SHALL 包含基本面评分信息。

#### Scenario: 结果包含评分
- **WHEN** 保存技术分析结果
- **THEN** 结果包含: ts_code, strategy_name, signal, fundamental_score, fundamental_grade

#### Scenario: 飞书通知包含评分
- **WHEN** 推送飞书通知
- **THEN** 通知内容包含"买入信号 (基本面评分: 75分, A级优质)"