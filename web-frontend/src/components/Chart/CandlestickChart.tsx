import { useEffect, useRef, useState } from 'react'
import { createChart, ColorType, IChartApi, CandlestickData, LineData } from 'lightweight-charts'
import { ChartDataPoint } from '../../types/analysis'

interface CandlestickChartProps {
  data: ChartDataPoint[]
  height?: number
}

function formatTime(time: string): string {
  if (time.length === 8 && /^\d{8}$/.test(time)) {
    return `${time.slice(0, 4)}-${time.slice(4, 6)}-${time.slice(6, 8)}`
  }
  return time
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

export default function CandlestickChart({ data, height = 400 }: CandlestickChartProps) {
  const chartContainerRef = useRef<HTMLDivElement>(null)
  const chartRef = useRef<IChartApi | null>(null)
  const [tooltip, setTooltip] = useState<{
    visible: boolean
    x: number
    y: number
    data: ChartDataPoint | null
  }>({ visible: false, x: 0, y: 0, data: null })

  useEffect(() => {
    if (!chartContainerRef.current || data.length === 0) return

    const chart = createChart(chartContainerRef.current, {
      layout: {
        background: { type: ColorType.Solid, color: '#ffffff' },
        textColor: '#333',
      },
      width: chartContainerRef.current.clientWidth,
      height: height,
      grid: {
        vertLines: { color: '#e1e1e1' },
        horzLines: { color: '#e1e1e1' },
      },
    })

    chartRef.current = chart

    const candlestickSeries = chart.addCandlestickSeries({
      upColor: '#EF5350',
      downColor: '#26A69A',
      borderUpColor: '#EF5350',
      borderDownColor: '#26A69A',
      wickUpColor: '#EF5350',
      wickDownColor: '#26A69A',
    })

    const candleData: CandlestickData[] = data.map(d => ({
      time: formatTime(d.time) as any,
      open: d.open,
      high: d.high,
      low: d.low,
      close: d.close,
    }))
    candlestickSeries.setData(candleData)

    chart.subscribeCrosshairMove((param) => {
      if (!param.time || !param.point) {
        setTooltip({ visible: false, x: 0, y: 0, data: null })
        return
      }
      
      const timeStr = param.time.toString()
      const pointData = data.find(d => formatTime(d.time) === timeStr)
      
      if (pointData && param.point) {
        setTooltip({
          visible: true,
          x: param.point.x,
          y: param.point.y,
          data: pointData,
        })
      } else {
        setTooltip({ visible: false, x: 0, y: 0, data: null })
      }
    })

    const ema17Series = chart.addLineSeries({
      color: '#2196F3',
      lineWidth: 2,
      title: 'EMA17',
    })
    const ema17Data: LineData[] = data
      .filter(d => d.ema17 !== undefined)
      .map(d => ({
        time: formatTime(d.time) as any,
        value: d.ema17!,
      }))
    ema17Series.setData(ema17Data)

    const ema25Series = chart.addLineSeries({
      color: '#FF9800',
      lineWidth: 2,
      title: 'EMA25',
    })
    const ema25Data: LineData[] = data
      .filter(d => d.ema25 !== undefined)
      .map(d => ({
        time: formatTime(d.time) as any,
        value: d.ema25!,
      }))
    ema25Series.setData(ema25Data)

    chart.timeScale().fitContent()

    const handleResize = () => {
      if (chartContainerRef.current && chartRef.current) {
        try {
          chartRef.current.applyOptions({
            width: chartContainerRef.current.clientWidth,
          })
        } catch {}
      }
    }

    window.addEventListener('resize', handleResize)
    return () => {
      window.removeEventListener('resize', handleResize)
      chartRef.current = null
      try {
        chart.remove()
      } catch {}
    }
  }, [data, height])

  const isUp = tooltip.data && tooltip.data.close >= tooltip.data.open

  return (
    <div className="relative">
      <div 
        ref={chartContainerRef} 
        className="w-full rounded-lg border border-gray-200 bg-white"
      />
      {tooltip.visible && tooltip.data && (
        <div 
          className="absolute pointer-events-none z-50 bg-white/95 backdrop-blur-sm border border-gray-200 rounded-lg shadow-lg p-3 text-sm"
          style={{ 
            left: Math.min(tooltip.x + 10, (chartContainerRef.current?.clientWidth || 400) - 160),
            top: Math.max(tooltip.y - 100, 10),
          }}
        >
          <div className="font-medium text-gray-900 mb-2">{tooltip.data.time}</div>
          <div className="grid grid-cols-2 gap-x-4 gap-y-1">
            <span className="text-gray-500">开:</span>
            <span className="font-medium">{formatNumber(tooltip.data.open)}</span>
            <span className="text-gray-500">高:</span>
            <span className="font-medium text-red-500">{formatNumber(tooltip.data.high)}</span>
            <span className="text-gray-500">低:</span>
            <span className="font-medium text-green-600">{formatNumber(tooltip.data.low)}</span>
            <span className="text-gray-500">收:</span>
            <span className={`font-medium ${isUp ? 'text-red-500' : 'text-green-600'}`}>
              {formatNumber(tooltip.data.close)}
            </span>
          </div>
          <div className="border-t border-gray-100 mt-2 pt-2">
            <span className="text-gray-500">量:</span>
            <span className="ml-2">{formatVolume(tooltip.data.volume)}</span>
          </div>
          {tooltip.data.ema17 !== undefined && (
            <div className="text-blue-500 text-xs mt-1">
              EMA17: {formatNumber(tooltip.data.ema17)}
            </div>
          )}
          {tooltip.data.ema25 !== undefined && (
            <div className="text-orange-500 text-xs mt-1">
              EMA25: {formatNumber(tooltip.data.ema25)}
            </div>
          )}
          {tooltip.data.signal && (
            <div className={`mt-2 pt-2 border-t border-gray-100 text-xs font-medium ${
              tooltip.data.signal === 'BUY' ? 'text-red-500' : 
              tooltip.data.signal === 'SELL' ? 'text-green-600' : 'text-gray-500'
            }`}>
              {tooltip.data.signal === 'BUY' ? '🔴 买入信号' : 
               tooltip.data.signal === 'SELL' ? '🟢 卖出信号' : tooltip.data.signal}
            </div>
          )}
        </div>
      )}
    </div>
  )
}