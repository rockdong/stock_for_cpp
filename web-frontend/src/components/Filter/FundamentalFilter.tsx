import { useState } from 'react'
import { FundamentalFilterParams } from '../../types/fundamental'

interface FundamentalFilterProps {
  onFilter: (params: FundamentalFilterParams) => void
}

export default function FundamentalFilter({ onFilter }: FundamentalFilterProps) {
  const [filters, setFilters] = useState({
    pe_max: '30',
    pb_max: '5',
    roe_min: '10',
    gross_margin_min: '20',
    total_score_min: '60',
  })

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault()
    const params: FundamentalFilterParams = {
      pe_max: parseFloat(filters.pe_max) || 30,
      pb_max: parseFloat(filters.pb_max) || 5,
      roe_min: parseFloat(filters.roe_min) || 10,
      gross_margin_min: parseFloat(filters.gross_margin_min) || 20,
      total_score_min: parseFloat(filters.total_score_min) || 60,
    }
    onFilter(params)
  }

  const handleReset = () => {
    setFilters({
      pe_max: '30',
      pb_max: '5',
      roe_min: '10',
      gross_margin_min: '20',
      total_score_min: '60',
    })
    onFilter({})
  }

  return (
    <form onSubmit={handleSubmit} className="bg-secondary rounded-2xl p-5 mb-6 border border-border">
      <div className="grid grid-cols-1 md:grid-cols-5 gap-4">
        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            PE 上限
          </label>
          <input
            type="number"
            value={filters.pe_max}
            onChange={e => setFilters({ ...filters, pe_max: e.target.value })}
            placeholder="30"
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text placeholder-muted/50 focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            PB 上限
          </label>
          <input
            type="number"
            value={filters.pb_max}
            onChange={e => setFilters({ ...filters, pb_max: e.target.value })}
            placeholder="5"
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text placeholder-muted/50 focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            ROE 下限
          </label>
          <input
            type="number"
            value={filters.roe_min}
            onChange={e => setFilters({ ...filters, roe_min: e.target.value })}
            placeholder="10"
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text placeholder-muted/50 focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            毛利率下限
          </label>
          <input
            type="number"
            value={filters.gross_margin_min}
            onChange={e => setFilters({ ...filters, gross_margin_min: e.target.value })}
            placeholder="20"
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text placeholder-muted/50 focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
        </div>

        <div>
          <label className="block text-xs font-medium text-muted mb-2">
            总分下限
          </label>
          <input
            type="number"
            value={filters.total_score_min}
            onChange={e => setFilters({ ...filters, total_score_min: e.target.value })}
            placeholder="60"
            className="w-full px-4 py-2.5 bg-primary border border-border rounded-xl text-text placeholder-muted/50 focus:outline-none focus:ring-2 focus:ring-cta/50 focus:border-cta transition-colors"
          />
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
          筛选
        </button>
      </div>
    </form>
  )
}