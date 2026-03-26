import { useState, useEffect } from 'react'
import { analysisApi } from '../../services/api'
import { FilterParams } from '../../types/analysis'

interface FilterProps {
  onFilter: (params: FilterParams) => void
}

export default function AnalysisFilter({ onFilter }: FilterProps) {
  const [strategies, setStrategies] = useState<string[]>([])
  const [filters, setFilters] = useState({
    ts_code: '',
    strategy: '',
    start_date: '',
    end_date: '',
    signal: '',
  })

  useEffect(() => {
    analysisApi.getStrategies().then(setStrategies).catch(console.error)
  }, [])

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault()
    const params: FilterParams = {}
    Object.entries(filters).forEach(([key, value]) => {
      if (value) params[key as keyof FilterParams] = value
    })
    onFilter(params)
  }

  const handleReset = () => {
    setFilters({
      ts_code: '',
      strategy: '',
      start_date: '',
      end_date: '',
      signal: '',
    })
    onFilter({})
  }

  return (
    <form onSubmit={handleSubmit} className="bg-secondary rounded-2xl p-5 mb-6 border border-border">
      <div className="grid grid-cols-1 md:grid-cols-5 gap-4">
        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            股票代码
          </label>
          <input
            type="text"
            value={filters.ts_code}
            onChange={e => setFilters({ ...filters, ts_code: e.target.value })}
            placeholder="000001.SZ"
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text placeholder-muted/50 focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            策略
          </label>
          <select
            value={filters.strategy}
            onChange={e => setFilters({ ...filters, strategy: e.target.value })}
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors cursor-pointer"
          >
            <option value="">全部策略</option>
            {strategies.map(s => (
              <option key={s} value={s}>{s}</option>
            ))}
          </select>
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            开始日期
          </label>
          <input
            type="date"
            value={filters.start_date}
            onChange={e => setFilters({ ...filters, start_date: e.target.value })}
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            结束日期
          </label>
          <input
            type="date"
            value={filters.end_date}
            onChange={e => setFilters({ ...filters, end_date: e.target.value })}
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            信号类型
          </label>
          <select
            value={filters.signal}
            onChange={e => setFilters({ ...filters, signal: e.target.value })}
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors cursor-pointer"
          >
            <option value="">全部信号</option>
            <option value="BUY">买入</option>
            <option value="SELL">卖出</option>
            <option value="HOLD">持有</option>
            <option value="NONE">无信号</option>
          </select>
        </div>
      </div>

      <div className="flex justify-end gap-3 mt-5">
        <button
          type="button"
          onClick={handleReset}
          className="px-5 py-2.5 text-muted bg-primary rounded-xl hover:bg-primary/80 transition-colors cursor-pointer"
        >
          重置
        </button>
        <button
          type="submit"
          className="px-5 py-2.5 text-white bg-cta rounded-xl hover:bg-cta/90 transition-colors cursor-pointer font-medium"
        >
          查询
        </button>
      </div>
    </form>
  )
}