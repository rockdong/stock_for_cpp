# 启动分析检查优化 - 技术设计文档

## Context

### 背景
当前系统在定时模式下启动时，会无条件执行一次完整的股票分析流程（`main.cpp` 第 619-621 行）。这种设计在以下场景导致问题：

- **运维重启**：配置更新、故障恢复、版本升级等正常操作触发重复分析
- **资源浪费**：每次分析数千只股票，耗时数小时，消耗 Tushare API 配额（500次/分钟限制）
- **已有数据未利用**：系统已记录分析日期（`data/.last_run`）和进度状态（`analysis_progress` 表），但启动流程未检查

### 当前架构状态
```
程序启动流程（定时模式）:
┌─────────────────────────────────┐
│ main.cpp: main()                │
│ - 初始化系统 (配置、日志、数据库)│
│ - 加载股票列表                   │
│ - ❌ 强制执行一次分析            │ ← 问题点：无条件执行
│ - 创建 Scheduler                │
│ - 启动调度器 run()              │
│   - shouldRunNow()              │
│     - hasRunToday()             │ ← 已有检查机制，但未被启动流程使用
│     - 交易日判断                 │
│     - 时间匹配                   │
└─────────────────────────────────┘
```

### 约束条件
- **不破坏兼容性**：`--once` 模式必须保持强制执行能力
- **性能要求**：检查操作应在毫秒级完成，不阻塞启动流程
- **数据一致性**：文件记录和数据库状态可能不一致，需要容错处理
- **跨天逻辑**：凌晨 2:00 前使用前一天日期，需与现有 `calculateAnalysisDate()` 兼容

### 相关方
- **运维人员**：频繁重启场景的主要受益者
- **开发人员**：需要理解新的启动检查逻辑
- **API 供应商**：Tushare（减少不必要的 API 调用）

## Goals / Non-Goals

### Goals
1. **智能启动检查**：启动时判断当天是否已完成完整分析，避免重复执行
2. **双重验证机制**：文件记录（快速）+ 数据库进度（准确）的组合检查
3. **完整日志记录**：明确记录跳过/执行原因，便于运维排查
4. **保持兼容性**：不影响 `--once` 强制执行、定时调度、交易日判断等现有功能

### Non-Goals
1. **不修改分析逻辑**：分析算法、策略引擎、数据处理流程保持不变
2. **不新增数据结构**：不创建新的数据库表或文件记录机制
3. **不优化分析性能**：仅优化启动流程，不涉及分析本身的性能改进
4. **不处理并发重启**：不考虑多个实例同时启动的竞态条件（单实例架构）

## Decisions

### 决策 1: 检查机制架构

**选择：双重验证机制（文件记录 + 数据库进度）**

**理由**：
- 文件记录检查快速（读取 `data/.last_run`，毫秒级），适合首次筛查
- 数据库进度检查准确（验证 `analysis_progress` 表状态），确保完整性
- 单一检查存在缺陷：
  - 仅文件：可能记录损坏、人为修改、状态不一致
  - 仅数据库：需要提前建立数据库连接，启动流程耦合度高

**备选方案**：

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| **仅文件检查** | 最快、无依赖 | 不准确、易损坏 | ❌ 不采用 |
| **仅数据库检查** | 最准确 | 需提前连接、耦合高 | ❌ 不采用 |
| **双重检查（推荐）** | 快速 + 准确 | 逻辑稍复杂 | ✅ 采用 |

**实现设计**：
```cpp
// main.cpp 启动前检查流程
bool needFirstRun = true;

// Step 1: 文件记录快速检查
std::ifstream lastRunFile("data/.last_run");
if (lastRunFile.is_open()) {
    std::string lastDate;
    lastRunFile >> lastDate;
    std::string analysisDate = calculateAnalysisDate();
    
    if (lastDate == analysisDate) {
        needFirstRun = false;
        LOG_INFO("文件记录显示今天已完成分析");
    }
}

// Step 2: 数据库进度精确验证（仅在文件显示已完成时）
if (!needFirstRun) {
    auto progress = getAnalysisProgress(); // 查询 analysis_progress 表
    
    if (progress && 
        progress.status == "completed" && 
        progress.completed == progress.total) {
        LOG_INFO("数据库进度确认完成，跳过首次执行");
    } else {
        needFirstRun = true; // 文件显示完成但数据库未完成，重新执行
        LOG_WARN("文件记录与数据库状态不一致，将重新执行");
    }
}

// Step 3: 执行决策
if (needFirstRun) {
    LOG_INFO("首次启动或数据不完整，立即执行分析...");
    performBatchAnalysis(...);
}
```

