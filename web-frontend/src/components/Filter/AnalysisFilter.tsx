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
  const [expanded, setExpanded] = useState(true)

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
    <div className="bg-surface rounded-xl border border-border-default overflow-hidden">
      {/* 筛选器头部 */}
      <button
        onClick={() => setExpanded(!expanded)}
        className="w-full flex items-center justify-between px-5 py-3.5 text-sm cursor-pointer hover:bg-elevated/50 transition-colors"
      >
        <div className="flex items-center gap-2">
          <svg className="w-4 h-4 text-text-tertiary" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
            <path strokeLinecap="round" strokeLinejoin="round" d="M12 3c2.755 0 5.455.232 8.083.678.533.09.917.556.917 1.096v1.044a2.256 2.256 0 01-.659 1.59l-4.432 4.432a2.256 2.256 0 00-.659 1.59v1.044a2.25 2.25 0 01-1.244 2.013l-1.376.688a2.25 2.25 0 01-3.005-.876l-.431-.739a2.25 2.25 0 00-.659-.876L3.659 7.408A2.256 2.256 0 013 5.818V4.774c0-.54.384-1.006.917-1.096A48.32 48.32 0 0112 3z" />
          </svg>
          <span className="font-medium text-text-primary">筛选条件</span>
          {Object.values(filters).some(v => v) && (
            <span className="w-1.5 h-1.5 rounded-full bg-accent-blue" />
          )}
        </div>
        <svg
          className={`w-4 h-4 text-text-tertiary transition-transform duration-200 ${expanded ? 'rotate-180' : ''}`}
          fill="none"
          viewBox="0 0 24 24"
          stroke="currentColor"
          strokeWidth={1.5}
        >
          <path strokeLinecap="round" strokeLinejoin="round" d="M19.5 8.25l-7.5 7.5-7.5-7.5" />
        </svg>
      </button>

      {/* 筛选器内容 */}
      {expanded && (
        <form onSubmit={handleSubmit} className="px-5 pb-5 border-t border-border-default">
          <div className="grid grid-cols-1 md:grid-cols-5 gap-4 pt-4">
            <div>
              <label className="block text-xs font-medium text-text-tertiary mb-1.5">
                股票代码
              </label>
              <input
                type="text"
                value={filters.ts_code}
                onChange={e => setFilters({ ...filters, ts_code: e.target.value })}
                placeholder="000001.SZ"
                className="input-field font-mono"
              />
            </div>

            <div>
              <label className="block text-xs font-medium text-text-tertiary mb-1.5">
                策略
              </label>
              <select
                value={filters.strategy}
                onChange={e => setFilters({ ...filters, strategy: e.target.value })}
                className="input-field cursor-pointer"
              >
                <option value="">全部策略</option>
                {strategies.map(s => (
                  <option key={s} value={s}>{s}</option>
                ))}
              </select>
            </div>

            <div>
              <label className="block text-xs font-medium text-text-tertiary mb-1.5">
                开始日期
              </label>
              <input
                type="date"
                value={filters.start_date}
                onChange={e => setFilters({ ...filters, start_date: e.target.value })}
                className="input-field"
              />
            </div>

            <div>
              <label className="block text-xs font-medium text-text-tertiary mb-1.5">
                结束日期
              </label>
              <input
                type="date"
                value={filters.end_date}
                onChange={e => setFilters({ ...filters, end_date: e.target.value })}
                className="input-field"
              />
            </div>

            <div>
              <label className="block text-xs font-medium text-text-tertiary mb-1.5">
                信号类型
              </label>
              <select
                value={filters.signal}
                onChange={e => setFilters({ ...filters, signal: e.target.value })}
                className="input-field cursor-pointer"
              >
                <option value="">全部信号</option>
                <option value="BUY">🟥 买入</option>
                <option value="SELL">🟩 卖出</option>
                <option value="HOLD">⬜ 持有</option>
                <option value="NONE">无信号</option>
              </select>
            </div>
          </div>

          <div className="flex justify-end gap-3 mt-4">
            <button
              type="button"
              onClick={handleReset}
              className="btn--ghost"
            >
              重置
            </button>
            <button
              type="submit"
              className="btn--primary"
            >
              <svg className="w-4 h-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M21 21l-5.197-5.197m0 0A7.5 7.5 0 105.196 5.196a7.5 7.5 0 0010.607 10.607z" />
              </svg>
              查询
            </button>
          </div>
        </form>
      )}
    </div>
  )
}
