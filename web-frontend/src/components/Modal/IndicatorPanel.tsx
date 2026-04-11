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
    <div className="grid grid-cols-4 gap-4 px-6 py-4 border-t border-gray-200 bg-gray-50">
      <div className="text-center">
        <div className="text-xs text-gray-500 mb-1">MACD</div>
        <div className="text-base font-semibold text-gray-900">
          {formatMacd(lastData.macd)}
        </div>
      </div>
      <div className="text-center">
        <div className="text-xs text-gray-500 mb-1">RSI</div>
        <div className="text-base font-semibold text-gray-900">
          {formatNumber(lastData.rsi)}
        </div>
      </div>
      <div className="text-center">
        <div className="text-xs text-gray-500 mb-1">EMA17</div>
        <div className="text-base font-semibold text-blue-500">
          {formatNumber(lastData.ema17)}
        </div>
      </div>
      <div className="text-center">
        <div className="text-xs text-gray-500 mb-1">EMA25</div>
        <div className="text-base font-semibold text-orange-500">
          {formatNumber(lastData.ema25)}
        </div>
      </div>
    </div>
  )
}