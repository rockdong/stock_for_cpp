import { useState, useEffect } from 'react'
import { AnalysisProcessRecord, FreqType, ChartDataPoint } from '../../types/analysis'
import ModalHeader from './ModalHeader'
import CandlestickChart from '../Chart/CandlestickChart'
import IndicatorPanel from './IndicatorPanel'

interface DrillLevel {
  id: number
  sourceFreq: FreqType
  targetFreq: FreqType
  drillTime: string
}

interface ChartModalProps {
  visible: boolean
  record: AnalysisProcessRecord | null
  freq: FreqType
  strategy: string
  onClose: () => void
  onFreqChange: (freq: FreqType) => void
  onStrategyChange: (strategy: string) => void
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

export default function ChartModal({
  visible,
  record,
  freq,
  strategy,
  onClose,
  onFreqChange,
  onStrategyChange,
}: ChartModalProps) {
  const [drillStack, setDrillStack] = useState<DrillLevel[]>([])
  const [drillIdCounter, setDrillIdCounter] = useState(0)
  const [animating, setAnimating] = useState(false)

  useEffect(() => {
    if (visible) {
      setDrillStack([])
      setDrillIdCounter(0)
      setAnimating(true)
    } else {
      setAnimating(false)
    }
  }, [visible, freq, strategy])

  const getStrategies = (record: AnalysisProcessRecord): string[] => {
    return (record.data?.strategies || []).map(s => s.name)
  }

  const getCandles = (record: AnalysisProcessRecord, strategyName: string, freq: FreqType): ChartDataPoint[] => {
    const strategies = record.data?.strategies || []
    const strat = strategies.find(s => s.name === strategyName)
    if (!strat) return []
    const freqData = strat.freqs.find(f => f.freq === freq)
    return freqData?.candles || []
  }

  const getFilteredCandles = (
    record: AnalysisProcessRecord,
    strategyName: string,
    freq: FreqType,
    drillLevel: DrillLevel | null
  ): ChartDataPoint[] => {
    const allCandles = getCandles(record, strategyName, freq)

    if (!drillLevel) return allCandles

    const drillParts = parseTimeToParts(drillLevel.drillTime)
    if (!drillParts) return allCandles

    if (drillLevel.sourceFreq === 'm' && freq === 'w') {
      return allCandles.filter(candle => {
        const parts = parseTimeToParts(candle.time)
        if (!parts) return false
        return parts.year === drillParts.year && parts.month === drillParts.month
      })
    }

    if (drillLevel.sourceFreq === 'm' && freq === 'd') {
      return allCandles.filter(candle => {
        const parts = parseTimeToParts(candle.time)
        if (!parts) return false
        return parts.year === drillParts.year && parts.month === drillParts.month
      })
    }

    if (drillLevel.sourceFreq === 'w' && freq === 'd') {
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

  const handleDrillDown = (info: { time: string; targetFreq: FreqType }) => {
    const currentDrill = drillStack.length > 0 ? drillStack[drillStack.length - 1] : null
    const sourceFreq = currentDrill ? currentDrill.targetFreq : freq

    const newLevel: DrillLevel = {
      id: drillIdCounter,
      sourceFreq,
      targetFreq: info.targetFreq,
      drillTime: info.time.replace(/-/g, ''),
    }

    setDrillIdCounter(drillIdCounter + 1)
    setDrillStack([...drillStack, newLevel])
  }

  const handleRemoveDrillLevel = (id: number) => {
    setDrillStack(drillStack.filter(level => level.id !== id))
  }

  const handleClearDrillStack = () => {
    setDrillStack([])
  }

  if (!visible || !record) return null

  const strategies = getStrategies(record)
  const currentDrill = drillStack.length > 0 ? drillStack[drillStack.length - 1] : null
  const currentFreq = currentDrill ? currentDrill.targetFreq : freq
  const chartData = getFilteredCandles(record, strategy, currentFreq, currentDrill)

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-6">
      {/* 遮罩层 */}
      <div
        className="absolute inset-0 bg-black/70 backdrop-blur-sm animate-fade-in"
        onClick={onClose}
      />

      {/* 弹窗主体 */}
      <div className={`relative bg-elevated rounded-2xl shadow-modal w-full max-w-5xl max-h-[90vh] overflow-hidden flex flex-col ${animating ? 'animate-slide-up' : ''}`}>
        {/* 头部 */}
        <ModalHeader
          stockName={record.stock_name}
          tsCode={record.ts_code}
          tradeDate={record.trade_date}
          strategy={strategy}
          freq={currentDrill ? currentDrill.targetFreq : freq}
          onFreqChange={onFreqChange}
          onClose={onClose}
        />

        {/* 策略选择 */}
        {strategies.length > 1 && (
          <div className="px-6 py-2.5 border-b border-border-default bg-surface/50">
            <div className="flex items-center gap-2">
              <span className="text-xs text-text-tertiary">策略:</span>
              <div className="flex gap-1.5">
                {strategies.map(s => (
                  <button
                    key={s}
                    onClick={() => {
                      onStrategyChange(s)
                      setDrillStack([])
                    }}
                    className={`px-3 py-1.5 text-xs font-medium rounded-md transition-colors cursor-pointer ${
                      strategy === s
                        ? 'bg-accent-blue-bg text-accent-blue border border-accent-blue/20'
                        : 'bg-elevated text-text-secondary hover:text-text-primary border border-transparent'
                    }`}
                  >
                    {s}
                  </button>
                ))}
              </div>
            </div>
          </div>
        )}

        {/* 下钻路径 */}
        {drillStack.length > 0 && (
          <div className="px-6 py-2 border-b border-border-default bg-accent-blue-bg/30 flex items-center gap-2">
            <svg className="w-3.5 h-3.5 text-accent-blue flex-shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
              <path strokeLinecap="round" strokeLinejoin="round" d="M3.75 6A2.25 2.25 0 016 3.75h2.25A2.25 2.25 0 0110.5 6v2.25a2.25 2.25 0 01-2.25 2.25H6a2.25 2.25 0 01-2.25-2.25V6z" />
            </svg>
            <span className="text-xs text-text-tertiary">下钻:</span>
            {drillStack.map((level, index) => (
              <div key={level.id} className="flex items-center gap-1">
                <button
                  onClick={() => handleRemoveDrillLevel(level.id)}
                  className="px-2 py-0.5 text-xs bg-accent-blue-bg text-accent-blue rounded hover:bg-accent-blue/20 transition-colors cursor-pointer"
                >
                  {level.sourceFreq === 'm' ? '月K' : '周K'} → {level.targetFreq === 'w' ? '周K' : '日K'}
                </button>
                {index < drillStack.length - 1 && (
                  <svg className="w-3 h-3 text-text-tertiary" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                    <path strokeLinecap="round" strokeLinejoin="round" d="M8.25 4.5l7.5 7.5-7.5 7.5" />
                  </svg>
                )}
              </div>
            ))}
            <button
              onClick={handleClearDrillStack}
              className="ml-auto text-xs text-text-tertiary hover:text-text-secondary cursor-pointer"
            >
              清除
            </button>
          </div>
        )}

        {/* 图表区 */}
        <div className="flex-1 p-6 overflow-auto">
          {chartData.length > 0 ? (
            <div className="bg-surface rounded-xl p-4 border border-border-default">
              <CandlestickChart
                data={chartData}
                height={420}
                freq={currentFreq}
                onDrillDown={handleDrillDown}
              />
            </div>
          ) : (
            <div className="bg-surface rounded-xl p-12 text-center border border-border-default">
              <svg className="w-10 h-10 text-text-tertiary mx-auto mb-3" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M3 13.125C3 12.504 3.504 12 4.125 12h2.25c.621 0 1.125.504 1.125 1.125v6.75C7.5 20.496 6.996 21 6.375 21h-2.25A1.125 1.125 0 013 19.875v-6.75z" />
              </svg>
              <p className="text-text-secondary text-sm">暂无图表数据</p>
            </div>
          )}
        </div>

        {/* 指标面板 */}
        <IndicatorPanel data={chartData} />
      </div>
    </div>
  )
}
