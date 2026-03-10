## Why

当前系统仅在用户主动查询时获取股票数据，缺乏实时监控特定股票价格变动的功能。用户无法及时获得重要的价格变动通知，错过了关键的投资时机。添加实时监控功能将使系统能够主动推送价格变动信息，大大提升用户体验和投资决策的及时性。

## What Changes

- **添加多线程价格监控引擎**：在C++后端实现高效的并发价格监控
- **实现价格变动检测算法**：支持百分比变动、绝对值变动等多种触发条件
- **创建监控配置管理**：允许用户设置个性化监控规则和通知阈值
- **集成多种通知渠道**：支持飞书机器人、邮件、短信等通知方式
- **优化数据库存储结构**：添加价格历史记录表和监控配置表

## Capabilities

### New Capabilities
- `real-time-monitoring`: 持续监控指定股票的价格变动，支持自定义监控频率
- `price-change-detection`: 智能检测价格变动，支持涨跌幅、成交量等技术指标触发
- `notification-engine`: 多渠道通知系统，支持分级通知和去重机制
- `monitoring-configuration`: 灵活的监控规则配置，支持定时监控和条件触发

### Modified Capabilities
- `data-fetching`: 扩展现有的数据获取模块，支持高频实时数据获取
- `database-operations`: 增加价格监控相关的数据读写操作

## Impact

- `cpp/data/DataSource.h`: 扩展数据源接口，添加实时订阅功能
- `cpp/analysis/PriceMonitor.h`: 新增价格监控核心模块
- `cpp/notification/NotificationManager.h`: 新增通知管理模块
- `nodejs/src/routes/monitoring.js`: 新增Node.js监控API路由
- `database/schema.sql`: 更新数据库表结构，添加监控相关表