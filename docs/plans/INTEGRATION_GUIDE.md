# 数据库并发优化集成指南

**Worktree**: `.worktrees/db-concurrency`
**分支**: `feature/database-concurrency-optimization`
**已完成**: 7/15 任务

---

## 已完成组件

| 组件 | 文件 | 状态 |
|------|------|------|
| ConnectionPool | cpp/data/database/ConnectionPool.h/cpp | ✅ |
| WriteQueue | cpp/data/database/WriteQueue.h/cpp | ✅ |
| Node.js Proxy | nodejs/src/proxy.js | ✅ |

---

## 待集成任务

### Task 8: Node.js 改用 Proxy

修改 `nodejs/src/index.js`，将所有 `/api/*` 路由改为使用 proxy：

```javascript
const { proxyRequest } = require('./proxy');

// 股票
app.get('/api/stocks', (req, res) => proxyRequest(req, res, '/api/stocks'));
app.get('/api/stocks/search', (req, res) => proxyRequest(req, res, '/api/stocks/search'));
app.get('/api/stocks/:code', (req, res) => proxyRequest(req, res, `/api/stocks/${req.params.code}`));

// 分析
app.get('/api/analysis/process', (req, res) => proxyRequest(req, res, '/api/analysis/process'));
// ... 其他路由类似
```

---

### Task 9: Connection 改用 ConnectionPool

修改 `cpp/data/database/Connection.h/cpp`，移除全局 `g_db`。

---

### Task 10-11: DAO/API 改用连接池

修改 `StockDAO.cpp` 和 `ApiRouter.cpp`：

```cpp
// 读操作
auto conn = ConnectionPool::getInstance().acquireRead();
// 执行查询
ConnectionPool::getInstance().releaseRead(conn);

// 写操作
auto conn = ConnectionPool::getInstance().acquireWrite();
// 执行写入
ConnectionPool::getInstance().releaseWrite();
```

---

### Task 12: main.cpp 初始化

```cpp
#include "ConnectionPool.h"
#include "WriteQueue.h"

int main() {
    ConnectionPool::getInstance().initialize(5);
    WriteQueue::getInstance().start();
    
    // 程序结束时
    WriteQueue::getInstance().stop();
    ConnectionPool::getInstance().shutdown();
}
```

---

### Task 13: CMakeLists.txt

```cmake
set(DB_CONCURRENCY_SRC
    data/database/ConnectionPool.cpp
    data/database/WriteQueue.cpp
)
target_sources(stock_for_cpp PRIVATE ${DB_CONCURRENCY_SRC})
```

---

### Task 14: Docker Compose

```yaml
services:
  stock-analysis:
    environment:
      - CPP_API_URL=http://localhost:3001
```

---

## 继续执行

在新会话中：
1. 进入 worktree: `cd .worktrees/db-concurrency`
2. 使用 `executing-plans` skill 继续 Task 8-15
3. 计划文件: `docs/plans/2026-04-17-database-concurrency-optimization.md`

---

## 提交记录

```
dbd6e9f feat(db): add ConnectionPool for read/write separation (draft)
27e95cd feat(db): add WriteQueue and Node.js proxy for database concurrency optimization
```