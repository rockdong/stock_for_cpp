import { useState, useEffect, useRef } from 'react'
import { createChart, ColorType, IChartApi, CandlestickData, LineData } from 'lightweight-charts'
import { ChartDataPoint, FreqType } from '../../types/analysis'
import DrillDownTooltip from './DrillDownTooltip'

function formatTime(time: string): string {
  if (time.length === 8 && /^\d{8}$/.test(time)) {
    return `${time.slice(0, 4)}-${time.slice(4, 6)}-${time.slice(6, 8)}`
  }
  return time
}

interface CandlestickChartProps {
  data: ChartDataPoint[]
  height?: number
  freq?: FreqType
  onDrillDown?: (info: { time: string; targetFreq: FreqType }) => void
}

export default function CandlestickChart({ data, height = 400, freq = 'd', onDrillDown }: CandlestickChartProps) {
  const chartContainerRef = useRef<HTMLDivElement>(null)
  const chartRef = useRef<IChartApi | null>(null)
  const [tooltip, setTooltip] = useState<{
    visible: boolean
    x: number
    y: number
    data: ChartDataPoint | null
  }>({ visible: false, x: 0, y: 0, data: null })
  const [tooltipLocked, setTooltipLocked] = useState(false)
  const hideTimeoutRef = useRef<number | null>(null)

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
      if (tooltipLocked) return
      
      if (!param.time || !param.point) {
        if (hideTimeoutRef.current) {
          clearTimeout(hideTimeoutRef.current)
        }
        hideTimeoutRef.current = setTimeout(() => {
          setTooltip({ visible: false, x: 0, y: 0, data: null })
        }, 100)
        return
      }
      
      const timeStr = param.time.toString()
      const pointData = data.find(d => formatTime(d.time) === timeStr)
      
      if (pointData && param.point) {
        if (hideTimeoutRef.current) {
          clearTimeout(hideTimeoutRef.current)
          hideTimeoutRef.current = null
        }
        setTooltip({
          visible: true,
          x: param.point.x,
          y: param.point.y,
          data: pointData,
        })
      }
    })

    const ema17Series = chart.addLineSeries({
      color: '#2196F3',
      lineWidth: 2,
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
      if (hideTimeoutRef.current) {
        clearTimeout(hideTimeoutRef.current)
      }
      chartRef.current = null
      try {
        chart.remove()
      } catch {}
    }
  }, [data, height, tooltipLocked])

  const handleTooltipMouseEnter = () => {
    setTooltipLocked(true)
    if (hideTimeoutRef.current) {
      clearTimeout(hideTimeoutRef.current)
      hideTimeoutRef.current = null
    }
  }

  const handleTooltipMouseLeave = () => {
    setTooltipLocked(false)
    setTooltip({ visible: false, x: 0, y: 0, data: null })
  }

  const handleDrillDown = (targetFreq: FreqType) => {
    if (tooltip.data && onDrillDown) {
      onDrillDown({
        time: tooltip.data.time,
        targetFreq,
      })
      setTooltip({ visible: false, x: 0, y: 0, data: null })
      setTooltipLocked(false)
    }
  }

  return (
    <div className="relative">
      <div 
        ref={chartContainerRef} 
        className="w-full rounded-lg border border-gray-200 bg-white"
      />
      <DrillDownTooltip 
        visible={tooltip.visible}
        x={tooltip.x}
        y={tooltip.y}
        data={tooltip.data}
        freq={freq}
        onDrillDown={handleDrillDown}
        containerWidth={chartContainerRef.current?.clientWidth || 600}
        onMouseEnter={handleTooltipMouseEnter}
        onMouseLeave={handleTooltipMouseLeave}
      />
    </div>
  )
}