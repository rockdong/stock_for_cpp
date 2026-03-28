import { AnalysisProcessRecord, StrategyData } from '../../types/analysis'

interface RecordTableProps {
  records: AnalysisProcessRecord[]
  onSelect: (record: AnalysisProcessRecord) => void
}

export default function RecordTable({ records, onSelect }: RecordTableProps) {
  const getStrategies = (record: AnalysisProcessRecord): StrategyData[] => {
    return record.data?.strategies || []
  }

  const getSignalSummary = (record: AnalysisProcessRecord): { buy: number; sell: number } => {
    const strategies = getStrategies(record)
    let buy = 0, sell = 0
    strategies.forEach(s => {
      s.freqs.forEach(f => {
        if (f.signal === 'BUY') buy++
        if (f.signal === 'SELL') sell++
      })
    })
    return { buy, sell }
  }

  const getLastClose = (record: AnalysisProcessRecord): number | null => {
    const strategies = getStrategies(record)
    if (strategies.length === 0) return null
    const firstStrategy = strategies[0]
    const dailyFreq = firstStrategy.freqs.find(f => f.freq === 'd')
    if (!dailyFreq || dailyFreq.candles.length === 0) return null
    return dailyFreq.candles[dailyFreq.candles.length - 1]?.close
  }

  if (records.length === 0) {
    return (
      <div className="bg-secondary rounded-2xl p-12 text-center border border-border">
        <div className="w-16 h-16 mx-auto mb-4 rounded-2xl bg-primary flex items-center justify-center">
          <svg className="w-8 h-8 text-muted" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={1.5} d="M9 17v-2m3 2v-4m3 4v-6m2 10H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
          </svg>
        </div>
        <p className="text-muted">暂无分析记录</p>
      </div>
    )
  }

  return (
    <div className="bg-secondary rounded-2xl border border-border overflow-hidden">
      <div className="overflow-x-auto">
        <table className="min-w-full">
          <thead>
            <tr className="border-b border-border">
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">日期</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">股票</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">策略数</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">信号概览</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">收盘价</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">操作</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-border">
            {records.map(record => {
              const { buy, sell } = getSignalSummary(record)
              return (
                <tr 
                  key={record.id} 
                  className="hover:bg-primary/50 transition-colors cursor-pointer"
                  onClick={() => onSelect(record)}
                >
                  <td className="px-5 py-4 text-sm text-text font-medium">{record.trade_date}</td>
                  <td className="px-5 py-4">
                    <div className="text-sm font-medium text-text">{record.stock_name}</div>
                    <div className="text-xs text-muted">{record.ts_code}</div>
                  </td>
                  <td className="px-5 py-4 text-sm text-muted">{getStrategies(record).length}</td>
                  <td className="px-5 py-4">
                    <div className="flex gap-2">
                      {buy > 0 && (
                        <span className="px-2 py-0.5 text-xs rounded bg-cta/20 text-cta">{buy} 买入</span>
                      )}
                      {sell > 0 && (
                        <span className="px-2 py-0.5 text-xs rounded bg-sell/20 text-sell">{sell} 卖出</span>
                      )}
                      {buy === 0 && sell === 0 && (
                        <span className="text-xs text-muted">无信号</span>
                      )}
                    </div>
                  </td>
                  <td className="px-5 py-4 text-sm text-text font-mono">
                    {getLastClose(record)?.toFixed(2) || '-'}
                  </td>
                  <td className="px-5 py-4">
                    <button className="px-3 py-1.5 text-xs font-medium text-cta bg-cta/10 rounded-lg hover:bg-cta/20 transition-colors cursor-pointer">
                      查看图表
                    </button>
                  </td>
                </tr>
              )
            })}
          </tbody>
        </table>
      </div>
    </div>
  )
}