## 1. 数据库层

- [x] 1.1 在 Connection.cpp 中添加 chart_data 表创建逻辑（包含 id, ts_code, freq, analysis_date, data, updated_at 字段）
- [x] 1.2 添加 (ts_code, freq, analysis_date) 唯一索引
- [ ] 1.3 编写数据库表创建的单元测试

## 2. C++ 数据访问层

- [x] 2.1 创建 ChartDataDAO.h 头文件，定义 ChartData 结构体和 DAO 接口
- [x] 2.2 实现 ChartDataDAO.cpp，包含 saveChartData() 和 getChartData() 方法
- [x] 2.3 实现 JSON 序列化/反序列化逻辑（OHLC + EMA 数据）
- [ ] 2.4 编写 ChartDataDAO 的单元测试（保存和查询场景）

## 3. C++ 分析引擎集成

- [x] 3.1 修改 analyzeStock() 函数，在分析完成后调用 ChartDataDAO 保存数据
- [x] 3.2 从 StockData 向量中提取最后 10 条 K 线数据
- [x] 3.3 计算 EMA17 和 EMA25 指标值并添加到数据中
- [x] 3.4 实现多频率支持（日线、周线、月线分别存储）
- [ ] 3.5 编写集成测试验证数据存储功能

## 4. Node.js 数据库接口

- [x] 4.1 在 database.js 中添加 getChartData(tsCode, freq) 方法
- [x] 4.2 实现 SQL 查询逻辑，返回最新的图表数据记录
- [x] 4.3 添加错误处理（无数据、查询失败场景）
- [ ] 4.4 编写 database.js 的单元测试

## 5. Node.js 图表生成模块

- [x] 5.1 安装 lightweight-charts 和 node-canvas 依赖
- [x] 5.2 创建 chartGenerator.js 模块
- [x] 5.3 实现 generateChart() 函数，生成 K 线图 + EMA 叠加线
- [x] 5.4 实现图表样式配置（K 线样式、EMA17 蓝线、EMA25 橙线）
- [x] 5.5 实现 PNG 图片导出功能
- [ ] 5.6 编写图表生成模块的单元测试

## 6. 飞书命令处理

- [x] 6.1 在 reply.js 中添加 "图表" 命令处理逻辑
- [x] 6.2 解析命令参数（股票代码、可选频率）
- [x] 6.3 调用 database.js 获取图表数据
- [x] 6.4 调用 chartGenerator.js 生成图表图片
- [x] 6.5 实现图片上传到飞书并发送功能
- [x] 6.6 添加错误处理（无效股票代码、无数据、上传失败）
- [ ] 6.7 编写飞书命令处理的集成测试

## 7. 验证和部署

- [ ] 7.1 在测试环境验证数据库表创建
- [ ] 7.2 在测试环境验证 C++ 数据存储功能
- [ ] 7.3 在测试环境验证 Node.js 图表生成和飞书发送
- [ ] 7.4 执行端到端测试（分析 → 存储 → 查询 → 图表生成 → 飞书发送）
- [ ] 7.5 更新项目文档，添加新功能说明