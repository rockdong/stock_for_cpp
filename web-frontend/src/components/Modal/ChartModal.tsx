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

  useEffect(() => {
    if (visible) {
      setDrillStack([])
      setDrillIdCounter(0)
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
    <div className="fixed inset-0 z-50 bg-black/50 backdrop-blur-sm flex items-center justify-center p-8">
      <div className="bg-white rounded-2xl shadow-2xl w-full max-w-4xl max-h-[90vh] overflow-hidden">
        <ModalHeader
          stockName={record.stock_name}
          tsCode={record.ts_code}
          tradeDate={record.trade_date}
          strategy={strategy}
          freq={currentDrill ? currentDrill.targetFreq : freq}
          onFreqChange={onFreqChange}
          onClose={onClose}
        />
        
        {strategies.length > 1 && (
          <div className="px-6 py-2 border-b border-gray-200 bg-gray-50">
            <select
              value={strategy}
              onChange={(e) => {
                onStrategyChange(e.target.value)
                setDrillStack([])
              }}
              className="px-3 py-2 rounded-lg bg-white border border-gray-200 text-gray-700 text-sm cursor-pointer"
            >
              {strategies.map(s => (
                <option key={s} value={s}>{s}</option>
              ))}
            </select>
          </div>
        )}
        
        {drillStack.length > 0 && (
          <div className="px-6 py-2 border-b border-gray-200 bg-blue-50 flex items-center gap-2">
            <span className="text-sm text-gray-500">下钻路径:</span>
            {drillStack.map((level, index) => (
              <div key={level.id} className="flex items-center gap-1">
                <button
                  onClick={() => handleRemoveDrillLevel(level.id)}
                  className="px-2 py-1 text-xs bg-blue-100 text-blue-600 rounded hover:bg-blue-200 cursor-pointer"
                >
                  {level.sourceFreq === 'm' ? '月K' : '周K'} → {level.targetFreq === 'w' ? '周K' : '日K'}
                </button>
                {index < drillStack.length - 1 && <span className="text-gray-400">→</span>}
              </div>
            ))}
            <button
              onClick={handleClearDrillStack}
              className="ml-auto text-xs text-gray-500 hover:text-gray-700 cursor-pointer"
            >
              清除
            </button>
          </div>
        )}
        
        <div className="p-6">
          {chartData.length > 0 ? (
            <div className="bg-gray-50 rounded-xl p-4">
              <CandlestickChart
                data={chartData}
                height={400}
                freq={currentFreq}
                onDrillDown={handleDrillDown}
              />
            </div>
          ) : (
            <div className="bg-gray-50 rounded-xl p-8 text-center">
              <p className="text-gray-500">暂无图表数据</p>
            </div>
          )}
        </div>
        
        <IndicatorPanel data={chartData} />
      </div>
    </div>
  )
}