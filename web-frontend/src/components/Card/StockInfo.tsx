import { AnalysisProcessRecord } from '../../types/analysis'

interface StockInfoProps {
  record: AnalysisProcessRecord
}

function formatPrice(price: number | undefined): string {
  if (price === undefined) return '-'
  return price.toFixed(2)
}

function formatChange(close: number, open: number): { value: string; isUp: boolean } {
  const change = ((close - open) / open) * 100
  return {
    value: (change >= 0 ? '+' : '') + change.toFixed(2) + '%',
    isUp: change >= 0
  }
}

function getSignalSummary(record: AnalysisProcessRecord): { buy: string[]; sell: string[] } {
  const strategies = record.data?.strategies || []
  const buy: string[] = []
  const sell: string[] = []
  
  strategies.forEach(s => {
    s.freqs.forEach(f => {
      if (f.signal === 'BUY') {
        buy.push(`${s.name}(${f.freq === 'd' ? '日K' : f.freq === 'w' ? '周K' : '月K'})`)
      }
      if (f.signal === 'SELL') {
        sell.push(`${s.name}(${f.freq === 'd' ? '日K' : f.freq === 'w' ? '周K' : '月K'})`)
      }
    })
  })
  
  return { buy, sell }
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
    low: lastCandle.low
  }
}

export default function StockInfo({ record }: StockInfoProps) {
  const prices = getLastPrice(record)
  const { buy, sell } = getSignalSummary(record)
  const change = prices ? formatChange(prices.close, prices.open) : { value: '-', isUp: false }
  
  return (
    <div className="space-y-3">
      <div className="flex justify-between items-start">
        <div>
          <div className="text-lg font-semibold text-gray-900">{record.stock_name}</div>
          <div className="text-sm text-gray-500">{record.ts_code}</div>
        </div>
        <div className="text-sm text-gray-400">
          {record.trade_date}
        </div>
      </div>
      
      {prices && (
        <div className="grid grid-cols-2 gap-2 text-sm">
          <div className="flex justify-between">
            <span className="text-gray-500">收盘:</span>
            <span className={`font-medium ${change.isUp ? 'text-red-500' : 'text-green-600'}`}>
              {formatPrice(prices.close)}
            </span>
          </div>
          <div className="flex justify-between">
            <span className="text-gray-500">最高:</span>
            <span className="font-medium text-red-500">{formatPrice(prices.high)}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-gray-500">最低:</span>
            <span className="font-medium text-green-600">{formatPrice(prices.low)}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-gray-500">涨跌:</span>
            <span className={`font-medium ${change.isUp ? 'text-red-500' : 'text-green-600'}`}>
              {change.value}
            </span>
          </div>
        </div>
      )}
      
      {(buy.length > 0 || sell.length > 0) && (
        <div className="flex flex-wrap gap-1">
          {buy.slice(0, 3).map((s, i) => (
            <span key={i} className="px-2 py-0.5 text-xs rounded bg-red-100 text-red-600">
              🔴 {s}买入
            </span>
          ))}
          {sell.slice(0, 3).map((s, i) => (
            <span key={i} className="px-2 py-0.5 text-xs rounded bg-green-100 text-green-600">
              🟢 {s}卖出
            </span>
          ))}
        </div>
      )}
      
      {buy.length === 0 && sell.length === 0 && (
        <div className="text-xs text-gray-400">
          无信号
        </div>
      )}
    </div>
  )
}