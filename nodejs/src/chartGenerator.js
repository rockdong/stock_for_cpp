const { createCanvas } = require('canvas');
const path = require('path');
const fs = require('fs');

const CHART_WIDTH = 800;
const CHART_HEIGHT = 500;
const PADDING = { top: 40, right: 60, bottom: 40, left: 80 };

function generateChart(chartData) {
  if (!chartData || !chartData.candles || chartData.candles.length === 0) {
    throw new Error('无图表数据');
  }

  const candles = chartData.candles;
  const canvas = createCanvas(CHART_WIDTH, CHART_HEIGHT);
  const ctx = canvas.getContext('2d');

  ctx.fillStyle = '#1e222d';
  ctx.fillRect(0, 0, CHART_WIDTH, CHART_HEIGHT);

  const chartArea = {
    x: PADDING.left,
    y: PADDING.top,
    width: CHART_WIDTH - PADDING.left - PADDING.right,
    height: CHART_HEIGHT - PADDING.top - PADDING.bottom
  };

  const prices = candles.flatMap(c => [c.high, c.low]);
  const minPrice = Math.min(...prices);
  const maxPrice = Math.max(...prices);
  const priceRange = maxPrice - minPrice || 1;

  const priceToY = (price) => {
    return chartArea.y + chartArea.height - ((price - minPrice) / priceRange) * chartArea.height;
  };

  ctx.strokeStyle = '#363a45';
  ctx.lineWidth = 1;
  for (let i = 0; i <= 4; i++) {
    const y = chartArea.y + (chartArea.height / 4) * i;
    ctx.beginPath();
    ctx.moveTo(chartArea.x, y);
    ctx.lineTo(chartArea.x + chartArea.width, y);
    ctx.stroke();

    const price = maxPrice - (priceRange / 4) * i;
    ctx.fillStyle = '#848e9c';
    ctx.font = '11px Arial';
    ctx.textAlign = 'right';
    ctx.fillText(price.toFixed(2), chartArea.x - 8, y + 4);
  }

  const candleWidth = Math.max(8, Math.min(40, (chartArea.width / candles.length) * 0.7));
  const candleGap = (chartArea.width - candleWidth * candles.length) / (candles.length + 1);

  candles.forEach((candle, i) => {
    const x = chartArea.x + candleGap + (candleWidth + candleGap) * i;
    const isUp = candle.close >= candle.open;

    ctx.strokeStyle = isUp ? '#26a69a' : '#ef5350';
    ctx.fillStyle = isUp ? '#26a69a' : '#ef5350';

    const highY = priceToY(candle.high);
    const lowY = priceToY(candle.low);
    const openY = priceToY(candle.open);
    const closeY = priceToY(candle.close);

    ctx.beginPath();
    ctx.moveTo(x + candleWidth / 2, highY);
    ctx.lineTo(x + candleWidth / 2, lowY);
    ctx.stroke();

    const bodyTop = Math.min(openY, closeY);
    const bodyHeight = Math.abs(closeY - openY) || 1;
    ctx.fillRect(x, bodyTop, candleWidth, bodyHeight);
  });

  const drawLine = (values, color, lineWidth = 1.5) => {
    ctx.strokeStyle = color;
    ctx.lineWidth = lineWidth;
    ctx.beginPath();
    let started = false;
    values.forEach((value, i) => {
      if (value === null || value === undefined || value === 0) return;
      const x = chartArea.x + candleGap + (candleWidth + candleGap) * i + candleWidth / 2;
      const y = priceToY(value);
      if (!started) {
        ctx.moveTo(x, y);
        started = true;
      } else {
        ctx.lineTo(x, y);
      }
    });
    ctx.stroke();
  };

  const ema17Values = candles.map(c => c.ema17);
  const ema25Values = candles.map(c => c.ema25);
  drawLine(ema17Values, '#2962ff', 2);
  drawLine(ema25Values, '#ff6d00', 2);

  ctx.fillStyle = '#ffffff';
  ctx.font = 'bold 14px Arial';
  ctx.textAlign = 'left';
  const title = `${chartData.ts_code} - ${chartData.freq === 'd' ? '日线' : chartData.freq === 'w' ? '周线' : '月线'}`;
  ctx.fillText(title, chartArea.x, 20);

  ctx.font = '11px Arial';
  const legendY = 20;
  ctx.fillStyle = '#2962ff';
  ctx.fillRect(chartArea.x + chartArea.width - 150, legendY - 8, 10, 10);
  ctx.fillStyle = '#ffffff';
  ctx.fillText('EMA17', chartArea.x + chartArea.width - 135, legendY);

  ctx.fillStyle = '#ff6d00';
  ctx.fillRect(chartArea.x + chartArea.width - 80, legendY - 8, 10, 10);
  ctx.fillStyle = '#ffffff';
  ctx.fillText('EMA25', chartArea.x + chartArea.width - 65, legendY);

  return canvas.toBuffer('image/png');
}

function generateChartToFile(chartData, outputPath) {
  const buffer = generateChart(chartData);
  fs.writeFileSync(outputPath, buffer);
  return outputPath;
}

module.exports = {
  generateChart,
  generateChartToFile
};