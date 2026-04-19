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

export default function CandlestickChart({ data, height = 420, freq = 'd', onDrillDown }: CandlestickChartProps) {
  const chartContainerRef = useRef<HTMLDivElement>(null)
  const chartRef = useRef<IChartApi | null>(null)
  const isLockedRef = useRef(false)
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
        background: { type: ColorType.Solid, color: '#141820' },
        textColor: '#8B95A5',
        fontSize: 12,
      },
      width: chartContainerRef.current.clientWidth,
      height: height,
      grid: {
        vertLines: { color: '#1E2536' },
        horzLines: { color: '#1E2536' },
      },
      crosshair: {
        mode: 0,
        vertLine: {
          color: '#3B82F6',
          width: 1,
          style: 2,
          labelBackgroundColor: '#3B82F6',
        },
        horzLine: {
          color: '#3B82F6',
          width: 1,
          style: 2,
          labelBackgroundColor: '#3B82F6',
        },
      },
      rightPriceScale: {
        borderColor: '#2A3142',
        textColor: '#8B95A5',
      },
      timeScale: {
        borderColor: '#2A3142',
        timeVisible: false,
      },
    })

    chartRef.current = chart

    const candlestickSeries = chart.addCandlestickSeries({
      upColor: '#EF4444',
      downColor: '#22C55E',
      borderUpColor: '#EF4444',
      borderDownColor: '#22C55E',
      wickUpColor: '#EF4444',
      wickDownColor: '#22C55E',
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
      if (isLockedRef.current) return

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
      }
    })

    chart.subscribeClick((param) => {
      if (!param.time || !param.point) {
        isLockedRef.current = false
        setTooltip({ visible: false, x: 0, y: 0, data: null })
        return
      }

      const timeStr = param.time.toString()
      const pointData = data.find(d => formatTime(d.time) === timeStr)

      if (pointData && param.point) {
        isLockedRef.current = true
        setTooltip({
          visible: true,
          x: param.point.x,
          y: param.point.y,
          data: pointData,
        })
      }
    })

    const ema17Series = chart.addLineSeries({
      color: '#3B82F6',
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
      color: '#F59E0B',
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
      chartRef.current = null
      try {
        chart.remove()
      } catch {}
    }
  }, [data, height])

  const handleDrillDown = (targetFreq: FreqType) => {
    if (tooltip.data && onDrillDown) {
      onDrillDown({
        time: tooltip.data.time,
        targetFreq,
      })
      isLockedRef.current = false
      setTooltip({ visible: false, x: 0, y: 0, data: null })
    }
  }

  return (
    <div className="relative">
      <div
        ref={chartContainerRef}
        className="w-full rounded-lg border border-border-default bg-surface"
      />
      <DrillDownTooltip
        visible={tooltip.visible}
        x={tooltip.x}
        y={tooltip.y}
        data={tooltip.data}
        freq={freq}
        showDrillButtons={true}
        onDrillDown={handleDrillDown}
        containerWidth={chartContainerRef.current?.clientWidth || 600}
      />
    </div>
  )
}
