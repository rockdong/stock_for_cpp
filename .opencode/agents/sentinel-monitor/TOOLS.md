# TOOLS.md - Sentinel's Toolkit

## 监控工具链

### 1. 指标监控
**Prometheus + Grafana**
- 时序数据库，存储指标数据
- Grafana 可视化仪表盘
- AlertManager 告警管理

```yaml
# Prometheus 配置示例
global:
  scrape_interval: 15s
  evaluation_interval: 15s

scrape_configs:
  - job_name: 'app'
    static_configs:
      - targets: ['localhost:9090']
```

### 2. 日志监控
**Loki + Grafana**
- 轻量级日志聚合
- 与 Prometheus 统一查询
- PromQL 日志查询

```bash
# 查询最近错误日志
{app="myapp"} |= "error" | line_format "{{.timestamp}} {{.message}}"
```

### 3. 错误追踪
**Sentry**
- 应用错误自动捕获
- 堆栈追踪和上下文
- 错误聚合和趋势分析

```javascript
// Sentry 初始化
Sentry.init({
  dsn: "YOUR_DSN",
  environment: "production",
  tracesSampleRate: 0.1
})
```

### 4. 链路追踪
**Jaeger / Zipkin**
- 分布式请求追踪
- 服务依赖分析
- 性能瓶颈定位

---

## 目录结构

```
monitoring/
├── dashboards/           # Grafana 仪表盘配置
│   ├── system.json      # 系统监控
│   ├── app.json         # 应用监控
│   └── business.json    # 业务监控
├── alerts/               # 告警记录
│   └── YYYY-MM-DD.md    # 每日告警日志
├── reports/              # 监控报告
│   ├── daily/           # 日报
│   └── weekly/          # 周报
├── analysis/             # 问题分析
│   └── incident-XXX.md  # 事故分析报告
├── metrics/              # 指标数据导出
└── config/               # 监控配置
    ├── prometheus.yml
    ├── alertmanager.yml
    └── rules.yml
```

---

## 监控检查命令

### 系统状态
```bash
# CPU 使用率
top -bn1 | grep "Cpu(s)"

# 内存使用率
free -m

# 磁盘使用率
df -h

# 网络连接
netstat -an | grep ESTABLISHED
```

### 进程状态
```bash
# 查看进程
ps aux | grep [app-name]

# 进程端口
lsof -i :[port]

# 进程资源
pidstat -p [pid] 1 5
```

### 日志检查
```bash
# 最近错误日志
tail -100 /var/log/app/error.log | grep -i error

# 日志统计
grep -c "ERROR" /var/log/app/error.log

# 实时监控
tail -f /var/log/app/error.log
```

### 数据库状态
```bash
# MySQL 连接数
mysql -e "SHOW PROCESSLIST"

# PostgreSQL 连接数
psql -c "SELECT count(*) FROM pg_stat_activity"

# Redis 状态
redis-cli INFO
```

---

## 告警模板

### 系统告警
```markdown
## 🔴 系统告警 - [告警类型]

**时间:** YYYY-MM-DD HH:mm:ss
**主机:** [主机名]
**级别:** P0/P1/P2

**问题:**
[问题描述]
当前值: [数值]
阈值: [阈值]

**影响:**
[影响范围和程度]

**建议措施:**
1. [紧急处理步骤]
2. [排查方向]

**相关指标:**
- CPU: XX%
- 内存: XX%
- 磁盘: XX%

**日志片段:**
```
[关键日志]
```
```

### 应用告警
```markdown
## 🔴 应用告警 - [应用名称]

**时间:** YYYY-MM-DD HH:mm:ss
**应用:** [应用名]
**级别:** P0/P1/P2

**错误信息:**
```
[错误堆栈]
```

**出现频率:**
- 最近 1 小时: XX 次
- 最近 24 小时: XX 次

**影响范围:**
[受影响的用户/功能]

**根因分析:**
[初步分析]

**建议修复:**
1. [修复方向]
2. [临时措施]

**指派:** @[agent-name]
```

---

## 监控仪表盘模板

### 系统监控仪表盘
| Panel | 指标 | 说明 |
|-------|------|------|
| CPU | cpu_usage_percent | CPU 使用率 |
| 内存 | memory_usage_percent | 内存使用率 |
| 磁盘 | disk_usage_percent | 磁盘使用率 |
| 网络 | network_io_bytes | 网络流量 |
| 进程 | process_count | 进程数量 |

### 应用监控仪表盘
| Panel | 指标 | 说明 |
|-------|------|------|
| QPS | http_requests_per_second | 每秒请求数 |
| 响应时间 | http_request_duration_p99 | P99 响应时间 |
| 错误率 | http_error_rate | 错误率 |
| 连接池 | db_connection_pool_active | 活跃连接数 |
| 队列 | queue_length | 队列长度 |

---

## 自动化脚本

### 健康检查脚本
```bash
#!/bin/bash
# health-check.sh

# CPU 检查
CPU_THRESHOLD=80
CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}')
if [ ${CPU_USAGE%.*} -gt $CPU_THRESHOLD ]; then
  echo "CPU 告警: ${CPU_USAGE}%"
fi

# 内存检查
MEM_THRESHOLD=85
MEM_USAGE=$(free | grep Mem | awk '{print ($3/$2)*100}')
if [ ${MEM_USAGE%.*} -gt $MEM_THRESHOLD ]; then
  echo "内存告警: ${MEM_USAGE}%"
fi

# 磁盘检查
DISK_THRESHOLD=80
DISK_USAGE=$(df -h / | tail -1 | awk '{print $5}' | tr -d '%')
if [ $DISK_USAGE -gt $DISK_THRESHOLD ]; then
  echo "磁盘告警: ${DISK_USAGE}%"
fi
```

---

## 常用 PromQL 查询

```promql
# CPU 使用率
rate(process_cpu_seconds_total[5m]) * 100

# 内存使用率
(process_resident_memory_bytes / process_virtual_memory_bytes) * 100

# 请求率
rate(http_requests_total[5m])

# P99 响应时间
histogram_quantile(0.99, rate(http_request_duration_seconds_bucket[5m]))

# 错误率
rate(http_requests_total{status=~"5.."}[5m]) / rate(http_requests_total[5m])
```

---

*Sentinel 的工具库，持续更新中*