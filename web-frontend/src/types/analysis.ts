export type SignalType = 'BUY' | 'SELL' | 'HOLD' | 'NONE'
export type FreqType = 'd' | 'w' | 'm'
export type ProgressStatus = 'idle' | 'running' | 'completed'

export interface PhaseProgress {
  status: ProgressStatus
  total: number
  completed: number
  qualified?: number
  failed?: number
}

export interface AnalysisProgress {
  id: number
  phase1: PhaseProgress
  phase2: PhaseProgress
  started_at: string | null
  phase1_completed_at: string | null
  updated_at: string
}

export interface DataPoint {
  time: string
  open: number
  high: number
  low: number
  close: number
  volume: number
  ema17?: number
  ema25?: number
  macd?: number
  macd_signal?: number
  macd_hist?: number
  rsi?: number
  boll_upper?: number
  boll_lower?: number
}

export interface StrategyFreqData {
  freq: FreqType
  signal: SignalType
  candles: DataPoint[]
}

export interface StrategyData {
  name: string
  freqs: StrategyFreqData[]
}

export interface AnalysisProcessRecord {
  id: number
  ts_code: string
  stock_name: string
  trade_date: string
  data: {
    strategies: StrategyData[]
  }
  created_at: string
  expires_at: string | null
}

export interface ChartDataPoint extends DataPoint {
  signal?: string
}

export interface ChartResponse {
  data: ChartDataPoint[]
  record?: {
    id: number
    ts_code: string
    stock_name: string
    trade_date: string
  }
}

export interface FilterParams {
  ts_code?: string
  start_date?: string
  end_date?: string
}