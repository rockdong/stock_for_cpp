import { AnalysisProcessRecord } from '../../types/analysis'

interface RecordTableProps {
  records: AnalysisProcessRecord[]
  onSelect: (record: AnalysisProcessRecord) => void
}

export default function RecordTable({ records, onSelect }: RecordTableProps) {
  const getSignalBadge = (signal: string) => {
    const config: Record<string, { style: string; label: string }> = {
      BUY: { style: 'bg-cta/20 text-cta border-cta/30', label: '买入' },
      SELL: { style: 'bg-sell/20 text-sell border-sell/30', label: '卖出' },
      HOLD: { style: 'bg-hold/20 text-hold border-hold/30', label: '持有' },
      NONE: { style: 'bg-muted/10 text-muted border-muted/20', label: '无信号' },
    }
    const { style, label } = config[signal] || config.NONE
    return (
      <span className={`px-2.5 py-1 rounded-lg text-xs font-medium border ${style}`}>
        {label}
      </span>
    )
  }

  const getFreqBadge = (freq: string) => {
    const config: Record<string, { style: string; label: string }> = {
      d: { style: 'bg-blue-500/20 text-blue-400 border-blue-500/30', label: '日' },
      w: { style: 'bg-purple-500/20 text-purple-400 border-purple-500/30', label: '周' },
      m: { style: 'bg-amber-500/20 text-amber-400 border-amber-500/30', label: '月' },
    }
    const { style, label } = config[freq] || { style: 'bg-muted/10 text-muted border-muted/20', label: freq }
    return (
      <span className={`px-2 py-0.5 rounded text-xs font-medium border ${style}`}>
        {label}
      </span>
    )
  }

  const getLastClose = (data: AnalysisProcessRecord['data']) => {
    if (!data || data.length === 0) return null
    return data[data.length - 1]?.close
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
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">策略</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">周期</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">信号</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">收盘价</th>
              <th className="px-5 py-4 text-left text-xs font-medium text-muted uppercase tracking-wider">操作</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-border">
            {records.map(record => (
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
                <td className="px-5 py-4 text-sm text-muted">{record.strategy_name}</td>
                <td className="px-5 py-4">{getFreqBadge(record.freq)}</td>
                <td className="px-5 py-4">{getSignalBadge(record.signal)}</td>
                <td className="px-5 py-4 text-sm text-text font-mono">
                  {getLastClose(record.data)?.toFixed(2) || '-'}
                </td>
                <td className="px-5 py-4">
                  <button className="px-3 py-1.5 text-xs font-medium text-cta bg-cta/10 rounded-lg hover:bg-cta/20 transition-colors cursor-pointer">
                    查看图表
                  </button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  )
}