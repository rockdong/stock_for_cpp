import { useState } from 'react'
import FundamentalFilter from '../../components/Filter/FundamentalFilter'
import FundamentalCardGrid from '../../components/Card/FundamentalCardGrid'
import { fundamentalApi } from '../../services/fundamentalApi'
import { FundamentalFilterParams, FundamentalScore } from '../../types/fundamental'

export default function FundamentalPage() {
  const [scores, setScores] = useState<FundamentalScore[]>([])
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string | null>(null)

  const handleFilter = async (params: FundamentalFilterParams) => {
    setLoading(true)
    setError(null)
    try {
      const result = await fundamentalApi.filter(params)
      if (result.success && result.data) {
        setScores(result.data)
      } else {
        setError(result.message || '筛选失败')
        setScores([])
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : '请求超时，请稍后重试')
      setScores([])
    } finally {
      setLoading(false)
    }
  }

  const handleRetry = () => {
    setError(null)
    handleFilter({})
  }

  return (
    <div className="min-h-screen bg-primary p-6">
      <header className="mb-6">
        <h1 className="text-2xl font-bold text-text">基本面筛选</h1>
        <p className="text-muted text-sm mt-1">
          基于财务指标筛选优质股票，4维度评分模型
        </p>
      </header>

      <FundamentalFilter onFilter={handleFilter} />

      {error && (
        <div className="bg-red-50 border border-red-200 rounded-xl p-4 mb-6">
          <p className="text-red-600 text-sm">{error}</p>
          <button
            onClick={handleRetry}
            className="mt-2 px-4 py-2 bg-red-600 text-white rounded-lg hover:bg-red-700 transition-colors cursor-pointer text-sm"
          >
            重试
          </button>
        </div>
      )}

      <FundamentalCardGrid scores={scores} loading={loading} />

      {!loading && scores.length > 0 && (
        <div className="mt-6 text-center text-sm text-muted">
          共筛选出 {scores.length} 只优质股票
        </div>
      )}
    </div>
  )
}