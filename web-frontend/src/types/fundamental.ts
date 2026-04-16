export interface FinancialIndicator {
  ts_code: string
  ann_date: string
  end_date: string
  pe: number
  pe_ttm: number
  pb: number
  ps: number
  pcf: number
  roe: number
  roe_ttm: number
  roa: number
  grossprofit_margin: number
  netprofit_margin: number
  operating_margin: number
  or_yoy: number
  op_yoy: number
  ebt_yoy: number
  netprofit_yoy: number
  tr_yoy: number
  debt_to_assets: number
  assets_turn: number
  current_ratio: number
  ocf_to_operatingprofit: number
  ocf_to_or: number
}

export interface FundamentalScore {
  ts_code: string
  total: number
  valuation: number
  quality: number
  growth: number
  health: number
  grade: string
  is_qualified: boolean
  reason: string
}

export interface FundamentalFilterParams {
  pe_max?: number
  pb_max?: number
  roe_min?: number
  gross_margin_min?: number
  total_score_min?: number
}

export interface FundamentalResult {
  success: boolean
  message: string
  thresholds?: FundamentalFilterParams
  data?: FundamentalScore[]
}

export interface FundamentalProgress {
  total: number
  completed: number
  qualified: number
  status: 'idle' | 'running' | 'completed'
  started_at: string | null
  updated_at: string
}