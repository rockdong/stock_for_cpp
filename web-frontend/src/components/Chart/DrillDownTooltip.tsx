import { FreqType, ChartDataPoint } from '../../types/analysis'

interface DrillDownTooltipProps {
  visible: boolean
  x: number
  y: number
  data: ChartDataPoint | null
  freq: FreqType
  showDrillButtons: boolean
  onDrillDown?: (targetFreq: FreqType) => void
  containerWidth: number
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
  showDrillButtons,
  onDrillDown,
  containerWidth
}: DrillDownTooltipProps) {
  if (!visible || !data) return null

  const isUp = data.close >= data.open
  const changePercent = ((data.close - data.open) / data.open * 100).toFixed(2)
  const drillOptions = getDrillOptions(freq)

  const tooltipWidth = 180
  const shouldPlaceLeft = x + tooltipWidth + 20 > containerWidth
  const leftPosition = shouldPlaceLeft ? Math.max(x - tooltipWidth - 10, 10) : Math.min(x + 10, containerWidth - tooltipWidth)

  return (
    <div
      className="absolute pointer-events-none z-50 bg-elevated/95 backdrop-blur-sm border border-border-default rounded-lg shadow-modal p-3 text-sm animate-fade-in"
      style={{
        left: leftPosition,
        top: Math.max(y - 120, 10),
      }}
    >
      <div className="font-medium text-text-primary font-mono text-xs">{data.time}</div>
      <div className="grid grid-cols-2 gap-x-4 gap-y-1 mt-2 text-xs">
        <span className="text-text-tertiary">开</span>
        <span className="font-medium text-text-primary font-mono tabular-nums">{formatNumber(data.open)}</span>
        <span className="text-text-tertiary">高</span>
        <span className="font-medium text-signal-buy font-mono tabular-nums">{formatNumber(data.high)}</span>
        <span className="text-text-tertiary">低</span>
        <span className="font-medium text-signal-sell font-mono tabular-nums">{formatNumber(data.low)}</span>
        <span className="text-text-tertiary">收</span>
        <span className={`font-medium font-mono tabular-nums ${isUp ? 'text-signal-buy' : 'text-signal-sell'}`}>
          {formatNumber(data.close)}
        </span>
        <span className="text-text-tertiary">涨跌</span>
        <span className={`font-medium font-mono tabular-nums ${isUp ? 'text-signal-buy' : 'text-signal-sell'}`}>
          {isUp ? '+' : ''}{changePercent}%
        </span>
      </div>
      <div className="border-t border-border-default mt-2 pt-2 text-xs">
        <span className="text-text-tertiary">量</span>
        <span className="ml-2 text-text-primary font-mono tabular-nums">{formatVolume(data.volume)}</span>
      </div>
      {data.ema17 !== undefined && (
        <div className="text-accent-blue text-xs mt-1 font-mono tabular-nums">
          EMA17: {formatNumber(data.ema17)}
        </div>
      )}
      {data.ema25 !== undefined && (
        <div className="text-accent-amber text-xs mt-1 font-mono tabular-nums">
          EMA25: {formatNumber(data.ema25)}
        </div>
      )}
      {showDrillButtons && drillOptions.length > 0 && onDrillDown && (
        <div className="border-t border-border-default mt-2 pt-2 flex gap-2 pointer-events-auto">
          {drillOptions.map((option, index) => (
            <button
              key={index}
              onClick={() => onDrillDown(option.targetFreq)}
              className="px-2.5 py-1 text-xs bg-accent-blue-bg text-accent-blue rounded-md hover:bg-accent-blue/20 border border-accent-blue/20 transition-colors cursor-pointer font-medium"
            >
              {option.label}
            </button>
          ))}
        </div>
      )}
    </div>
  )
}
