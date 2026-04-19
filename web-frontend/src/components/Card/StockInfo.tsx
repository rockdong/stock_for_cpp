import { AnalysisProcessRecord } from '../../types/analysis'

interface StockInfoProps {
  record: AnalysisProcessRecord
}

function formatPrice(price: number | undefined): string {
  if (price === undefined) return '-'
  return price.toFixed(2)
}

function formatChange(close: number, open: number): { value: string; isUp: boolean; percent: number } {
  const change = ((close - open) / open) * 100
  return {
    value: (change >= 0 ? '+' : '') + change.toFixed(2) + '%',
    isUp: change >= 0,
    percent: change,
  }
}

function getSignalSummary(record: AnalysisProcessRecord): { buy: string[]; sell: string[]; hold: string[] } {
  const strategies = record.data?.strategies || []
  const buy: string[] = []
  const sell: string[] = []
  const hold: string[] = []

  strategies.forEach(s => {
    s.freqs.forEach(f => {
      const label = `${s.name}(${f.freq === 'd' ? '日K' : f.freq === 'w' ? '周K' : '月K'})`
      if (f.signal === 'BUY') buy.push(label)
      else if (f.signal === 'SELL') sell.push(label)
      else hold.push(label)
    })
  })

  return { buy, sell, hold }
}

function getLastPrice(record: AnalysisProcessRecord): { close: number; open: number; high: number; low: number } | null {
  const strategies = record.data?.strategies || []
  if (strategies.length === 0) return null

  const dailyFreq = strategies[0].freqs.find(f => f.freq === 'd')
  if (!dailyFreq || dailyFreq.candles.length === 0) return null

  const lastCandle = dailyFreq.candles[dailyFreq.candles.length - 1]
  return {
    close: lastCandle.close,
    open: lastCandle.open,
    high: lastCandle.high,
    low: lastCandle.low,
  }
}

/* 判断信号强度：买入>卖出>持有 */
function getSignalStrength(record: AnalysisProcessRecord): 'strong-buy' | 'buy' | 'sell' | 'strong-sell' | 'neutral' {
  const { buy, sell } = getSignalSummary(record)
  if (buy.length >= 3) return 'strong-buy'
  if (buy.length > 0) return 'buy'
  if (sell.length >= 3) return 'strong-sell'
  if (sell.length > 0) return 'sell'
  return 'neutral'
}

export default function StockInfo({ record }: StockInfoProps) {
  const prices = getLastPrice(record)
  const { buy, sell } = getSignalSummary(record)
  const change = prices ? formatChange(prices.close, prices.open) : { value: '-', isUp: false, percent: 0 }
  const strength = getSignalStrength(record)

  /* 信号强度对应的左侧竖条颜色 */
  const strengthBorderMap: Record<string, string> = {
    'strong-buy': 'border-l-signal-buy',
    'buy': 'border-l-signal-buy/60',
    'strong-sell': 'border-l-signal-sell',
    'sell': 'border-l-signal-sell/60',
    'neutral': 'border-l-border-default',
  }

  return (
    <div className="space-y-3">
      {/* 股票名称 + 信号强度指示条 */}
      <div className={`border-l-[3px] ${strengthBorderMap[strength]} pl-3`}>
        <div className="flex items-center gap-2">
          <span className="text-base font-semibold text-text-primary">{record.stock_name}</span>
          <span className="text-xs text-text-tertiary font-mono">{record.ts_code}</span>
        </div>
      </div>

      {/* 价格信息 */}
      {prices && (
        <div className="grid grid-cols-2 gap-x-4 gap-y-1.5 text-sm">
          <div className="flex justify-between">
            <span className="text-text-tertiary">收盘</span>
            <span className={`font-medium font-mono tabular-nums ${change.isUp ? 'text-signal-buy' : 'text-signal-sell'}`}>
              {formatPrice(prices.close)}
            </span>
          </div>
          <div className="flex justify-between">
            <span className="text-text-tertiary">涨跌</span>
            <span className={`font-medium font-mono tabular-nums ${change.isUp ? 'text-signal-buy' : 'text-signal-sell'}`}>
              {change.value}
            </span>
          </div>
          <div className="flex justify-between">
            <span className="text-text-tertiary">最高</span>
            <span className="font-medium font-mono tabular-nums text-signal-buy">
              {formatPrice(prices.high)}
            </span>
          </div>
          <div className="flex justify-between">
            <span className="text-text-tertiary">最低</span>
            <span className="font-medium font-mono tabular-nums text-signal-sell">
              {formatPrice(prices.low)}
            </span>
          </div>
        </div>
      )}

      {/* 信号标签 */}
      <div className="flex flex-wrap gap-1.5 pt-1">
        {buy.slice(0, 3).map((s, i) => (
          <span key={i} className="signal-badge--buy">
            {s} 买入
          </span>
        ))}
        {sell.slice(0, 3).map((s, i) => (
          <span key={i} className="signal-badge--sell">
            {s} 卖出
          </span>
        ))}
        {buy.length === 0 && sell.length === 0 && (
          <span className="text-xs text-text-tertiary py-1">暂无信号</span>
        )}
      </div>
    </div>
  )
}
