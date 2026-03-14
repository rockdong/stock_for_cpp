<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>EMA25CrossoverStrategy 金叉策略分析</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
            min-height: 100vh;
            padding: 20px;
            color: #e0e0e0;
        }
        .container { max-width: 1200px; margin: 0 auto; }
        h1 {
            text-align: center;
            color: #00d9ff;
            margin-bottom: 10px;
            font-size: 28px;
        }
        .subtitle { text-align: center; color: #888; margin-bottom: 30px; }
        .card {
            background: rgba(255, 255, 255, 0.05);
            border-radius: 16px;
            padding: 24px;
            margin-bottom: 20px;
            border: 1px solid rgba(255, 255, 255, 0.1);
        }
        .card h2 {
            color: #00ff88;
            margin-bottom: 16px;
            font-size: 20px;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .card h2::before {
            content: '';
            display: inline-block;
            width: 4px;
            height: 20px;
            background: #00ff88;
            border-radius: 2px;
        }
        .signal-box {
            text-align: center;
            padding: 20px;
            background: linear-gradient(135deg, #00d9ff20, #00ff8820);
            border-radius: 12px;
            border: 2px solid #00ff88;
            margin: 20px 0;
        }
        .signal-box h3 { color: #00ff88; font-size: 24px; margin-bottom: 10px; }
        .signal-box p { color: #aaa; }
        .condition {
            background: rgba(0, 217, 255, 0.1);
            border-left: 4px solid #00d9ff;
            padding: 16px;
            border-radius: 0 8px 8px 0;
            margin-bottom: 12px;
        }
        .highlight { color: #00ff88; font-weight: bold; }
        .chart-container { position: relative; height: 400px; margin-top: 20px; }
        .flow {
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            gap: 10px;
            margin: 20px 0;
        }
        .flow-step {
            flex: 1;
            min-width: 150px;
            text-align: center;
            padding: 16px;
            background: rgba(255, 255, 255, 0.05);
            border-radius: 12px;
            position: relative;
        }
        .flow-step::after {
            content: '→';
            position: absolute;
            right: -15px;
            top: 50%;
            transform: translateY(-50%);
            color: #00d9ff;
            font-size: 20px;
        }
        .flow-step:last-child::after { display: none; }
        .flow-step .num {
            background: #00d9ff;
            color: #000;
            width: 28px;
            height: 28px;
            border-radius: 50%;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            font-weight: bold;
            margin-bottom: 8px;
        }
        .flow-step .title { color: #fff; font-weight: bold; margin-bottom: 4px; }
        .flow-step .desc { color: #888; font-size: 12px; }
        .formula {
            background: #0d1117;
            padding: 16px;
            border-radius: 8px;
            font-family: 'Monaco', monospace;
            color: #c9d1d9;
            font-size: 14px;
            line-height: 1.8;
            overflow-x: auto;
        }
        .key-point {
            background: linear-gradient(135deg, #00ff8820, #00d9ff20);
            border: 2px solid #00ff88;
            border-radius: 12px;
            padding: 20px;
            margin-top: 20px;
        }
        .key-point h3 { color: #00ff88; margin-bottom: 12px; }
        .key-point p { color: #ccc; line-height: 1.8; }
        .comparison {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
        }
        .old-strategy, .new-strategy {
            padding: 20px;
            border-radius: 12px;
        }
        .old-strategy {
            background: rgba(255, 107, 107, 0.1);
            border: 1px solid rgba(255, 107, 107, 0.3);
        }
        .new-strategy {
            background: rgba(0, 255, 136, 0.1);
            border: 1px solid rgba(0, 255, 136, 0.3);
        }
        .old-strategy h3 { color: #ff6b6b; margin-bottom: 12px; }
        .new-strategy h3 { color: #00ff88; margin-bottom: 12px; }
        @media (max-width: 768px) { .comparison { grid-template-columns: 1fr; } }
    </style>
</head>
<body>
    <div class="container">
        <h1>📈 EMA25CrossoverStrategy 金叉策略</h1>
        <p class="subtitle">EMA17 上穿 EMA25 提前识别策略</p>
        
        <div class="card">
            <h2>策略概述</h2>
            <div class="signal-box">
                <h3>🎯 买入信号</h3>
                <p>当 EMA17 从下方穿越 EMA25 时触发（黄金交叉）</p>
            </div>
            <p style="color: #aaa; line-height: 1.8; margin-top: 20px;">
                这是一个<strong style="color: #00ff88;">提前识别</strong>策略：<br>
                在 EMA17 上穿 EMA25 的第一时间捕捉到上涨趋势的开始，
                而不是在趋势已经确认后（收盘价等于 EMA17）才入场。
            </p>
        </div>
        
        <div class="card">
            <h2>触发条件</h2>
            <div class="flow">
                <div class="flow-step">
                    <div class="num">1</div>
                    <div class="title">昨天</div>
                    <div class="desc">EMA17 &lt; EMA25</div>
                </div>
                <div class="flow-step">
                    <div class="num">2</div>
                    <div class="title">今天</div>
                    <div class="desc">EMA17 ≥ EMA25</div>
                </div>
                <div class="flow-step">
                    <div class="num">🎯</div>
                    <div class="title">信号触发</div>
                    <div class="desc">买入！</div>
                </div>
            </div>
            
            <div class="condition">
                <h3>🔍 核心逻辑</h3>
                <p>
                    <strong>昨天 EMA17[i-1] &lt; EMA25[i-1]</strong><br>
                    <strong>今天 EMA17[i] ≥ EMA25[i]</strong><br><br>
                    这表明短期均线从下方穿越长期均线，形成<strong style="color: #00ff88;">黄金交叉</strong>，
                    是<strong style="color: #00ff88;">上涨趋势开始的信号</strong>。
                </p>
            </div>
        </div>
        
        <div class="card">
            <h2>核心代码逻辑</h2>
            <div class="formula">
bool isGoldenCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const {
    size_t lastIndex = fastEMA.size() - 1;
    size_t prevIndex = lastIndex - 1;
    
    double prevFast = fastEMA[prevIndex];   // 昨天的 EMA17
    double prevSlow = slowEMA[prevIndex];   // 昨天的 EMA25
    double currFast = fastEMA[lastIndex];   // 今天的 EMA17
    double currSlow = slowEMA[lastIndex];   // 今天的 EMA25
    
    bool wasBelow = prevFast < prevSlow;          // 昨天在下方
    bool isAboveOrEqual = currFast >= currSlow;  // 今天在上方或相等
    
    return wasBelow && isAboveOrEqual;  // 金叉成立
}
            </div>
        </div>
        
        <div class="card">
            <h2>策略对比</h2>
            <div class="comparison">
                <div class="old-strategy">
                    <h3>❌ 原策略 (EMA25Greater17PriceMatch)</h3>
                    <ul style="list-style: none; color: #ccc; font-size: 14px;">
                        <li>• 触发条件：收盘价 = EMA17（精确相等）</li>
                        <li>• 识别时机：事后确认</li>
                        <li>• 入场时间：较晚</li>
                        <li>• 信号数量：较少</li>
                        <li>• 问题：条件太苛刻</li>
                    </ul>
                </div>
                <div class="new-strategy">
                    <h3>✅ 新策略 (EMA25Crossover)</h3>
                    <ul style="list-style: none; color: #ccc; font-size: 14px;">
                        <li>• 触发条件：EMA17 ≥ EMA25（金叉）</li>
                        <li>• 识别时机：<strong>提前识别</strong></li>
                        <li>• 入场时间：较早</li>
                        <li>• 信号数量：较多</li>
                        <li>• 优势：更早入场</li>
                    </ul>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>可视化演示</h2>
            <div class="chart-container">
                <canvas id="crossoverChart"></canvas>
            </div>
            <p style="text-align: center; color: #888; margin-top: 15px; font-size: 14px;">
                📌 绿色 = EMA25 &nbsp;|&nbsp; 红色 = EMA17 &nbsp;|&nbsp; 黄色 = 收盘价<br>
                <span style="color: #00d9ff;">蓝色虚线</span> = 金叉触发点（EMA17 上穿 EMA25）
            </p>
        </div>
        
        <div class="key-point">
            <h3>💡 关键优势：提前入场</h3>
            <p>
                <strong>原策略</strong>：等待收盘价与 EMA17 精确相等 → <span style="color: #ff6b6b;">已经涨起来了</span><br><br>
                <strong>新策略</strong>：在 EMA17 上穿 EMA25 的第一时间入场 → <span style="color: #00ff88;">买在起涨点</span><br><br>
                这个策略能够在趋势刚刚开始时就识别出来，给你更多的时间优势。
            </p>
        </div>
        
        <div class="card">
            <h2>使用方式</h2>
            <div class="formula">
// 通过名称创建策略
auto strategy = core::StrategyFactory::create("EMA25_CROSSOVER");

// 或使用中文别名
auto strategy2 = core::StrategyFactory::create("金叉");

// 分析数据
auto result = strategy->analyze("000001.SZ", stockData);
if (result) {
    std::cout << "买入信号! " << result->trade_date << std::endl;
}
            </div>
        </div>
        
        <div class="card">
            <h2>策略参数</h2>
            <table style="width: 100%; border-collapse: collapse; color: #ccc; font-size: 14px;">
                <tr style="border-bottom: 1px solid #333;">
                    <th style="text-align: left; padding: 12px; color: #00d9ff;">参数名</th>
                    <th style="text-align: left; padding: 12px; color: #00d9ff;">默认值</th>
                    <th style="text-align: left; padding: 12px; color: #00d9ff;">说明</th>
                </tr>
                <tr style="border-bottom: 1px solid #333;">
                    <td style="padding: 12px;">fast_period</td>
                    <td style="padding: 12px;">17</td>
                    <td style="padding: 12px;">EMA17 周期（短期）</td>
                </tr>
                <tr style="border-bottom: 1px solid #333;">
                    <td style="padding: 12px;">slow_period</td>
                    <td style="padding: 12px;">25</td>
                    <td style="padding: 12px;">EMA25 周期（长期）</td>
                </tr>
                <tr>
                    <td style="padding: 12px;">confirm_days</td>
                    <td style="padding: 12px;">1</td>
                    <td style="padding: 12px;">确认天数（可选）</td>
                </tr>
            </table>
        </div>
    </div>
    
    <script>
        const days = 30;
        const labels = [];
        const closeData = [];
        const ema17Data = [];
        const ema25Data = [];
        
        let price = 100;
        for (let i = 0; i < days; i++) {
            labels.push(`Day ${i + 1}`);
            
            if (i < 20) {
                price = 100 - (20 - i) * 0.8 + Math.random() * 2;
            } else if (i < 28) {
                price = 84 + (i - 20) * 2.5 + Math.random() * 2;
            } else {
                price = 104 + (i - 28) * 0.8 + Math.random() * 1;
            }
            closeData.push(price);
        }
        
        function calculateEMA(data, period) {
            const ema = [];
            const multiplier = 2 / (period + 1);
            let sum = 0;
            for (let i = 0; i < period; i++) sum += data[i];
            let prevEMA = sum / period;
            
            for (let i = 0; i < data.length; i++) {
                if (i < period - 1) {
                    ema.push(null);
                } else if (i === period - 1) {
                    ema.push(prevEMA);
                } else {
                    const currentEMA = (data[i] - prevEMA) * multiplier + prevEMA;
                    ema.push(currentEMA);
                    prevEMA = currentEMA;
                }
            }
            return ema;
        }
        
        ema17Data.push(...calculateEMA(closeData, 17));
        ema25Data.push(...calculateEMA(closeData, 25));
        
        const ctx = document.getElementById('crossoverChart').getContext('2d');
        new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [
                    {
                        label: '收盘价',
                        data: closeData,
                        borderColor: '#ffd93d',
                        backgroundColor: 'rgba(255, 217, 61, 0.1)',
                        borderWidth: 2,
                        fill: true,
                        tension: 0.4,
                        pointRadius: 3,
                        pointBackgroundColor: '#ffd93d'
                    },
                    {
                        label: 'EMA17 (短期)',
                        data: ema17Data,
                        borderColor: '#ff6b6b',
                        borderWidth: 2,
                        fill: false,
                        tension: 0.4,
                        pointRadius: 0
                    },
                    {
                        label: 'EMA25 (长期)',
                        data: ema25Data,
                        borderColor: '#00ff88',
                        borderWidth: 2,
                        fill: false,
                        tension: 0.4,
                        pointRadius: 0
                    }
                ]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { labels: { color: '#ccc' } }
                },
                scales: {
                    x: { grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: '#888' } },
                    y: { grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: '#888' } }
                }
            }
        });
    </script>
</body>
</html>