---

### 决策 2: 检查时机与方法暴露

**选择：在 `main.cpp` 启动前检查 + `Scheduler` 暴露公共方法**

**理由**：
- `main.cpp` 是启动流程的入口，在此检查最直观
- `Scheduler` 已有 `hasRunToday()` 私有方法，暴露为公共方法可复用
- 避免 `Scheduler` 内部逻辑修改，保持其核心调度职责不变

**备选方案**：

| 方案 | 实现位置 | 复杂度 | 维护性 |
|------|----------|--------|--------|
| **仅 Scheduler 内部检查** | Scheduler::run() | 低 | 中（调度器职责混合） |
| **仅 main.cpp 独立实现** | main.cpp | 中 | 低（代码重复） |
| **组合方式（推荐）** | main.cpp + Scheduler 公共方法 | 中 | 高（职责清晰、可复用） |

**实现设计**：

**Scheduler.h 扩展**：
```cpp
class Scheduler {
public:
    // 新增公共方法
    bool hasRunToday() const;  // 检查今天是否已执行
    std::string getLastRunDate() const;  // 获取最后执行日期
    
    // 保持原有方法
    void run();
    void stop();
    void setOnceMode(bool once);
};
```

**main.cpp 使用**：
```cpp
// 创建调度器（用于定时执行）
scheduler::Scheduler sched(options.executeTime, [&]() {
    performBatchAnalysis(...);
});

g_scheduler = &sched;

// 检查是否需要首次执行
if (options.onceMode) {
    // --once 模式：强制执行，不受检查限制
    LOG_INFO("--once 模式：强制执行分析...");
    performBatchAnalysis(...);
} else if (sched.hasRunToday()) {
    // 文件记录显示已完成，进一步验证数据库
    auto progress = getAnalysisProgress();
    if (progress && progress.status == "completed") {
        LOG_INFO("今天已完成分析，跳过首次执行，进入调度等待");
    } else {
        LOG_WARN("进度状态不完整，重新执行分析");
        performBatchAnalysis(...);
    }
} else {
    // 未完成或无记录，立即执行
    LOG_INFO("首次启动，立即执行分析...");
    performBatchAnalysis(...);
}

// 启动调度器
sched.run();
```

---

### 决策 3: 数据完整性验证逻辑

**选择：验证 `analysis_progress` 表的三个字段组合**

**验证条件**：
1. `status == "completed"`（状态标记）
2. `completed == total`（计数器一致性）
3. `updated_at >= started_at`（时间逻辑合理）

**理由**：
- 单字段验证可能误判：
  - 仅 `status`：可能被错误设置为 "completed"
  - 仅计数器：可能部分完成但未更新状态
- 三字段组合验证确保真实性

**备选方案**：

| 验证方式 | 误判风险 | 复杂度 |
|----------|----------|--------|
| 仅 status 字段 | 高 | 低 |
| 仅 completed == total | 中 | 低 |
| 三字段组合（推荐） | 低 | 中 |

**实现设计**：

**AnalysisProcessRecordDAO 新增方法**：
```cpp
struct AnalysisProgress {
    int total;
    int completed;
    int failed;
    std::string status;  // "running" / "completed" / "failed"
    std::string started_at;
    std::string updated_at;
};

class AnalysisProcessRecordDAO {
public:
    // 新增方法
    AnalysisProgress getProgress() const;
    
    // 保持原有方法
    void upsert(const StockProcessRecord& record);
    StockProcessRecord findByTsCode(const std::string& tsCode);
};
```

**验证逻辑**：
```cpp
AnalysisProgress progress = processRecordDao.getProgress();

bool isComplete = 
    progress.status == "completed" &&
    progress.completed == progress.total &&
    progress.total > 0;  // 至少有一条分析任务
```

---

### 决策 4: 异常场景处理策略

**选择：容错优先 - 检查失败时默认执行分析**

**异常场景分类**：

| 异常场景 | 检查结果 | 处理策略 | 日志级别 |
|----------|----------|----------|----------|
| 文件不存在 | 未完成 | 执行分析 | INFO |
| 文件读取失败 | 检查失败 | 执行分析 | WARN |
| 数据库连接失败 | 检查失败 | 执行分析 | WARN |
| 进度表不存在 | 未完成 | 执行分析 | INFO |
| 数据字段异常 | 检查失败 | 执行分析 | ERROR |

**理由**：
- **安全优先**：宁可多执行一次，不可漏执行（影响业务）
- **数据一致性**：执行分析会自动更新记录，纠正不一致状态
- **运维友好**：明确日志记录异常，便于后续排查

