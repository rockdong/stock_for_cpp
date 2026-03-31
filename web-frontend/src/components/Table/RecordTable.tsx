import { useState } from 'react'
import { AnalysisProcessRecord, StrategyData, ChartDataPoint, FreqType } from '../../types/analysis'
import CandlestickChart, { DrillDownInfo } from '../Chart/CandlestickChart'

interface DrillState {
  active: boolean
  originalFreq: FreqType
  drillFreq: FreqType
  drillTime: string
  drillLabel: string
}

interface RecordTableProps {
  records: AnalysisProcessRecord[]
}

function parseTimeToParts(time: string): { year: number; month: number; day: number } | null {
  const cleanTime = time.replace(/-/g, '')
  if (cleanTime.length === 8) {
    return {
      year: parseInt(cleanTime.slice(0, 4)),
      month: parseInt(cleanTime.slice(4, 6)),
      day: parseInt(cleanTime.slice(6, 8)),
    }
  }
  return null
}

function getWeekRange(date: Date): { start: Date; end: Date } {
  const day = date.getDay()
  const diff = date.getDate() - day + (day === 0 ? -6 : 1)
  const start = new Date(date)
  start.setDate(diff)
  const end = new Date(start)
  end.setDate(start.getDate() + 6)
  return { start, end }
}

