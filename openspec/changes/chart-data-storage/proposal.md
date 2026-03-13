## Why

当前分析结果只存储策略判断结果（买入/卖出/持有），缺少关键指标数据和价格信息。用户无法直观查看分析时的市场状态和指标位置，难以验证策略判断的准确性。

通过存储 EMA17、EMA25 和 K线数据，并生成可视化图表，用户可以在飞书中直接查看分析依据，提升决策透明度和策略可信度。

## What Changes

- 新增 `chart_data` 数据库表，存储股票图表数据（JSON 格式）
- C++ 分析引擎在执行策略分析时，计算并存储 EMA17、EMA25 指标值和 OHLC 价格数据
- Node.js 服务新增图表生成功能，支持通过飞书命令查询图表
- 图表展示最近 10 条 K 线数据，叠加 EMA17 和 EMA25 指标线

## Capabilities

### New Capabilities
- `chart-data-storage`: 图表数据存储能力，包括数据库表设计、数据写入和查询接口
- `chart-generation`: 图表生成能力，包括 K 线图绘制、指标线叠加、图片生成和飞书发送

### Modified Capabilities

无

## Impact

**C++ 端：**
- `Connection.cpp`: 新增 `chart_data` 表创建
- `ChartDataDAO.h/cpp`: 新增 DAO 类，负责图表数据的存储
- `main.cpp`: `analyzeStock()` 函数新增图表数据保存逻辑

**Node.js 端：**
- `database.js`: 新增 `getChartData()` 查询方法
- `chartGenerator.js`: 新增图表生成模块
- `reply.js`: 新增 "图表 <代码>" 命令处理
- `package.json`: 新增 `lightweight-charts` 和 `canvas` 依赖

**数据库：**
- 新增 `chart_data` 表，预估数据量：5000 股票 × 3 频率 × 30 天 ≈ 450000 条记录