**实现设计**：
```cpp
// 异常捕获示例
try {
    std::ifstream file("data/.last_run");
    if (!file.is_open()) {
        LOG_INFO("无执行记录文件，将执行首次分析");
        return true;  // needFirstRun = true
    }
    // ... 检查逻辑
} catch (const std::exception& e) {
    LOG_ERROR("文件检查异常: " + std::string(e.what()) + "，默认执行分析");
    return true;  // 异常时默认执行
}

try {
    auto progress = processRecordDao.getProgress();
    // ... 验证逻辑
} catch (const std::exception& e) {
    LOG_ERROR("数据库检查异常: " + std::string(e.what()) + "，默认执行分析");
    return true;  // 异常时默认执行
}
```

## Risks / Trade-offs

### Risk 1: 文件记录与数据库状态不一致

**风险描述**：
- 文件显示已完成，但数据库进度表显示 "running" 或部分完成
- 可能原因：上次执行中断、进程崩溃、手动修改文件

**影响**：可能误判导致跳过执行，当天数据不完整

**缓解措施**：
- ✅ 双重验证：文件显示完成时，进一步验证数据库状态
- ✅ 自动修复：重新执行分析会更新记录，纠正不一致
- ✅ 明确日志：记录不一致情况，便于运维识别

---

### Risk 2: 跨天重启时间窗口判断错误

**风险描述**：
- 凌晨 00:00 - 02:00 重启时，`calculateAnalysisDate()` 返回前一天日期
- 文件记录可能是前一天的，导致误判为"已完成"

**影响**：凌晨重启可能跳过当天首次分析

**缓解措施**：
- ✅ 时间窗口兼容：检查日期使用 `calculateAnalysisDate()`（与分析日期一致）
- ✅ 调度器兜底：即使跳过首次执行，定时调度器仍会在当天指定时间执行
- ✅ `--once` 兜底：运维可通过 `--once` 强制执行

**验证场景**：
```
场景1: 凌晨 01:00 重启（前一天 20:00 已完成）
  - calculateAnalysisDate() = 前一天
  - 文件记录 = 前一天
  - 判断: 已完成（正确，前一天已分析）
  - 等待到当天 02:00 后，调度器会用新日期

场景2: 凌晨 03:00 重启（当天未执行）
  - calculateAnalysisDate() = 当天
  - 文件记录 = 前一天
  - 判断: 未完成（正确）
  - 立即执行当天首次分析
```

---

### Risk 3: 检查性能开销

**风险描述**：
- 文件读取和数据库查询增加启动时间
- 大型数据库查询可能阻塞启动

**影响**：启动流程延迟（毫秒级影响可忽略）

**缓解措施**：
- ✅ 文件检查优先：毫秒级完成，快速筛查
- ✅ 数据库查询优化：`analysis_progress` 表仅一行记录，查询极快（< 5ms）
- ✅ 仅在必要时查询：文件显示已完成才验证数据库

**性能预估**：
```
文件检查: 1-2ms (读取 20 字节文件)
数据库查询: 3-5ms (单行记录查询)
总开销: < 10ms (对比分析耗时数小时，可忽略)
```

---

### Risk 4: 新旧版本兼容性

**风险描述**：
- 升级后旧版本遗留的 `.last_run` 文件可能格式不同
- 旧版本可能无 `analysis_progress` 表

**影响**：检查失败，默认执行分析（安全但可能重复）

**缓解措施**：
- ✅ 容错设计：检查失败时默认执行（安全优先）
- ✅ 数据库迁移：`analysis_progress` 表已存在（v2.0+）
- ✅ 文件格式兼容：`.last_run` 仅日期字符串，格式不变

---

### Trade-off: 代码复杂度增加

**权衡描述**：
- 新增检查逻辑增加 `main.cpp` 和 `Scheduler` 的代码复杂度
- 需要理解双重验证机制才能维护

**收益**：
- ✅ 避免重复执行，节省大量时间和资源
- ✅ 提升运维效率，减少 API 调用成本

**成本**：
- ⚠️ 新增约 50 行检查逻辑代码
- ⚠️ 需要文档说明检查机制

**结论**：收益远大于成本（数小时分析 vs 50 行代码）

## Migration Plan

### 部署步骤

1. **代码修改**（开发阶段）
   - 修改 `Scheduler.h/cpp`：暴露 `hasRunToday()` 公共方法
   - 修改 `main.cpp`：启动前增加检查逻辑
   - 新增 `AnalysisProcessRecordDAO::getProgress()` 方法

