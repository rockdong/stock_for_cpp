import { useState, useEffect } from 'react'
import AnalysisFilter from '../../components/Filter/AnalysisFilter'
import RecordTable from '../../components/Table/RecordTable'
import CandlestickChart from '../../components/Chart/CandlestickChart'
import ProgressBadge from '../../components/Progress/ProgressBadge'
import { analysisApi } from '../../services/api'
import { AnalysisProcessRecord, ChartDataPoint, FilterParams } from '../../types/analysis'

export default function AnalysisPage() {
  const [records, setRecords] = useState<AnalysisProcessRecord[]>([])
  const [selectedRecord, setSelectedRecord] = useState<AnalysisProcessRecord | null>(null)
  const [chartData, setChartData] = useState<ChartDataPoint[]>([])
  const [loading, setLoading] = useState(false)
  const [chartLoading, setChartLoading] = useState(false)

  useEffect(() => {
    loadRecords({})
  }, [])

  const loadRecords = async (params: FilterParams) => {
    setLoading(true)
    try {
      const data = await analysisApi.getProcessRecords(params)
      setRecords(data)
    } catch (error) {
      console.error('加载记录失败:', error)
    } finally {
      setLoading(false)
    }
  }

  const handleSelectRecord = async (record: AnalysisProcessRecord) => {
    setSelectedRecord(record)
    
    if (record.data && record.data.length > 0) {
      setChartData(record.data)
      return
    }
    
    setChartLoading(true)
    try {
      const response = await analysisApi.getChartData(
        record.ts_code,
        record.strategy_name,
        record.freq
      )
      setChartData(response.data || [])
    } catch (error) {
      console.error('加载图表数据失败:', error)
    } finally {
      setChartLoading(false)
    }
  }

  return (
    <div className="min-h-screen bg-background">
      <header className="sticky top-0 z-50 bg-secondary/80 backdrop-blur-xl border-b border-border">
        <div className="max-w-7xl mx-auto px-4 py-4 sm:px-6 lg:px-8">
          <div className="flex justify-between items-center">
            <div className="flex items-center gap-3">
              <div className="w-10 h-10 rounded-xl bg-gradient-to-br from-cta to-emerald-600 flex items-center justify-center">
                <svg className="w-6 h-6 text-white" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 7h8m0 0v8m0-8l-8 8-4-4-6 6" />
                </svg>
              </div>
              <div>
                <h1 className="text-xl font-semibold text-text">Stock Analysis</h1>
                <p className="text-xs text-muted">股票分析过程数据展示</p>
              </div>
            </div>
            <ProgressBadge />
          </div>
        </div>
      </header>

      <main className="max-w-7xl mx-auto px-4 py-6 sm:px-6 lg:px-8">
        <AnalysisFilter onFilter={loadRecords} />

        {selectedRecord && (
          <div className="mb-6">
            <div className="bg-secondary rounded-2xl p-5 mb-4 border border-border">
              <div className="flex justify-between items-start">
                <div className="flex items-center gap-4">
                  <div className="w-12 h-12 rounded-xl bg-primary flex items-center justify-center border border-border">
                    <svg className="w-6 h-6 text-cta" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z" />
                    </svg>
                  </div>
                  <div>
                    <h2 className="text-lg font-semibold text-text">
                      {selectedRecord.stock_name}
                    </h2>
                    <p className="text-sm text-muted">
                      {selectedRecord.ts_code} · {selectedRecord.strategy_name} · {selectedRecord.freq === 'd' ? '日线' : selectedRecord.freq === 'w' ? '周线' : '月线'}
                    </p>
                  </div>
                </div>
                <button
                  onClick={() => {
                    setSelectedRecord(null)
                    setChartData([])
                  }}
                  className="p-2 rounded-lg hover:bg-primary transition-colors cursor-pointer"
                >
                  <svg className="w-5 h-5 text-muted" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                  </svg>
                </button>
              </div>
            </div>

            {chartLoading ? (
              <div className="bg-secondary rounded-2xl p-8 text-center border border-border">
                <div className="animate-pulse flex flex-col items-center gap-3">
                  <div className="w-12 h-12 rounded-full bg-primary"></div>
                  <p className="text-muted">加载图表中...</p>
                </div>
              </div>
            ) : chartData.length > 0 ? (
              <div className="bg-secondary rounded-2xl p-5 border border-border">
                <div className="flex gap-4 mb-4 text-sm">
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-3 rounded bg-cta"></span>
                    <span className="text-muted">K线 (涨)</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-3 rounded bg-sell"></span>
                    <span className="text-muted">K线 (跌)</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-0.5 bg-blue-500 rounded"></span>
                    <span className="text-muted">EMA17</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-0.5 bg-orange-500 rounded"></span>
                    <span className="text-muted">EMA25</span>
                  </div>
                </div>
                <CandlestickChart data={chartData} height={400} />

                <div className="grid grid-cols-4 gap-4 mt-5 pt-5 border-t border-border">
                  <div className="text-center p-3 rounded-xl bg-primary">
                    <div className="text-xs text-muted mb-1">MACD</div>
                    <div className="text-lg font-semibold text-text">
                      {chartData[chartData.length - 1]?.macd?.toFixed(4) || '-'}
                    </div>
                  </div>
                  <div className="text-center p-3 rounded-xl bg-primary">
                    <div className="text-xs text-muted mb-1">RSI</div>
                    <div className="text-lg font-semibold text-text">
                      {chartData[chartData.length - 1]?.rsi?.toFixed(2) || '-'}
                    </div>
                  </div>
                  <div className="text-center p-3 rounded-xl bg-primary">
                    <div className="text-xs text-muted mb-1">EMA17</div>
                    <div className="text-lg font-semibold text-text">
                      {chartData[chartData.length - 1]?.ema17?.toFixed(2) || '-'}
                    </div>
                  </div>
                  <div className="text-center p-3 rounded-xl bg-primary">
                    <div className="text-xs text-muted mb-1">EMA25</div>
                    <div className="text-lg font-semibold text-text">
                      {chartData[chartData.length - 1]?.ema25?.toFixed(2) || '-'}
                    </div>
                  </div>
                </div>
              </div>
            ) : (
              <div className="bg-secondary rounded-2xl p-8 text-center border border-border">
                <p className="text-muted">暂无图表数据</p>
              </div>
            )}
          </div>
        )}

        {loading ? (
          <div className="bg-secondary rounded-2xl p-8 text-center border border-border">
            <div className="animate-pulse flex flex-col items-center gap-3">
              <div className="w-12 h-12 rounded-full bg-primary"></div>
              <p className="text-muted">加载中...</p>
            </div>
          </div>
        ) : (
          <RecordTable records={records} onSelect={handleSelectRecord} />
        )}
      </main>
    </div>
  )
}