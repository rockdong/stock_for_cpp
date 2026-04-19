import { ChartDataPoint } from '../../types/analysis'

interface IndicatorPanelProps {
  data: ChartDataPoint[]
}

function formatNumber(num: number | undefined): string {
  if (num === undefined) return '-'
  return num.toFixed(2)
}

function formatMacd(num: number | undefined): string {
  if (num === undefined) return '-'
  return num.toFixed(4)
}

export default function IndicatorPanel({ data }: IndicatorPanelProps) {
  const lastData = data[data.length - 1]

  if (!lastData) return null

  return (
    <div className="grid grid-cols-4 gap-3 px-6 py-4 border-t border-border-default bg-surface/50">
      <IndicatorItem
        label="MACD"
        value={formatMacd(lastData.macd)}
        color={lastData.macd !== undefined && lastData.macd >= 0 ? 'text-signal-buy' : 'text-signal-sell'}
      />
      <IndicatorItem
        label="RSI"
        value={formatNumber(lastData.rsi)}
        color={
          lastData.rsi !== undefined
            ? lastData.rsi > 70 ? 'text-signal-buy' : lastData.rsi < 30 ? 'text-signal-sell' : 'text-text-primary'
            : 'text-text-primary'
        }
      />
      <IndicatorItem
        label="EMA17"
        value={formatNumber(lastData.ema17)}
        color="text-accent-blue"
      />
      <IndicatorItem
        label="EMA25"
        value={formatNumber(lastData.ema25)}
        color="text-accent-amber"
      />
    </div>
  )
}

function IndicatorItem({ label, value, color }: { label: string; value: string; color: string }) {
  return (
    <div className="bg-surface rounded-lg px-3 py-2.5 border border-border-default">
      <div className="text-2xs text-text-tertiary mb-1">{label}</div>
      <div className={`text-sm font-semibold font-mono tabular-nums ${color}`}>{value}</div>
    </div>
  )
}
