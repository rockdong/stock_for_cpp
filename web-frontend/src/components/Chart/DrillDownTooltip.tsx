import { FreqType, ChartDataPoint } from '../../types/analysis'

interface DrillDownTooltipProps {
  visible: boolean
  x: number
  y: number
  data: ChartDataPoint | null
  freq: FreqType
  onDrillDown: (targetFreq: FreqType) => void
  containerWidth: number
  onMouseEnter?: () => void
  onMouseLeave?: () => void
}

function formatNumber(num: number | undefined): string {
  if (num === undefined) return '-'
  return num.toFixed(2)
}

function formatVolume(vol: number | undefined): string {
  if (vol === undefined) return '-'
  if (vol >= 100000000) return (vol / 100000000).toFixed(2) + '亿'
  if (vol >= 10000) return (vol / 10000).toFixed(2) + '万'
  return vol.toString()
}

function getDrillOptions(freq: FreqType): { label: string; targetFreq: FreqType }[] {
  if (freq === 'm') {
    return [
      { label: '查看周K', targetFreq: 'w' },
      { label: '查看日K', targetFreq: 'd' },
    ]
  }
  if (freq === 'w') {
    return [
      { label: '查看日K', targetFreq: 'd' },
    ]
  }
  return []
}

export default function DrillDownTooltip({ 
  visible, 
  x, 
  y, 
  data, 
  freq, 
  onDrillDown, 
  containerWidth,
  onMouseEnter,
  onMouseLeave
}: DrillDownTooltipProps) {
  if (!visible || !data) return null
  
  const isUp = data.close >= data.open
  const changePercent = ((data.close - data.open) / data.open * 100).toFixed(2)
  const drillOptions = getDrillOptions(freq)
  
  return (
    <div 
      className="absolute pointer-events-auto z-50 bg-white/95 backdrop-blur-sm border border-gray-200 rounded-lg shadow-lg p-3 text-sm"
      style={{ 
        left: Math.min(x + 10, containerWidth - 180),
        top: Math.max(y - 120, 10),
      }}
      onMouseEnter={onMouseEnter}
      onMouseLeave={onMouseLeave}
    >
      <div className="font-medium text-gray-900 mb-2">{data.time}</div>
      <div className="grid grid-cols-2 gap-x-4 gap-y-1">
        <span className="text-gray-500">开:</span>
        <span className="font-medium">{formatNumber(data.open)}</span>
        <span className="text-gray-500">高:</span>
        <span className="font-medium text-red-500">{formatNumber(data.high)}</span>
        <span className="text-gray-500">低:</span>
        <span className="font-medium text-green-600">{formatNumber(data.low)}</span>
        <span className="text-gray-500">收:</span>
        <span className={`font-medium ${isUp ? 'text-red-500' : 'text-green-600'}`}>
          {formatNumber(data.close)}
        </span>
        <span className="text-gray-500">涨跌:</span>
        <span className={`font-medium ${isUp ? 'text-red-500' : 'text-green-600'}`}>
          {isUp ? '+' : ''}{changePercent}%
        </span>
      </div>
      <div className="border-t border-gray-100 mt-2 pt-2">
        <span className="text-gray-500">量:</span>
        <span className="ml-2">{formatVolume(data.volume)}</span>
      </div>
      {data.ema17 !== undefined && (
        <div className="text-blue-500 text-xs mt-1">
          EMA17: {formatNumber(data.ema17)}
        </div>
      )}
      {data.ema25 !== undefined && (
        <div className="text-orange-500 text-xs mt-1">
          EMA25: {formatNumber(data.ema25)}
        </div>
      )}
      {drillOptions.length > 0 && (
        <div className="border-t border-gray-100 mt-2 pt-2 flex gap-2">
          {drillOptions.map((option, index) => (
            <button
              key={index}
              onClick={() => onDrillDown(option.targetFreq)}
              className="px-2 py-1 text-xs bg-blue-100 text-blue-600 rounded hover:bg-blue-200 transition-colors cursor-pointer"
            >
              {option.label}
            </button>
          ))}
        </div>
      )}
    </div>
  )
}