2. **本地测试**（测试阶段）
   ```bash
   # 编译
   cd cpp && mkdir build && cd build && cmake .. && make
   
   # 测试场景
   ./stock_for_cpp              # 定时模式，首次启动
   ./stock_for_cpp              # 定时模式，重启（已完成）
   ./stock_for_cpp --once       # 强制模式（不受检查限制）
   ```

3. **Docker 部署**（生产阶段）
   ```bash
   # 构建新镜像
   docker build -t stock-analysis:v2.1 .
   
   # 部署
   docker-compose down
   docker-compose up -d
   
   # 验证日志
   docker-compose logs -f stock-analysis | grep "跳过首次执行"
   ```

### 回滚策略

如果新版本出现问题，可快速回滚：

```bash
# 回滚到旧版本
docker-compose down
docker tag stock-analysis:v2.0 stock-analysis:latest
docker-compose up -d

# 旧版本行为：无条件执行首次分析（安全但可能重复）
```

**回滚影响**：
- 旧版本会无条件执行首次分析（原有行为）
- 无数据损坏风险，仅可能重复执行一次

### 验证检查

部署后验证以下场景：

| 场景 | 验证方法 | 期望结果 |
|------|----------|----------|
| 首次启动 | 清空 `.last_run`，启动 | 执行分析 |
| 重启（已完成） | 保持 `.last_run`，重启 | 跳过执行 |
| `--once` 强制 | 使用 `--once` 参数 | 执行分析 |
| 异常恢复 | 手动破坏 `.last_run`，重启 | 执行分析 + 记录异常日志 |

## Open Questions

### Q1: 是否需要支持"强制重新分析"参数？

**当前状态**：`--once` 模式可强制执行，但执行后退出程序

**问题**：定时模式下是否需要 `--force` 参数强制重新分析当天数据？

**备选方案**：
- **方案A**：不新增参数，运维可用 `--once` 或手动删除 `.last_run` 文件
- **方案B**：新增 `--force-reanalyze` 参数，在定时模式下强制执行并继续调度

**倾向方案A**：保持简单，手动删除文件已足够

---

### Q2: 是否需要检查分析结果数量？

**当前状态**：仅检查进度表状态，不验证实际分析结果数量

**问题**：是否需要检查 `analysis_results` 表中当天记录数？

**备选方案**：
- **方案A**：仅检查进度表（当前设计）
- **方案B**：额外检查 `analysis_results` 表记录数与进度表 `completed` 一致

**倾向方案A**：
- 进度表已有 `completed` 计数器，足够可信
- 检查结果表增加数据库查询复杂度
- 执行分析时会自动生成结果记录

---

### Q3: 是否需要支持多实例并发？

**当前状态**：单实例架构，不考虑并发

**问题**：如果未来部署多个分析实例，文件记录机制会冲突

**备选方案**：
- **方案A**：保持单实例，当前设计不变
- **方案B**：改用数据库锁或分布式锁机制

**倾向方案A**：
- 当前架构明确为单实例
- 多实例场景需求不明确，过早优化

---

### Q4: 数据库进度表的更新时机是否需要优化？

**当前状态**：`analysis_progress` 表在分析开始和完成时更新

**问题**：如果分析中断（进程崩溃），进度表可能停留在 "running" 状态

**备选方案**：
- **方案A**：保持现有逻辑，重启时检测 "running" 状态视为未完成
- **方案B**：新增超时判断，"running" 状态超过 N 小时视为失败

**倾向方案A**：
- 重启时检测 "running" 会重新执行分析（安全）
- 超时判断增加复杂度，当前简单逻辑已足够

---

## 附录：代码修改清单

### 修改文件清单

| 文件 | 修改类型 | 主要变更 |
|------|----------|----------|
| `cpp/scheduler/Scheduler.h` | 扩展 | 新增 `hasRunToday()` 公共方法 |
| `cpp/scheduler/Scheduler.cpp` | 扩展 | 暴露私有方法实现为公共方法 |
| `cpp/main.cpp` | 修改 | 第 619-630 行：增加启动检查逻辑 |
| `cpp/data/database/AnalysisProcessRecordDAO.h` | 扩展 | 新增 `getProgress()` 方法 |
| `cpp/data/database/AnalysisProcessRecordDAO.cpp` | 扩展 | 实现 `getProgress()` 方法 |

### 不修改文件

- API 层（Node.js）：无需修改
- 前端界面：无需修改
- 数据库表结构：无需修改
- 策略引擎：无需修改
- 定时调度核心逻辑：无需修改