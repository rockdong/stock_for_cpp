import { FundamentalScore } from '../../types/fundamental'

interface FundamentalCardProps {
  score: FundamentalScore
  stockName?: string
}

export default function FundamentalCard({ score, stockName }: FundamentalCardProps) {
  const gradeColors: Record<string, string> = {
    A: 'bg-green-500',
    B: 'bg-blue-500',
    C: 'bg-yellow-500',
    D: 'bg-red-500',
  }

  return (
    <div className="bg-white rounded-xl border border-gray-200 shadow-sm hover:shadow-md transition-shadow p-4">
      <div className="flex items-center justify-between mb-3">
        <div>
          <h3 className="font-semibold text-text">
            {stockName || score.ts_code}
          </h3>
          <p className="text-sm text-muted">{score.ts_code}</p>
        </div>
        <div className={`px-3 py-1 rounded-lg text-white font-bold ${gradeColors[score.grade] || 'bg-gray-500'}`}>
          {score.grade}
        </div>
      </div>

      <div className="mb-3">
        <div className="flex items-center justify-between text-sm mb-1">
          <span className="text-muted">总分</span>
          <span className="font-bold text-text">{score.total.toFixed(1)}</span>
        </div>
        <div className="w-full bg-gray-200 rounded-full h-2">
          <div 
            className="bg-cta rounded-full h-2 transition-all"
            style={{ width: `${Math.min(score.total, 100)}%` }}
          />
        </div>
      </div>

      <div className="grid grid-cols-2 gap-2 text-sm">
        <div className="flex items-center justify-between">
          <span className="text-muted">估值</span>
          <span className="text-text">{score.valuation.toFixed(1)}</span>
        </div>
        <div className="flex items-center justify-between">
          <span className="text-muted">盈利</span>
          <span className="text-text">{score.quality.toFixed(1)}</span>
        </div>
        <div className="flex items-center justify-between">
          <span className="text-muted">增长</span>
          <span className="text-text">{score.growth.toFixed(1)}</span>
        </div>
        <div className="flex items-center justify-between">
          <span className="text-muted">健康</span>
          <span className="text-text">{score.health.toFixed(1)}</span>
        </div>
      </div>

      {score.reason && (
        <p className="mt-3 text-xs text-muted border-t border-gray-100 pt-2">
          {score.reason}
        </p>
      )}
    </div>
  )
}