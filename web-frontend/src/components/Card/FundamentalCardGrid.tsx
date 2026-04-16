import { FundamentalScore } from '../../types/fundamental'
import FundamentalCard from './FundamentalCard'

interface FundamentalCardGridProps {
  scores: FundamentalScore[]
  stockNames?: Record<string, string>
  loading?: boolean
}

export default function FundamentalCardGrid({ scores, stockNames, loading }: FundamentalCardGridProps) {
  if (loading) {
    return (
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-4">
        {[1, 2, 3, 4].map(i => (
          <div key={i} className="bg-white rounded-xl border border-gray-200 p-4 animate-pulse">
            <div className="h-4 bg-gray-200 rounded w-1/2 mb-2" />
            <div className="h-3 bg-gray-200 rounded w-1/3 mb-3" />
            <div className="h-2 bg-gray-200 rounded w-full mb-2" />
            <div className="h-2 bg-gray-200 rounded w-3/4" />
          </div>
        ))}
      </div>
    )
  }

  if (scores.length === 0) {
    return (
      <div className="text-center py-12">
        <p className="text-muted">暂无筛选结果</p>
      </div>
    )
  }

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-4">
      {scores.map(score => (
        <FundamentalCard
          key={score.ts_code}
          score={score}
          stockName={stockNames?.[score.ts_code]}
        />
      ))}
    </div>
  )
}