## Context

`AnalysisResultDAO.insert()` 方法在插入分析结果时，会检查 `(ts_code, trade_date, strategy_name)` 是否已存在。如果存在，则追加 opt/freq 字段（用 `|` 分隔）。

当前问题：同一天重跑时，相同的 opt/freq 值会被重复追加，导致数据膨胀。

## Goals / Non-Goals

**Goals:**
- 实现 opt/freq 字段的幂等追加（使用集合去重）
- 如果数据未变化，跳过数据库更新以提升性能
- 添加 split/join 工具函数

**Non-Goals:**
- 不修改数据库表结构
- 不改变 label 字段的处理逻辑
- 不改变 `(ts_code, trade_date, strategy_name)` 作为唯一键的业务语义

## Decisions

### D1: 使用 std::set 进行去重

**选择**: 使用 `std::set<std::string>` 存储分割后的元素，自动去重

**理由**:
- 简单直接，无需额外实现去重逻辑
- 性能足够（opt/freq 字段最多 3 个元素：d/w/m）
- 字符串精确匹配，不会误判

**替代方案**:
- A: 使用 `std::find()` 字符串查找 → 可能误判（如 "d" 在 "abc" 中找到）
- B: 先删除再插入 → 需要修改分析流程，侵入性大

### D2: 工具函数放在 StringUtil 中

**选择**: 在 `cpp/utils/StringUtil.h/cpp` 中添加 `split()` 和 `join()` 函数

**理由**:
- StringUtil 已存在，符合项目结构
- 这些函数是通用的，其他地方也可能用到

### D3: 无变化时跳过更新

**选择**: 检查集合是否有变化，无变化则跳过 UPDATE

**理由**:
- 减少不必要的数据库写操作
- 简单实现：`std::set::insert()` 返回 pair，second 表示是否插入了新元素

## Risks / Trade-offs

| 风险 | 缓解措施 |
|------|----------|
| set 不保持插入顺序 | opt 映射是固定的（☀️=d, ⭐=w, 🌙=m），顺序不影响语义 |
| split 空字符串 | 空字符串返回空集合，逻辑正确处理 |
| 旧数据格式不一致 | 现有数据格式已标准化，无需迁移 |