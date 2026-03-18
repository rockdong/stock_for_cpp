## Why

同一天多次重跑分析任务时，`AnalysisResultDAO.insert()` 会重复追加 opt/freq 字段，导致数据膨胀。例如第二次运行后 opt 从 `☀️|⭐|🌙` 变成 `☀️|⭐|🌙|☀️|⭐|🌙`。

重跑场景：发现代码修改后，通过 GitHub Action 编译上传 Docker，重新拉取并重跑。

## What Changes

- `AnalysisResultDAO.insert()` 方法改用集合去重逻辑，确保 opt/freq 字段幂等
- 在 `StringUtil` 工具类中添加 `split()` 和 `join()` 函数支持集合操作
- 优化：如果数据未变化，跳过数据库更新操作

## Capabilities

### New Capabilities
- `idempotent-data-insert`: 分析结果数据插入的幂等性保证

### Modified Capabilities
- 无（这是 bug 修复，不改变 spec 级别的行为要求）

## Impact

- `cpp/data/database/AnalysisResultDAO.cpp` - insert() 方法逻辑修改
- `cpp/utils/StringUtil.h` - 添加 split/join 函数声明
- `cpp/utils/StringUtil.cpp` - 实现 split/join 函数