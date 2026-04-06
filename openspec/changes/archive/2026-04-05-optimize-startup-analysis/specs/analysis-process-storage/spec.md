# analysis-process-storage Specification (Delta)

## Purpose
扩展分析过程存储能力，新增对 `analysis_progress` 表的查询和验证功能，支持启动检查机制。

## ADDED Requirements

### Requirement: 查询分析进度状态
系统 SHALL 支持查询 `analysis_progress` 表的当前状态，用于验证分析的完整性。

#### Scenario: 查询进度记录
- **WHEN** 调用 `AnalysisProcessRecordDAO.getProgress()` 方法
- **THEN** 系统 SHALL 返回 `AnalysisProgress` 结构体，包含以下字段：
  - total (int): 总任务数
  - completed (int): 已完成任务数
  - failed (int): 失败任务数
  - status (string): 状态（"running" / "completed" / "failed"）
  - started_at (string): 开始时间（ISO 8601 格式）
  - updated_at (string): 最后更新时间（ISO 8601 格式）

#### Scenario: 进度表为空
- **WHEN** `analysis_progress` 表无记录或记录为空
- **THEN** 系统 SHALL 返回空值或默认值（total=0）
- **AND** 系统 SHALL 不抛出异常

#### Scenario: 查询失败
- **WHEN** 数据库查询失败（连接失败、表不存在等）
- **THEN** 系统 SHALL 抛出异常或返回错误码
- **AND** 系统 SHALL 记录错误日志

---

### Requirement: 验证分析完整性
系统 SHALL 支持验证 `analysis_progress` 表记录的分析完整性状态。

#### Scenario: 验证完成状态
- **WHEN** 调用验证方法检查进度状态
- **AND** `status` 字段为 "completed"
- **AND** `completed` 字段等于 `total` 字段
- **AND** `total` 字段大于 0
- **THEN** 系统 SHALL 返回 `true`（分析完整）

#### Scenario: 验证未完成状态
- **WHEN** 调用验证方法检查进度状态
- **AND** `status` 字段不为 "completed"
- **OR** `completed` 字段不等于 `total` 字段
- **THEN** 系统 SHALL 返回 `false`（分析不完整）

#### Scenario: 验证异常状态
- **WHEN** 调用验证方法检查进度状态
- **AND** 字段为 NULL 或异常值
- **THEN** 系统 SHALL 返回 `false`（分析状态异常）

---

### Requirement: AnalysisProgress 数据结构
系统 SHALL 定义标准的 `AnalysisProgress` 数据结构，用于封装进度信息。

#### Scenario: 数据结构定义
- **WHEN** 定义 `AnalysisProgress` 结构体
- **THEN** 结构体 SHALL 包含以下字段：
  ```cpp
  struct AnalysisProgress {
      int total;           // 总任务数
      int completed;       // 已完成任务数
      int failed;          // 失败任务数
      std::string status;  // 状态："running" / "completed" / "failed"
      std::string started_at;   // 开始时间
      std::string updated_at;   // 最后更新时间
  };
  ```

#### Scenario: 空值处理
- **WHEN** `AnalysisProgress` 表示空状态
- **THEN** `total` 字段 SHALL 为 0
- **AND** `status` 字段 SHALL 为空字符串

---

### Requirement: DAO 方法扩展
`AnalysisProcessRecordDAO` 类 SHALL 扩展新方法以支持进度查询。

#### Scenario: getProgress() 方法签名
- **WHEN** 定义 `getProgress()` 方法
- **THEN** 方法签名 SHALL 为：
  ```cpp
  AnalysisProgress getProgress() const;
  ```
- **AND** 方法 SHALL 为 const 方法（不修改对象状态）
- **AND** 方法 SHALL 不抛出异常（异常通过返回值或错误码表示）

#### Scenario: 方法返回值
- **WHEN** 查询成功
- **THEN** 方法 SHALL 返回有效的 `AnalysisProgress` 对象
- **WHEN** 查询失败
- **THEN** 方法 SHALL 返回空 `AnalysisProgress` 对象（total=0, status=""）

---

### Requirement: 与现有存储逻辑兼容
新增的查询和验证方法 SHALL 不影响现有的 upsert 和查询逻辑。

#### Scenario: upsert 操作不受影响
- **WHEN** 调用 `upsert()` 方法保存过程记录
- **THEN** 方法 SHALL 按原有逻辑工作
- **AND** 新增的 `getProgress()` 方法 SHALL 不修改 upsert 行为

#### Scenario: 现有查询接口不受影响
- **WHEN** 调用现有的 `findByTsCode()` 等查询方法
- **THEN** 方法 SHALL 按原有逻辑工作
- **AND** 返回结果 SHALL 与之前一致

#### Scenario: 数据库表结构不变
- **WHEN** 使用新的 DAO 方法
- **THEN** 系统 SHALL 不修改 `analysis_process_records` 表结构
- **AND** 系统 SHALL 不修改 `analysis_progress` 表结构（假设已存在）