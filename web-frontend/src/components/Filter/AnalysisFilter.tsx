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
    <form onSubmit={handleSubmit} className="bg-white rounded-lg shadow p-4 mb-6">
      <div className="grid grid-cols-1 md:grid-cols-5 gap-4">
        <div>
          <label className="block text-sm font-medium text-gray-700 mb-1">
            股票代码
          </label>
          <input
            type="text"
            value={filters.ts_code}
            onChange={e => setFilters({ ...filters, ts_code: e.target.value })}
            placeholder="如: 000001.SZ"
            className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
          />
        </div>

        <div>
          <label className="block text-sm font-medium text-gray-700 mb-1">
            策略
          </label>
          <select
            value={filters.strategy}
            onChange={e => setFilters({ ...filters, strategy: e.target.value })}
            className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
          >
            <option value="">全部策略</option>
            {strategies.map(s => (
              <option key={s} value={s}>{s}</option>
            ))}
          </select>
        </div>

        <div>
          <label className="block text-sm font-medium text-gray-700 mb-1">
            开始日期
          </label>
          <input
            type="date"
            value={filters.start_date}
            onChange={e => setFilters({ ...filters, start_date: e.target.value })}
            className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
          />
        </div>

        <div>
          <label className="block text-sm font-medium text-gray-700 mb-1">
            结束日期
          </label>
          <input
            type="date"
            value={filters.end_date}
            onChange={e => setFilters({ ...filters, end_date: e.target.value })}
            className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
          />
        </div>

        <div>
          <label className="block text-sm font-medium text-gray-700 mb-1">
            信号类型
          </label>
          <select
            value={filters.signal}
            onChange={e => setFilters({ ...filters, signal: e.target.value })}
            className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
          >
            <option value="">全部信号</option>
            <option value="BUY">买入</option>
            <option value="SELL">卖出</option>
            <option value="HOLD">持有</option>
            <option value="NONE">无信号</option>
          </select>
        </div>
      </div>

      <div className="flex justify-end gap-2 mt-4">
        <button
          type="button"
          onClick={handleReset}
          className="px-4 py-2 text-gray-600 bg-gray-100 rounded-md hover:bg-gray-200"
        >
          重置
        </button>
        <button
          type="submit"
          className="px-4 py-2 text-white bg-blue-500 rounded-md hover:bg-blue-600"
        >
          查询
        </button>
      </div>
    </form>
  )
}