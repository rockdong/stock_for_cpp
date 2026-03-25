export interface AnalysisProcessRecord {
  id: number
  ts_code: string
  stock_name: string
  strategy_name: string
  trade_date: string
  freq: 'd' | 'w' | 'm'
  signal: 'BUY' | 'SELL' | 'HOLD' | 'NONE'
  time: string
  open: number
  high: number
  low: number
  close: number
  volume: number
  ema17: number
  ema25: number
  macd: number
  rsi: number
  created_at: string
  expires_at: string
}

export interface ChartDataPoint {
  time: string
  open: number
  high: number
  low: number
  close: number
  volume: number
  ema17: number
  ema25: number
  macd: number
  rsi: number
  signal: string
}

export interface FilterParams {
  ts_code?: string
  strategy?: string
  start_date?: string
  end_date?: string
  signal?: string
}