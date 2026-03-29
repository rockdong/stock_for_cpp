import { useState, useEffect } from 'react'
import AnalysisFilter from '../../components/Filter/AnalysisFilter'
import RecordTable from '../../components/Table/RecordTable'
import ProgressBadge from '../../components/Progress/ProgressBadge'
import { analysisApi } from '../../services/api'
import { FilterParams, AnalysisProcessRecord } from '../../types/analysis'

export default function AnalysisPage() {
  const [records, setRecords] = useState<AnalysisProcessRecord[]>([])
  const [loading, setLoading] = useState(false)

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

        {loading ? (
          <div className="bg-secondary rounded-2xl p-8 text-center border border-border">
            <div className="animate-pulse flex flex-col items-center gap-3">
              <div className="w-12 h-12 rounded-full bg-primary"></div>
              <p className="text-muted">加载中...</p>
            </div>
          </div>
        ) : (
          <RecordTable records={records} />
        )}
      </main>
    </div>
  )
}