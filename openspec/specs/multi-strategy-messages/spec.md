# Multi-Strategy Messages

## Purpose

将分析结果按策略分组发送多条消息，优化用户阅读体验。

## Requirements

### Requirement: Analysis results grouped by strategy
The system SHALL group analysis results by strategy name and send each group as a separate message.

#### Scenario: Single strategy results
- **WHEN** user sends "分析结果" and all results belong to one strategy
- **THEN** the system SHALL send one message with that strategy's results

#### Scenario: Multiple strategy results
- **WHEN** user sends "分析结果" and results belong to multiple strategies
- **THEN** the system SHALL send one message per strategy
- **AND** each message SHALL contain only results from that strategy

#### Scenario: No analysis results
- **WHEN** user sends "分析结果" and no results exist
- **THEN** the system SHALL send a message indicating no results found

### Requirement: Stock code display format
The system SHALL display stock codes without the exchange suffix.

#### Scenario: Code with SZ suffix
- **WHEN** displaying stock code "000001.SZ"
- **THEN** the system SHALL show "000001"

#### Scenario: Code with SH suffix
- **WHEN** displaying stock code "600001.SH"
- **THEN** the system SHALL show "600001"

### Requirement: Simplified analysis table
The analysis results table SHALL NOT include the signal column.

#### Scenario: Table columns
- **WHEN** formatting analysis results as a table
- **THEN** the table SHALL have columns: 代码, 名称, opt
- **AND** the table SHALL NOT have a "信号" column