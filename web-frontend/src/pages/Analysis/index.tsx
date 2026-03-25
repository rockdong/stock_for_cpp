import { useState, useEffect } from 'react'
import AnalysisFilter from '../../components/Filter/AnalysisFilter'
import RecordTable from '../../components/Table/RecordTable'
import CandlestickChart from '../../components/Chart/CandlestickChart'
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
    setChartLoading(true)
    try {
      const data = await analysisApi.getChartData(
        record.ts_code,
        record.strategy_name,
        record.freq
      )
      setChartData(data)
    } catch (error) {
      console.error('加载图表数据失败:', error)
    } finally {
      setChartLoading(false)
    }
  }

  return (
    <div className="min-h-screen bg-gray-50">
      <header className="bg-white shadow">
        <div className="max-w-7xl mx-auto px-4 py-4 sm:px-6 lg:px-8">
          <h1 className="text-2xl font-bold text-gray-900">分析过程数据展示</h1>
          <p className="text-sm text-gray-500 mt-1">查看 C++ 分析引擎的过程数据和结果</p>
        </div>
      </header>

      <main className="max-w-7xl mx-auto px-4 py-6 sm:px-6 lg:px-8">
        <AnalysisFilter onFilter={loadRecords} />

        {selectedRecord && (
          <div className="mb-6">
            <div className="bg-white rounded-lg shadow p-4 mb-4">
              <div className="flex justify-between items-center">
                <div>
                  <h2 className="text-lg font-semibold">
                    {selectedRecord.stock_name} ({selectedRecord.ts_code})
                  </h2>
                  <p className="text-sm text-gray-500">
                    策略: {selectedRecord.strategy_name} | 
                    周期: {selectedRecord.freq === 'd' ? '日线' : selectedRecord.freq === 'w' ? '周线' : '月线'} |
                    日期: {selectedRecord.trade_date}
                  </p>
                </div>
                <button
                  onClick={() => {
                    setSelectedRecord(null)
                    setChartData([])
                  }}
                  className="text-gray-500 hover:text-gray-700"
                >
                  关闭
                </button>
              </div>
            </div>

            {chartLoading ? (
              <div className="bg-white rounded-lg shadow p-8 text-center">
                加载图表中...
              </div>
            ) : chartData.length > 0 ? (
              <div className="bg-white rounded-lg shadow p-4">
                <div className="flex gap-4 mb-4 text-sm">
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-3 bg-green-500 rounded"></span>
                    <span>K线 (涨)</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-3 bg-red-500 rounded"></span>
                    <span>K线 (跌)</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-1 bg-blue-500 rounded"></span>
                    <span>EMA17</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="w-3 h-1 bg-orange-500 rounded"></span>
                    <span>EMA25</span>
                  </div>
                </div>
                <CandlestickChart data={chartData} height={400} />

                <div className="grid grid-cols-4 gap-4 mt-4 pt-4 border-t">
                  <div className="text-center">
                    <div className="text-xs text-gray-500">MACD</div>
                    <div className="text-lg font-medium">
                      {chartData[chartData.length - 1]?.macd?.toFixed(4) || '-'}
                    </div>
                  </div>
                  <div className="text-center">
                    <div className="text-xs text-gray-500">RSI</div>
                    <div className="text-lg font-medium">
                      {chartData[chartData.length - 1]?.rsi?.toFixed(2) || '-'}
                    </div>
                  </div>
                  <div className="text-center">
                    <div className="text-xs text-gray-500">EMA17</div>
                    <div className="text-lg font-medium">
                      {chartData[chartData.length - 1]?.ema17?.toFixed(2) || '-'}
                    </div>
                  </div>
                  <div className="text-center">
                    <div className="text-xs text-gray-500">EMA25</div>
                    <div className="text-lg font-medium">
                      {chartData[chartData.length - 1]?.ema25?.toFixed(2) || '-'}
                    </div>
                  </div>
                </div>
              </div>
            ) : (
              <div className="bg-white rounded-lg shadow p-8 text-center text-gray-500">
                暂无图表数据
              </div>
            )}
          </div>
        )}

        {loading ? (
          <div className="bg-white rounded-lg shadow p-8 text-center">
            加载中...
          </div>
        ) : (
          <RecordTable records={records} onSelect={handleSelectRecord} />
        )}
      </main>
    </div>
  )
}