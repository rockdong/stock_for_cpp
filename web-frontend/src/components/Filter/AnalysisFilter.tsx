import { useState, useEffect } from 'react'
import { analysisApi } from '../../services/api'
import { FilterParams, FreqType, SignalType } from '../../types/analysis'
import { DatePicker } from '@/components/ui/date-picker'

interface FilterProps {
  onFilter: (params: FilterParams) => void
}

const freqOptions: { freq: FreqType; label: string }[] = [
  { freq: 'd', label: '日K' },
  { freq: 'w', label: '周K' },
  { freq: 'm', label: '月K' },
]

export default function AnalysisFilter({ onFilter }: FilterProps) {
  const [strategies, setStrategies] = useState<string[]>([])
  const [filters, setFilters] = useState<{
    ts_code: string
    start_date: string
    end_date: string
    signal: SignalType | ''
  }>({
    ts_code: '',
    start_date: '',
    end_date: '',
    signal: '',
  })
  const [selectedStrategies, setSelectedStrategies] = useState<string[]>([])
  const [selectedFreqs, setSelectedFreqs] = useState<FreqType[]>([])
  const [expanded, setExpanded] = useState(true)

  useEffect(() => {
    analysisApi.getStrategies().then(setStrategies).catch(console.error)
  }, [])

  const toggleStrategy = (strategy: string) => {
    setSelectedStrategies(prev =>
      prev.includes(strategy)
        ? prev.filter(s => s !== strategy)
        : [...prev, strategy]
    )
  }

  const toggleFreq = (freq: FreqType) => {
    setSelectedFreqs(prev =>
      prev.includes(freq)
        ? prev.filter(f => f !== freq)
        : [...prev, freq]
    )
  }

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault()
    const params: FilterParams = {}
    if (filters.ts_code) params.ts_code = filters.ts_code
    if (filters.start_date) params.start_date = filters.start_date
    if (filters.end_date) params.end_date = filters.end_date
    if (filters.signal) params.signal = filters.signal as SignalType
    if (selectedStrategies.length > 0) {
      params.strategy = selectedStrategies.join(',')
    }
    if (selectedFreqs.length > 0) {
      params.freq = selectedFreqs
    }
    onFilter(params)
  }

  const handleReset = () => {
    setFilters({
      ts_code: '',
      start_date: '',
      end_date: '',
      signal: '',
    })
    setSelectedStrategies([])
    setSelectedFreqs([])
    onFilter({})
  }

  const hasActiveFilters = Object.values(filters).some(v => v) || selectedStrategies.length > 0 || selectedFreqs.length > 0

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
          {hasActiveFilters && (
            <span className="px-1.5 py-0.5 rounded text-2xs font-medium bg-accent-blue/20 text-accent-blue">
              {selectedStrategies.length + selectedFreqs.length + Object.values(filters).filter(v => v).length}
            </span>
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
          <div className="grid grid-cols-1 md:grid-cols-4 gap-4 pt-4">
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
                开始日期
              </label>
              <DatePicker
                value={filters.start_date}
                onChange={value => setFilters({ ...filters, start_date: value })}
                placeholder="选择开始日期"
              />
            </div>

            <div>
              <label className="block text-xs font-medium text-text-tertiary mb-1.5">
                结束日期
              </label>
              <DatePicker
                value={filters.end_date}
                onChange={value => setFilters({ ...filters, end_date: value })}
                placeholder="选择结束日期"
              />
            </div>

            <div>
              <label className="block text-xs font-medium text-text-tertiary mb-1.5">
                信号类型
              </label>
              <select
                value={filters.signal}
                onChange={e => setFilters({ ...filters, signal: e.target.value as SignalType | '' })}
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

          {/* 策略多选 */}
          {strategies.length > 0 && (
            <div className="mt-4">
              <label className="block text-xs font-medium text-text-tertiary mb-2">
                策略
              </label>
              <div className="flex flex-wrap gap-2">
                {strategies.map(strategy => (
                  <button
                    key={strategy}
                    type="button"
                    onClick={() => toggleStrategy(strategy)}
                    className={`px-4 py-2 rounded-lg text-sm font-medium transition-all cursor-pointer border ${
                      selectedStrategies.includes(strategy)
                        ? 'bg-accent-blue text-white border-accent-blue shadow-glow-blue'
                        : 'bg-elevated text-text-secondary border-border-default hover:border-accent-blue/50 hover:text-accent-blue'
                    }`}
                  >
                    {strategy}
                  </button>
                ))}
              </div>
            </div>
          )}

          {/* 频率多选 */}
          <div className="mt-4">
            <label className="block text-xs font-medium text-text-tertiary mb-2">
              频率
            </label>
            <div className="flex gap-2">
              {freqOptions.map(({ freq, label }) => (
                <button
                  key={freq}
                  type="button"
                  onClick={() => toggleFreq(freq)}
                  className={`px-4 py-2 rounded-lg text-sm font-medium transition-all cursor-pointer border ${
                    selectedFreqs.includes(freq)
                      ? 'bg-accent-blue text-white border-accent-blue shadow-glow-blue'
                      : 'bg-elevated text-text-secondary border-border-default hover:border-accent-blue/50 hover:text-accent-blue'
                  }`}
                >
                  {label}
                </button>
              ))}
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