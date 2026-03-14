<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>EMA25Greater17PriceMatchStrategy 分析</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
            min-height: 100vh;
            padding: 20px;
            color: #e0e0e0;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        h1 {
            text-align: center;
            color: #00d9ff;
            margin-bottom: 10px;
            font-size: 28px;
        }
        
        .subtitle {
            text-align: center;
            color: #888;
            margin-bottom: 30px;
        }
        
        .strategy-name {
            background: linear-gradient(90deg, #00d9ff, #00ff88);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            font-weight: bold;
        }
        
        .card {
            background: rgba(255, 255, 255, 0.05);
            border-radius: 16px;
            padding: 24px;
            margin-bottom: 20px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
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
        
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
            gap: 20px;
            margin-bottom: 20px;
        }
        
        .condition {
            background: rgba(0, 217, 255, 0.1);
            border-left: 4px solid #00d9ff;
            padding: 16px;
            border-radius: 0 8px 8px 0;
            margin-bottom: 12px;
        }
        
        .condition.ema {
            border-color: #ff6b6b;
            background: rgba(255, 107, 107, 0.1);
        }
        
        .condition.price {
            border-color: #ffd93d;
            background: rgba(255, 217, 61, 0.1);
        }
        
        .condition.match {
            border-color: #6bcb77;
            background: rgba(107, 203, 119, 0.1);
        }
        
        .condition h3 {
            color: #fff;
            font-size: 16px;
            margin-bottom: 8px;
        }
        
        .condition p {
            color: #aaa;
            font-size: 14px;
            line-height: 1.6;
        }
        
        .highlight {
            color: #00ff88;
            font-weight: bold;
        }
        
        .formula {
            background: #0d1117;
            padding: 16px;
            border-radius: 8px;
            font-family: 'Monaco', 'Menlo', monospace;
            color: #c9d1d9;
            font-size: 14px;
            line-height: 1.8;
            overflow-x: auto;
        }
        
        .formula .comment {
            color: #8b949e;
        }
        
        .formula .keyword {
            color: #ff7b72;
        }
        
        .formula .function {
            color: #d2a8ff;
        }
        
        .formula .number {
            color: #79c0ff;
        }
        
        .chart-container {
            position: relative;
            height: 400px;
            margin-top: 20px;
        }
        
        .signal-box {
            text-align: center;
            padding: 20px;
            background: linear-gradient(135deg, #00d9ff20, #00ff8820);
            border-radius: 12px;
            border: 2px solid #00ff88;
            margin: 20px 0;
        }
        
        .signal-box h3 {
            color: #00ff88;
            font-size: 24px;
            margin-bottom: 10px;
        }
        
        .signal-box p {
            color: #aaa;
        }
        
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
        
        .flow-step:last-child::after {
            display: none;
        }
        
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
        
        .flow-step .title {
            color: #fff;
            font-weight: bold;
            margin-bottom: 4px;
        }
        
        .flow-step .desc {
            color: #888;
            font-size: 12px;
        }
        
        .pros-cons {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
        }
        
        .pros, .cons {
            padding: 20px;
            border-radius: 12px;
        }
        
        .pros {
            background: rgba(0, 255, 136, 0.1);
            border: 1px solid rgba(0, 255, 136, 0.3);
        }
        
        .cons {
            background: rgba(255, 107, 107, 0.1);
            border: 1px solid rgba(255, 107, 107, 0.3);
        }
        
        .pros h3 {
            color: #00ff88;
            margin-bottom: 12px;
        }
        
        .cons h3 {
            color: #ff6b6b;
            margin-bottom: 12px;
        }
        
        .pros ul, .cons ul {
            list-style: none;
        }
        
        .pros li, .cons li {
            padding: 6px 0;
            color: #ccc;
            font-size: 14px;
        }
        
        .pros li::before {
            content: '✓';
            color: #00ff88;
            margin-right: 8px;
        }
        
        .cons li::before {
            content: '✗';
            color: #ff6b6b;
            margin-right: 8px;
        }
        
        .timing-diagram {
            background: #0d1117;
            border-radius: 12px;
            padding: 20px;
            margin-top: 20px;
        }
        
        .timing-row {
            display: flex;
            align-items: center;
            padding: 8px 0;
            border-bottom: 1px solid #21262d;
        }
        
        .timing-row:last-child {
            border-bottom: none;
        }
        
        .timing-label {
            width: 120px;
            color: #888;
            font-size: 14px;
        }
        
        .timing-bar {
            flex: 1;
            height: 24px;
            background: #21262d;
            border-radius: 4px;
            position: relative;
            overflow: hidden;
        }
        
        .timing-fill {
            height: 100%;
            border-radius: 4px;
            transition: width 0.3s ease;
        }
        
        .timing-fill.green {
            background: linear-gradient(90deg, #00ff88, #00d9ff);
        }
        
        .timing-fill.yellow {
            background: linear-gradient(90deg, #ffd93d, #ff9f43);
        }
        
        .timing-fill.red {
            background: linear-gradient(90deg, #ff6b6b, #ee5a5a);
        }
        
        .timing-marker {
            position: absolute;
            top: 0;
            bottom: 0;
            width: 3px;
            background: #fff;
            box-shadow: 0 0 10px #fff;
        }
        
        .key-insight {
            background: linear-gradient(135deg, #00d9ff20, #ff6b6b20);
            border: 2px solid #00d9ff;
            border-radius: 12px;
            padding: 20px;
            margin-top: 20px;
        }
        
        .key-insight h3 {
            color: #00d9ff;
            margin-bottom: 12px;
        }
        
        .key-insight p {
            color: #ccc;
            line-height: 1.8;
        }
        
        @media (max-width: 768px) {
            .pros-cons {
                grid-template-columns: 1fr;
            }
            
            .flow {
                flex-direction: column;
            }
            
            .flow-step::after {
                display: none;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📈 EMA25Greater17PriceMatchStrategy 分析</h1>
        <p class="subtitle">EMA25 大于 EMA17 且收盘价与 EMA17 小数点后两位相等策略</p>
        
        <!-- 策略概述 -->
        <div class="card">
            <h2>策略概述</h2>
            <p style="color: #aaa; line-height: 1.8; margin-bottom: 20px;">
                这是一种<strong style="color: #00ff88;">均值回归 + 趋势确认</strong>的组合策略。它结合了：
            </p>
            <div class="flow">
                <div class="flow-step">
                    <div class="num">1</div>
                    <div class="title">趋势确认</div>
                    <div class="desc">EMA25 > EMA17</div>
                </div>
                <div class="flow-step">
                    <div class="num">2</div>
                    <div class="title">底部确认</div>
                    <div class="desc">连续N天 &lt; EMA17</div>
                </div>
                <div class="flow-step">
                    <div class="num">3</div>
                    <div class="title">触发信号</div>
                    <div class="desc">收盘价 = EMA17</div>
                </div>
            </div>
            
            <div class="signal-box">
                <h3>🎯 买入信号</h3>
                <p>当收盘价与 EMA17 小数点后两位相等时触发</p>
            </div>
        </div>
        
        <!-- 触发条件 -->
        <div class="card">
            <h2>触发条件详解</h2>
            
            <div class="condition ema">
                <h3>📊 条件1: EMA25 > EMA17（持续N天）</h3>
                <p>
                    最近的 <span class="highlight">below_days</span> 天内，EMA25 始终大于 EMA17。
                    这表明<strong>长期趋势向上</strong>，多头排列形成。
                </p>
            </div>
            
            <div class="condition price">
                <h3>📉 条件2: 收盘价 &lt; EMA17（持续N天，不含今天）</h3>
                <p>
                    前 <span class="highlight">below_days</span> 天的收盘价都低于 EMA17。
                    这表明股价<strong>之前处于弱势区域</strong>，正在逐渐走强。
                </p>
            </div>
            
            <div class="condition match">
                <h3>🎯 条件3: 收盘价 = EMA17（两位小数）</h3>
                <p>
                    当日收盘价<strong>四舍五入到两位小数</strong>后与 EMA17 相等。
                    这是<strong>关键的触发点</strong>，表明价格<strong>刚刚触及</strong> EMA17 支撑位。
                </p>
            </div>
        </div>
        
        <!-- 代码逻辑 -->
        <div class="card">
            <h2>核心代码逻辑</h2>
            <div class="formula">
<span class="comment">// 条件1: 最近belowDays天EMA25始终大于EMA17</span>
<span class="keyword">if</span> (!isSlowAboveFastForDays(fastEMA, slowEMA, belowDays)) {
    <span class="keyword">return</span> std::nullopt;
}

<span class="comment">// 条件2: 最近belowDays天收盘价都小于EMA17（不含今天）</span>
<span class="keyword">if</span> (!isCloseBelowFastForDays(closes, fastEMA, belowDays)) {
    <span class="keyword">return</span> std::nullopt;
}

<span class="comment">// 条件3: 收盘价与EMA17两位小数相等</span>
<span class="keyword">double</span> roundedClose = <span class="function">round</span>(closes.back() * 100) / 100;
<span class="keyword">double</span> roundedFast = <span class="function">round</span>(fastEMA.back() * 100) / 100;
<span class="keyword">if</span> (roundedClose != roundedFast) {
    <span class="keyword">return</span> std::nullopt;
}

<span class="comment">// 触发买入信号</span>
<span class="keyword">return</span> createResult(tsCode, tradeDate, <span class="string">"买入"</span>);
            </div>
        </div>
        
        <!-- 可视化图表 -->
        <div class="card">
            <h2>策略逻辑可视化</h2>
            <div class="chart-container">
                <canvas id="strategyChart"></canvas>
            </div>
            
            <div style="margin-top: 20px; padding: 15px; background: rgba(0,0,0,0.3); border-radius: 8px;">
                <p style="color: #aaa; font-size: 14px; line-height: 1.6;">
                    <strong style="color: #00d9ff;">📌 图例说明：</strong><br>
                    • <span style="color: #00ff88;">绿色线</span> = EMA25（长期均线）<br>
                    • <span style="color: #ff6b6b;">红色线</span> = EMA17（短期均线）<br>
                    • <span style="color: #ffd93d;">黄色区域</span> = 收盘价在 EMA17 下方运行<br>
                    • <span style="color: #00d9ff;">蓝色虚线</span> = 收盘价与 EMA17 相等的位置（触发点）
                </p>
            </div>
        </div>
        
        <!-- 时序图 -->
        <div class="card">
            <h2>信号触发时序</h2>
            <div class="timing-diagram">
                <div class="timing-row">
                    <div class="timing-label">Day 1</div>
                    <div class="timing-bar">
                        <div class="timing-fill red" style="width: 80%"></div>
                        <div class="timing-marker" style="left: 60%"></div>
                    </div>
                </div>
                <div class="timing-row">
                    <div class="timing-label">Day 2</div>
                    <div class="timing-bar">
                        <div class="timing-fill red" style="width: 75%"></div>
                        <div class="timing-marker" style="left: 65%"></div>
                    </div>
                </div>
                <div class="timing-row">
                    <div class="timing-label">Day 3</div>
                    <div class="timing-bar">
                        <div class="timing-fill yellow" style="width: 70%"></div>
                        <div class="timing-marker" style="left: 70%"></div>
                    </div>
                </div>
                <div class="timing-row">
                    <div class="timing-label">Today</div>
                    <div class="timing-bar">
                        <div class="timing-fill green" style="width: 65%; animation: pulse 1s infinite;"></div>
                        <div class="timing-marker" style="left: 65%; background: #00ff88; box-shadow: 0 0 15px #00ff88;"></div>
                    </div>
                </div>
            </div>
            <p style="text-align: center; color: #888; margin-top: 15px; font-size: 14px;">
                股价从下方逐渐上涨 → 触及 EMA17 → 触发买入信号
            </p>
        </div>
        
        <!-- 优缺点分析 -->
        <div class="card">
            <h2>策略优缺点分析</h2>
            <div class="pros-cons">
                <div class="pros">
                    <h3>✅ 优点</h3>
                    <ul>
                        <li>趋势确认 + 回调确认的组合，更可靠</li>
                        <li>可以在**上涨初期**识别买入机会</li>
                        <li>条件明确，信号不易漂移</li>
                        <li>均值回归策略，胜率相对较高</li>
                        <li>参数简单，易于理解和实现</li>
                    </ul>
                </div>
                <div class="cons">
                    <h3>⚠️ 缺点</h3>
                    <ul>
                        <li>条件较严格，信号可能较少</li>
                        <li>需要连续N天在 EMA17 下方运行</li>
                        <li>"收盘价相等"条件较苛刻，可能错过最佳买点</li>
                        <li>震荡行情中可能失效</li>
                        <li>需要足够的历史数据计算 EMA</li>
                    </ul>
                </div>
            </div>
        </div>
        
        <!-- 关键洞察 -->
        <div class="key-insight">
            <h3>💡 关键洞察：能否在涨之前分析出来？</h3>
            <p>
                <strong>答案：可以的！</strong>这个策略实际上是一个<strong style="color: #00ff88;">前瞻性指标</strong>：<br><br>
                
                1. <strong>提前识别趋势</strong>：EMA25 > EMA17 表明多头排列已经形成<br>
                2. <strong>发现蓄势</strong>：连续N天收盘价 &lt; EMA17 表明股价在底部蓄势<br>
                3. <strong>精准捕捉</strong>：当收盘价与 EMA17 相等时，正是<strong style="color: #00ff88;">刚刚突破</strong>的时刻<br><br>
                
                <strong>最佳买入时机</strong>：收盘价从下方刚刚触及 EMA17 的那一刻。这比等价格已经上涨几天后再买入要好得多。
            </p>
        </div>
        
        <!-- 参数说明 -->
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
                    <td style="padding: 12px;">below_days</td>
                    <td style="padding: 12px;">3</td>
                    <td style="padding: 12px;">收盘价在 EMA17 下方的天数</td>
                </tr>
            </table>
        </div>
        
        <!-- 改进建议 -->
        <div class="card">
            <h2>潜在改进方向</h2>
            <div class="grid">
                <div class="condition">
                    <h3>🔄 改为区间匹配</h3>
                    <p>
                        不仅限于"相等"，可以改为"收盘价在 EMA17 ± X% 范围内"，
                        放宽条件以捕获更多信号。
                    </p>
                </div>
                <div class="condition">
                    <h3>📊 增加成交量确认</h3>
                    <p>
                        加上成交量放大的条件，确认是真突破而非假突破。
                    </p>
                </div>
                <div class="condition">
                    <h3>⏰ 引入时间窗口</h3>
                    <p>
                        设定在 EMA17 下方运行的最长时间，超过则不再等待。
                    </p>
                </div>
                <div class="condition">
                    <h3>🎯 提前信号</h3>
                    <p>
                        当收盘价接近 EMA17（如差距 &lt; 0.5%）时，
                        可以提前给出"即将触发"的预警。
                    </p>
                </div>
            </div>
        </div>
    </div>
    
    <script>
        // 生成模拟数据
        const days = 30;
        const labels = [];
        const closeData = [];
        const ema17Data = [];
        const ema25Data = [];
        
        // 模拟股价走势：先下跌，然后在 EMA17 附近震荡，最后上涨
        let price = 100;
        for (let i = 0; i < days; i++) {
            labels.push(`Day ${i + 1}`);
            
            if (i < 20) {
                // 前20天：下跌趋势，在 EMA17 下方
                price = 100 - (20 - i) * 0.8 + Math.random() * 2;
            } else if (i < 25) {
                // 20-25天：逐渐上涨，接近 EMA17
                price = 84 + (i - 20) * 3.2 + Math.random() * 2;
            } else {
                // 最后5天：突破 EMA17
                price = 100 + (i - 25) * 0.5 + Math.random() * 1;
            }
            closeData.push(price);
        }
        
        // 计算 EMA（简化版）
        function calculateEMA(data, period) {
            const ema = [];
            const multiplier = 2 / (period + 1);
            
            // 初始化为简单移动平均
            let sum = 0;
            for (let i = 0; i < period; i++) {
                sum += data[i];
            }
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
        
        // 修正最后几天的 EMA17 值使其与收盘价"相等"（模拟触发条件）
        const lastClose = closeData[closeData.length - 1];
        const lastEma17 = ema17Data[ema17Data.length - 1];
        const rounded = Math.round(lastClose * 100) / 100;
        ema17Data[ema17Data.length - 1] = rounded;
        
        // 创建图表
        const ctx = document.getElementById('strategyChart').getContext('2d');
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
                        label: 'EMA17',
                        data: ema17Data,
                        borderColor: '#ff6b6b',
                        borderWidth: 2,
                        fill: false,
                        tension: 0.4,
                        pointRadius: 0
                    },
                    {
                        label: 'EMA25',
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
                interaction: {
                    intersect: false,
                    mode: 'index'
                },
                plugins: {
                    legend: {
                        labels: {
                            color: '#ccc',
                            font: { size: 12 }
                        }
                    },
                    tooltip: {
                        backgroundColor: 'rgba(0, 0, 0, 0.8)',
                        titleColor: '#00d9ff',
                        bodyColor: '#ccc',
                        borderColor: '#333',
                        borderWidth: 1
                    },
                    annotation: {
                        annotations: {
                            triggerLine: {
                                type: 'line',
                                xMin: days - 1,
                                xMax: days - 1,
                                borderColor: '#00d9ff',
                                borderWidth: 2,
                                borderDash: [5, 5],
                                label: {
                                    content: '触发点',
                                    enabled: true,
                                    color: '#00d9ff'
                                }
                            }
                        }
                    }
                },
                scales: {
                    x: {
                        grid: { color: 'rgba(255,255,255,0.1)' },
                        ticks: { color: '#888' }
                    },
                    y: {
                        grid: { color: 'rgba(255,255,255,0.1)' },
                        ticks: { color: '#888' }
                    }
                }
            }
        });
    </script>
</body>
</html>