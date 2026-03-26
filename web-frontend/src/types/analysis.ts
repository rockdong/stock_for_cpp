export type SignalType = 'BUY' | 'SELL' | 'HOLD' | 'NONE'
export type FreqType = 'd' | 'w' | 'm'
export type ProgressStatus = 'idle' | 'running' | 'completed'

export interface AnalysisProgress {
  id: number
  total: number
  completed: number
  failed: number
  status: ProgressStatus
  started_at: string | null
  updated_at: string
}

export interface DataPoint {
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
}

export interface AnalysisProcessRecord {
  id: number
  ts_code: string
  stock_name: string
  strategy_name: string
  trade_date: string
  freq: FreqType
  signal: SignalType
  data: DataPoint[]
  created_at: string
  expires_at: string
}

export interface ChartDataPoint extends DataPoint {
  signal?: string
}

export interface ChartResponse {
  data: ChartDataPoint[]
  record: {
    id: number
    ts_code: string
    stock_name: string
    strategy_name: string
    trade_date: string
    freq: FreqType
    signal: SignalType
  }
}

export interface FilterParams {
  ts_code?: string
  strategy?: string
  start_date?: string
  end_date?: string
  signal?: string
}