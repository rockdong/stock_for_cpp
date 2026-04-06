# startup-analysis-check Specification

## Purpose
定义程序启动前的智能分析检查机制，避免重复执行当天的股票分析任务，提升运维效率和资源利用率。

## ADDED Requirements

### Requirement: 启动前分析状态检查
系统 SHALL 在启动分析流程前检查当天是否已完成完整的股票分析，避免不必要的重复执行。

#### Scenario: 无执行记录 - 首次启动
- **WHEN** `data/.last_run` 文件不存在
- **THEN** 系统 SHALL 判定为"未完成"
- **AND** 系统 SHALL 立即执行首次分析

#### Scenario: 文件记录日期不是今天
- **WHEN** `data/.last_run` 文件记录的日期不等于 `calculateAnalysisDate()` 返回值
- **THEN** 系统 SHALL 判定为"未完成"
- **AND** 系统 SHALL 立即执行首次分析

#### Scenario: 文件记录日期是今天
- **WHEN** `data/.last_run` 文件记录的日期等于 `calculateAnalysisDate()` 返回值
- **THEN** 系统 SHALL 进一步验证数据库进度状态
- **AND** 系统 SHALL 根据 `analysis_progress` 表状态决定是否执行

---

### Requirement: 数据库进度状态验证
系统 SHALL 验证 `analysis_progress` 表的完整性，确保当天分析真正完成。

#### Scenario: 进度表显示已完成
- **WHEN** `analysis_progress` 表中 `status` 字段为 "completed"
- **AND** `completed` 字段等于 `total` 字段
- **AND** `total` 字段大于 0
- **THEN** 系统 SHALL 判定为"已完成"
- **AND** 系统 SHALL 跳过首次执行
- **AND** 系统 SHALL 记录日志 "今天已完成分析，跳过首次执行，进入调度等待"

#### Scenario: 进度表显示未完成
- **WHEN** `analysis_progress` 表中 `status` 字段不为 "completed"
- **OR** `completed` 字段不等于 `total` 字段
- **THEN** 系统 SHALL 判定为"未完成"
- **AND** 系统 SHALL 立即执行首次分析
- **AND** 系统 SHALL 记录日志 "进度状态不完整，重新执行分析"

#### Scenario: 进度表不存在或查询失败
- **WHEN** 查询 `analysis_progress` 表失败（表不存在、数据库连接失败等）
- **THEN** 系统 SHALL 判定为"检查失败"
- **AND** 系统 SHALL 默认执行首次分析（安全优先）
- **AND** 系统 SHALL 记录错误日志 "数据库检查异常，默认执行分析"

---

### Requirement: 双重验证机制
系统 SHALL 实现文件记录和数据库进度的双重验证，确保判断的准确性和容错性。

#### Scenario: 文件未完成 - 跳过数据库检查
- **WHEN** 文件记录检查结果为"未完成"
- **THEN** 系统 SHALL 不再检查数据库进度（优化性能）
- **AND** 系统 SHALL 立即执行首次分析

#### Scenario: 文件已完成 - 进一步验证数据库
- **WHEN** 文件记录检查结果为"已完成"
- **THEN** 系统 SHALL 进一步验证数据库进度状态
- **AND** 系统 SHALL 根据数据库状态做出最终判断

#### Scenario: 文件完成但数据库未完成
- **WHEN** 文件记录显示"已完成"
- **AND** 数据库进度验证显示"未完成"
- **THEN** 系统 SHALL 判定为"数据不一致"
- **AND** 系统 SHALL 重新执行首次分析
- **AND** 系统 SHALL 记录警告日志 "文件记录与数据库状态不一致，将重新执行"

---

### Requirement: 强制执行模式保留
系统 SHALL 保留 `--once` 参数的强制执行能力，不受启动检查机制限制。

#### Scenario: 使用 --once 参数启动
- **WHEN** 用户使用 `--once` 参数启动程序
- **THEN** 系统 SHALL 跳过所有启动检查
- **AND** 系统 SHALL 强制执行分析
- **AND** 系统 SHALL 记录日志 "--once 模式：强制执行分析..."
- **AND** 执行完成后程序 SHALL 退出（原有行为保持不变）

#### Scenario: 定时模式下检查后执行
- **WHEN** 用户未使用 `--once` 参数（定时模式）
- **THEN** 系统 SHALL 执行启动检查逻辑
- **AND** 系统 SHALL 根据检查结果决定是否执行

---

### Requirement: 检查失败容错处理
系统 SHALL 在检查机制失败时默认执行分析，确保数据完整性（安全优先原则）。

#### Scenario: 文件读取失败
- **WHEN** 读取 `data/.last_run` 文件失败（权限、损坏等）
- **THEN** 系统 SHALL 默认执行首次分析
- **AND** 系统 SHALL 记录警告日志 "文件检查异常，默认执行分析"

#### Scenario: 数据库连接失败
- **WHEN** 数据库连接失败导致无法查询进度表
- **THEN** 系统 SHALL 默认执行首次分析
- **AND** 系统 SHALL 记录错误日志 "数据库连接失败，默认执行分析"

