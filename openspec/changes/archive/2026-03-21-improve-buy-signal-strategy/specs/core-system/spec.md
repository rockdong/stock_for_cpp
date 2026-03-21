# 核心系统规格说明 - 变更增量

## ADDED Requirements

### Requirement: 策略趋势判断窗口化
系统必须支持可配置的趋势判断窗口参数。

#### Scenario: 默认窗口判断
- **WHEN** 用户未指定趋势判断窗口参数
- **THEN** 系统使用默认值 trend_days=3
- **AND** 只检查最近3天的 EMA 趋势

#### Scenario: 自定义窗口判断
- **WHEN** 用户指定趋势判断窗口参数 trend_days=5
- **THEN** 系统检查最近5天的 EMA 趋势
- **AND** 所有指定天数内 EMA 必须呈上升趋势

#### Scenario: 窗口参数验证
- **WHEN** 用户设置 trend_days <= 0 或 trend_days > 数据长度
- **THEN** 系统返回 std::nullopt 表示无法判断
- **AND** 记录警告日志说明参数无效

### Requirement: 趋势判断逻辑改进
系统必须使用窗口化趋势判断替代全序列检查。

#### Scenario: 窗口化趋势向上判断
- **WHEN** 最近 N 天（N=trend_days）EMA 值连续上升
  - 每天的 EMA 值严格大于前一天
- **THEN** 系统判定趋势向上，返回 true

#### Scenario: 窗口内趋势中断判断
- **WHEN** 最近 N 天内任意一天的 EMA 值 <= 前一天
- **THEN** 系统判定趋势不满足，返回 false
- **AND** 不继续检查剩余天数

#### Scenario: 数据不足判断
- **WHEN** EMA 数据长度 < trend_days + 1
- **THEN** 系统返回 false
- **AND** 记录调试日志说明数据不足

## MODIFIED Requirements

### Requirement: 策略信号输出结构
系统必须为策略分析结果提供完整的信号信息。

#### Scenario: 完整信号输出
- **WHEN** 策略生成买入或卖出信号
- **THEN** 系统返回 AnalysisResult 结构，包含：
  - ts_code: 股票代码
  - strategy_name: 策略名称
  - trade_date: 交易日期
  - label: 信号标签（买入/卖出/持有）
  - opt: 操作类型（buy/sell/hold）
  - freq: 频率（d/w/m）
  - strength: 信号强度（STRONG/MEDIUM/WEAK）【新增】
  - confidence: 置信度（0-100）【新增】
  - risk_warning: 风险提示文本【新增】

#### Scenario: 默认值兼容性
- **WHEN** 策略未提供信号强度和置信度
- **THEN** 系统使用默认值
  - strength = MEDIUM
  - confidence = 50.0
  - risk_warning = ""（空字符串）
- **AND** 现有调用代码无需修改即可编译通过

---
**规格版本**: v1.1 (增量)
**创建日期**: 2026年3月21日
**适用范围**: 策略引擎趋势判断逻辑改进