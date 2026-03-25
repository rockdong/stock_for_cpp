import { useEffect, useRef } from 'react'
import { createChart, ColorType, IChartApi, CandlestickData, LineData } from 'lightweight-charts'
import { ChartDataPoint } from '../../types/analysis'

interface CandlestickChartProps {
  data: ChartDataPoint[]
  height?: number
}

export default function CandlestickChart({ data, height = 400 }: CandlestickChartProps) {
  const chartContainerRef = useRef<HTMLDivElement>(null)
  const chartRef = useRef<IChartApi | null>(null)

  useEffect(() => {
    if (!chartContainerRef.current || data.length === 0) return

    if (chartRef.current) {
      chartRef.current.remove()
    }

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
      upColor: '#07C160',
      downColor: '#FA5151',
      borderUpColor: '#07C160',
      borderDownColor: '#FA5151',
      wickUpColor: '#07C160',
      wickDownColor: '#FA5151',
    })

    const candleData: CandlestickData[] = data.map(d => ({
      time: d.time as any,
      open: d.open,
      high: d.high,
      low: d.low,
      close: d.close,
    }))
    candlestickSeries.setData(candleData)

    const ema17Series = chart.addLineSeries({
      color: '#2196F3',
      lineWidth: 2,
      title: 'EMA17',
    })
    const ema17Data: LineData[] = data.map(d => ({
      time: d.time as any,
      value: d.ema17,
    }))
    ema17Series.setData(ema17Data)

    const ema25Series = chart.addLineSeries({
      color: '#FF9800',
      lineWidth: 2,
      title: 'EMA25',
    })
    const ema25Data: LineData[] = data.map(d => ({
      time: d.time as any,
      value: d.ema25,
    }))
    ema25Series.setData(ema25Data)

    chart.timeScale().fitContent()

    const handleResize = () => {
      if (chartContainerRef.current && chartRef.current) {
        chartRef.current.applyOptions({
          width: chartContainerRef.current.clientWidth,
        })
      }
    }

    window.addEventListener('resize', handleResize)
    return () => {
      window.removeEventListener('resize', handleResize)
      chart.remove()
    }
  }, [data, height])

  return (
    <div 
      ref={chartContainerRef} 
      className="w-full rounded-lg border border-gray-200 bg-white"
    />
  )
}