export default function RecordTable({ records }: RecordTableProps) {
  const [expandedId, setExpandedId] = useState<number | null>(null)
  const [selectedStrategy, setSelectedStrategy] = useState<string>('')
  const [selectedFreq, setSelectedFreq] = useState<FreqType>('d')
  const [showJson, setShowJson] = useState(false)
  const [drillState, setDrillState] = useState<DrillState | null>(null)

  const getStrategies = (record: AnalysisProcessRecord): StrategyData[] => {
    return record.data?.strategies || []
  }

  const getSignalSummary = (record: AnalysisProcessRecord): { buy: number; sell: number } => {
    const strategies = getStrategies(record)
    let buy = 0, sell = 0
    strategies.forEach(s => {
      s.freqs.forEach(f => {
        if (f.signal === 'BUY') buy++
        if (f.signal === 'SELL') sell++
      })
    })
    return { buy, sell }
  }

  const getLastClose = (record: AnalysisProcessRecord): number | null => {
    const strategies = getStrategies(record)
    if (strategies.length === 0) return null
    const firstStrategy = strategies[0]
    const dailyFreq = firstStrategy.freqs.find(f => f.freq === 'd')
    if (!dailyFreq || dailyFreq.candles.length === 0) return null
    return dailyFreq.candles[dailyFreq.candles.length - 1]?.close
  }

  const getCandles = (record: AnalysisProcessRecord, strategyName: string, freq: FreqType): ChartDataPoint[] => {
    const strategies = getStrategies(record)
    const strategy = strategies.find(s => s.name === strategyName)
    if (!strategy) return []
    const freqData = strategy.freqs.find(f => f.freq === freq)
    return freqData?.candles || []
  }

  const getFilteredCandles = (
    record: AnalysisProcessRecord,
    strategyName: string,
    freq: FreqType,
    drill: DrillState | null
  ): ChartDataPoint[] => {
    const allCandles = getCandles(record, strategyName, freq)
    
    if (!drill || !drill.active) {
      return allCandles
    }

    const drillParts = parseTimeToParts(drill.drillTime)
    if (!drillParts) return allCandles

    if (drill.originalFreq === 'm' && freq === 'w') {
      return allCandles.filter(candle => {
        const parts = parseTimeToParts(candle.time)
        if (!parts) return false
        return parts.year === drillParts.year && parts.month === drillParts.month
      })
    }
    
    if (drill.originalFreq === 'm' && freq === 'd') {
      return allCandles.filter(candle => {
        const parts = parseTimeToParts(candle.time)
        if (!parts) return false
        return parts.year === drillParts.year && parts.month === drillParts.month
      })
    }
    
    if (drill.originalFreq === 'w' && freq === 'd') {
      const drillDate = new Date(drillParts.year, drillParts.month - 1, drillParts.day)
      const { start, end } = getWeekRange(drillDate)
      
      return allCandles.filter(candle => {
        const parts = parseTimeToParts(candle.time)
        if (!parts) return false
        const candleDate = new Date(parts.year, parts.month - 1, parts.day)
        return candleDate >= start && candleDate <= end
      })
    }

    return allCandles
  }

  const handleDrillDown = (info: DrillDownInfo) => {
    const targetFreq = info.targetFreq
    
    const label = info.targetFreq === 'w' 
      ? `${info.time.slice(0, 4)}年${info.time.slice(5, 7)}月 - 周K线`
      : `${info.time.slice(0, 4)}年${info.time.slice(5, 7)}月 - 日K线`
    
    setDrillState({
      active: true,
      originalFreq: selectedFreq,
      drillFreq: targetFreq,
      drillTime: info.time.replace(/-/g, ''),
      drillLabel: label,
    })
    setSelectedFreq(targetFreq)
  }

  const handleResetDrill = () => {
    if (drillState) {
      setSelectedFreq(drillState.originalFreq)
    }
    setDrillState(null)
  }

  const getSignalText = (record: AnalysisProcessRecord): string => {
    const strategies = getStrategies(record)
    const signals: string[] = []
    strategies.forEach(s => {
      s.freqs.forEach(f => {
        if (f.signal === 'BUY' || f.signal === 'SELL') {
          signals.push(`${s.name}(${f.freq}): ${f.signal}`)
        }
      })
    })
    return signals.length > 0 ? signals.slice(0, 3).join(', ') : 'NONE'
  }

  const handleToggle = (record: AnalysisProcessRecord) => {
    if (expandedId === record.id) {
      setExpandedId(null)
      setDrillState(null)
      return
    }
    
    setExpandedId(record.id)
    setShowJson(false)
    setDrillState(null)
    
    const strategies = getStrategies(record)
    if (strategies.length > 0) {
      setSelectedStrategy(strategies[0].name)
      setSelectedFreq('d')
    }
  }

  if (records.length === 0) {
    return (
      <div className="bg-secondary rounded-2xl p-12 text-center border border-border">
        <div className="w-16 h-16 mx-auto mb-4 rounded-2xl bg-primary flex items-center justify-center">
          <svg className="w-8 h-8 text-muted" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={1.5} d="M9 17v-2m3 2v-4m3 4v-6m2 10H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
          </svg>
        </div>
        <p className="text-muted">暂无分析记录</p>
      </div>
    )
  }

  return (
    <div className="space-y-3">
      {records.map(record => {
        const { buy, sell } = getSignalSummary(record)
        const isExpanded = expandedId === record.id
        const strategies = getStrategies(record)
        const chartData = isExpanded && selectedStrategy 
          ? getFilteredCandles(record, selectedStrategy, selectedFreq, drillState)
          : []

        return (
          <div key={record.id} className="bg-secondary rounded-2xl border border-border overflow-hidden">
            <div 
              className="px-5 py-4 flex items-center cursor-pointer hover:bg-primary/30 transition-colors"
              onClick={() => handleToggle(record)}
            >
              <div className="flex-1 grid grid-cols-6 gap-4 items-center">
                <div className="text-sm text-text font-medium">{record.trade_date}</div>
                <div className="col-span-2">
                  <div className="text-sm font-medium text-text">{record.stock_name}</div>
                  <div className="text-xs text-muted">{record.ts_code}</div>
                </div>
                <div className="text-sm text-muted">{strategies.length}</div>
                <div className="flex gap-2">
                  {buy > 0 && (
                    <span className="px-2 py-0.5 text-xs rounded bg-cta/20 text-cta">{buy} 买入</span>
                  )}
                  {sell > 0 && (
                    <span className="px-2 py-0.5 text-xs rounded bg-sell/20 text-sell">{sell} 卖出</span>
                  )}
                  {buy === 0 && sell === 0 && (
                    <span className="text-xs text-muted">无信号</span>
                  )}
                </div>
                <div className="text-sm text-text font-mono">
                  {getLastClose(record)?.toFixed(2) || '-'}
                </div>
              </div>
              <button className="ml-4 px-3 py-1.5 text-xs font-medium text-cta bg-cta/10 rounded-lg hover:bg-cta/20 transition-colors cursor-pointer">
                {isExpanded ? '收起' : '查看图表'}
              </button>
            </div>

            {isExpanded && (
              <div className="px-5 pb-5 border-t border-border">
                <div className="pt-4 flex items-center justify-between">
                  <div className="flex items-center gap-3">
                    <div className="w-10 h-10 rounded-xl bg-primary flex items-center justify-center border border-border">
                      <svg className="w-5 h-5 text-cta" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z" />
                      </svg>
                    </div>
                    <div>
                      <div className="text-sm font-semibold text-text">{record.stock_name}</div>
                      <div className="text-xs text-muted">{record.ts_code} · {getSignalText(record)}</div>
                    </div>
                  </div>
                  <div className="flex gap-3">
                    <select
                      value={selectedStrategy}
                      onChange={(e) => setSelectedStrategy(e.target.value)}
                      className="px-3 py-2 rounded-lg bg-primary border border-border text-text text-sm cursor-pointer"
                    >
                      {strategies.map(s => (
                        <option key={s.name} value={s.name}>{s.name}</option>
                      ))}
                    </select>
                    <select
                      value={selectedFreq}
                      onChange={(e) => setSelectedFreq(e.target.value as FreqType)}
                      className="px-3 py-2 rounded-lg bg-primary border border-border text-text text-sm cursor-pointer"
                    >
                      <option value="d">日线</option>
                      <option value="w">周线</option>
                      <option value="m">月线</option>
                    </select>
                  </div>
                </div>

                {chartData.length > 0 ? (
                  <div className="mt-4 bg-primary rounded-xl p-4 border border-border">
                    <div className="flex gap-4 mb-4 text-sm">
                      <div className="flex items-center gap-2">
                        <span className="w-3 h-3 rounded bg-cta"></span>
                        <span className="text-muted">K线 (涨)</span>
                      </div>
                      <div className="flex items-center gap-2">
                        <span className="w-3 h-3 rounded bg-sell"></span>
                        <span className="text-muted">K线 (跌)</span>
                      </div>
                      <div className="flex items-center gap-2">
                        <span className="w-3 h-0.5 bg-blue-500 rounded"></span>
                        <span className="text-muted">EMA17</span>
                      </div>
                      <div className="flex items-center gap-2">
                        <span className="w-3 h-0.5 bg-orange-500 rounded"></span>
                        <span className="text-muted">EMA25</span>
                      </div>
                    </div>
                    
                    {drillState?.active && (
                      <div className="flex items-center gap-2 mb-3 px-3 py-2 bg-gray-100 rounded-lg">
                        <button
                          onClick={handleResetDrill}
                          className="flex items-center gap-1 text-sm text-gray-600 hover:text-gray-900 cursor-pointer"
                        >
                          <svg className="w-4 h-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 19l-7-7 7-7" />
                          </svg>
                          返回
                        </button>
                        <span className="text-gray-400">›</span>
                        <span className="text-sm text-gray-600">
                          {drillState.originalFreq === 'm' ? '月K线' : '周K线'}
                        </span>
                        <span className="text-gray-400">›</span>
                        <span className="text-sm font-medium text-gray-900">{drillState.drillLabel}</span>
                        <span className="text-gray-400">›</span>
                        <span className="text-sm font-medium text-cta">
                          {drillState.drillFreq === 'w' ? '周K线' : '日K线'}
                        </span>
                      </div>
                    )}
                    
                    <CandlestickChart 
                      data={chartData} 
                      height={350} 
                      freq={selectedFreq}
                      onDrillDown={handleDrillDown}
                    />

                    <div className="grid grid-cols-4 gap-4 mt-4 pt-4 border-t border-border">
                      <div className="text-center p-3 rounded-xl bg-secondary">
                        <div className="text-xs text-muted mb-1">MACD</div>
                        <div className="text-base font-semibold text-text">
                          {chartData[chartData.length - 1]?.macd?.toFixed(4) || '-'}
                        </div>
                      </div>
                      <div className="text-center p-3 rounded-xl bg-secondary">
                        <div className="text-xs text-muted mb-1">RSI</div>
                        <div className="text-base font-semibold text-text">
                          {chartData[chartData.length - 1]?.rsi?.toFixed(2) || '-'}
                        </div>
                      </div>
                      <div className="text-center p-3 rounded-xl bg-secondary">
                        <div className="text-xs text-muted mb-1">EMA17</div>
                        <div className="text-base font-semibold text-text">
                          {chartData[chartData.length - 1]?.ema17?.toFixed(2) || '-'}
                        </div>
                      </div>
                      <div className="text-center p-3 rounded-xl bg-secondary">
                        <div className="text-xs text-muted mb-1">EMA25</div>
                        <div className="text-base font-semibold text-text">
                          {chartData[chartData.length - 1]?.ema25?.toFixed(2) || '-'}
                        </div>
                      </div>
                    </div>

                    <div className="mt-4 pt-4 border-t border-border">
                      <button
                        onClick={() => setShowJson(!showJson)}
                        className="flex items-center gap-2 text-sm text-muted hover:text-text transition-colors cursor-pointer"
                      >
                        <svg className="w-4 h-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d={showJson ? "M19 9l-7 7-7-7" : "M9 5l7 7-7 7"} />
                        </svg>
                        <span>{showJson ? '隐藏 JSON 数据' : '显示 JSON 数据'}</span>
                      </button>
                      
                      {showJson && (
                        <div className="mt-3 p-4 rounded-xl bg-secondary overflow-auto max-h-96">
                          <pre className="text-xs text-text font-mono whitespace-pre-wrap break-all">
                            {JSON.stringify(record, null, 2)}
                          </pre>
                        </div>
                      )}
                    </div>
                  </div>
                ) : (
                  <div className="mt-4 bg-primary rounded-xl p-8 text-center border border-border">
                    <p className="text-muted">暂无图表数据</p>
                    <button
                      onClick={() => setShowJson(!showJson)}
                      className="mt-3 text-sm text-cta hover:underline cursor-pointer"
                    >
                      {showJson ? '隐藏' : '显示'} JSON 数据
                    </button>
                    {showJson && (
                      <div className="mt-4 p-4 rounded-xl bg-secondary overflow-auto max-h-96">
                        <pre className="text-xs text-text font-mono whitespace-pre-wrap break-all">
                          {JSON.stringify(record, null, 2)}
                        </pre>
                      </div>
                    )}
                  </div>
                )}
              </div>
            )}
          </div>
        )
      })}
    </div>
  )
}