#### Scenario: 进度表字段异常
- **WHEN** `analysis_progress` 表字段为 NULL 或异常值
- **THEN** 系统 SHALL 默认执行首次分析
- **AND** 系统 SHALL 记录错误日志 "进度表字段异常，默认执行分析"

---

### Requirement: 跨天时间窗口兼容
系统 SHALL 兼容现有的跨天时间窗口逻辑（凌晨 02:00 前使用前一天日期）。

#### Scenario: 凌晨重启 - 前一天已完成
- **WHEN** 当前时间在 00:00 - 02:00 之间
- **AND** `calculateAnalysisDate()` 返回前一天日期
- **AND** 文件记录显示前一天已完成
- **THEN** 系统 SHALL 判定为"已完成"
- **AND** 系统 SHALL 跳过首次执行
- **AND** 系统 SHALL 等待到当天 02:00 后，调度器使用新日期

#### Scenario: 凌晨重启 - 前一天未完成
- **WHEN** 当前时间在 00:00 - 02:00 之间
- **AND** `calculateAnalysisDate()` 返回前一天日期
- **AND** 文件记录显示前一天未完成
- **THEN** 系统 SHALL 判定为"未完成"
- **AND** 系统 SHALL 立即执行前一天的分析

#### Scenario: 正常时间重启 - 当天未执行
- **WHEN** 当前时间在 02:00 之后
- **AND** `calculateAnalysisDate()` 返回当天日期
- **AND** 文件记录显示当天未完成
- **THEN** 系统 SHALL 判定为"未完成"
- **AND** 系统 SHALL 立即执行当天的分析

---

### Requirement: 日志记录完整性
系统 SHALL 为所有检查场景记录明确的日志，便于运维排查和审计。

#### Scenario: 跳过执行日志
- **WHEN** 系统判定为"已完成"并跳过执行
- **THEN** 系统 SHALL 记录 INFO 级别日志
- **AND** 日志 SHALL 包含以下信息：
  - "今天已完成分析，跳过首次执行，进入调度等待"
  - 文件记录日期
  - 进度表状态（status、completed、total）

#### Scenario: 执行分析日志
- **WHEN** 系统判定为"未完成"并执行分析
- **THEN** 系统 SHALL 记录 INFO 级别日志
- **AND** 日志 SHALL 包含以下信息：
  - "首次启动，立即执行分析..." 或具体原因

#### Scenario: 异常情况日志
- **WHEN** 检查过程出现异常或状态不一致
- **THEN** 系统 SHALL 记录 WARN 或 ERROR 级别日志
- **AND** 日志 SHALL 包含异常原因和堆栈信息（如有）

---

### Requirement: Scheduler 公共方法暴露
Scheduler 类 SHALL 暴露启动检查相关的公共方法，供外部调用。

#### Scenario: 调用 hasRunToday() 方法
- **WHEN** 外部代码调用 `scheduler.hasRunToday()`
- **THEN** 方法 SHALL 返回布尔值
  - `true`: 文件记录显示今天已执行
  - `false`: 文件记录显示今天未执行
- **AND** 方法 SHALL 为 const 方法（不修改对象状态）

#### Scenario: 调用 getLastRunDate() 方法
- **WHEN** 外部代码调用 `scheduler.getLastRunDate()`
- **THEN** 方法 SHALL 返回最后执行日期字符串（格式 YYYY-MM-DD）
- **OR** 方法 SHALL 返回空字符串（文件不存在或读取失败）
- **AND** 方法 SHALL 为 const 方法

---

### Requirement: 性能要求
启动检查操作 SHALL 在毫秒级完成，不显著影响程序启动速度。

#### Scenario: 文件检查性能
- **WHEN** 执行文件记录检查
- **THEN** 操作 SHALL 在 5ms 内完成
- **AND** 操作 SHALL 不阻塞主线程

#### Scenario: 数据库查询性能
- **WHEN** 执行 `analysis_progress` 表查询
- **THEN** 操作 SHALL 在 10ms 内完成
- **AND** 查询 SHALL 使用索引（如果表有索引）

#### Scenario: 总体检查性能
- **WHEN** 执行完整的启动检查流程（文件 + 数据库）
- **THEN** 总耗时 SHALL 不超过 20ms
- **AND** 系统 SHALL 记录检查耗时日志（DEBUG 级别）

---

### Requirement: 不影响现有功能
启动检查机制 SHALL 不影响现有的定时调度、交易日判断、分析逻辑等核心功能。

#### Scenario: 定时调度器正常运行
- **WHEN** 程序启动后进入定时调度模式
- **THEN** 调度器 SHALL 按照配置的时间执行分析（原有行为保持不变）
- **AND** 调度器内部的 `shouldRunNow()` 和 `hasRunToday()` 检查 SHALL 正常工作

#### Scenario: 交易日判断不受影响
- **WHEN** 启动检查判定为"需要执行"
- **AND** 当天为非交易日
- **THEN** 调度器 SHALL 正常跳过执行（原有 `TradeCalendar::isTradingDayToday()` 逻辑）

#### Scenario: 分析逻辑不受影响
- **WHEN** 系统执行分析
- **THEN** 分析流程、策略引擎、数据存储 SHALL 与原有逻辑完全一致
- **AND** 不引入新的分析参数或配置项