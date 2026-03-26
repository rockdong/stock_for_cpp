import { AnalysisProcessRecord } from '../../types/analysis'

interface RecordTableProps {
  records: AnalysisProcessRecord[]
  onSelect: (record: AnalysisProcessRecord) => void
}

export default function RecordTable({ records, onSelect }: RecordTableProps) {
  const getSignalBadge = (signal: string) => {
    const styles: Record<string, string> = {
      BUY: 'bg-green-100 text-green-800',
      SELL: 'bg-red-100 text-red-800',
      HOLD: 'bg-gray-100 text-gray-800',
      NONE: 'bg-gray-50 text-gray-500',
    }
    const labels: Record<string, string> = {
      BUY: '买入',
      SELL: '卖出',
      HOLD: '持有',
      NONE: '无信号',
    }
    return (
      <span className={`px-2 py-1 rounded text-xs font-medium ${styles[signal] || styles.NONE}`}>
        {labels[signal] || signal}
      </span>
    )
  }

  const getFreqLabel = (freq: string) => {
    const labels: Record<string, string> = {
      d: '日线',
      w: '周线',
      m: '月线',
    }
    return labels[freq] || freq
  }

  const getLastClose = (data: AnalysisProcessRecord['data']) => {
    if (!data || data.length === 0) return null
    return data[data.length - 1]?.close
  }

  if (records.length === 0) {
    return (
      <div className="bg-white rounded-lg shadow p-8 text-center text-gray-500">
        暂无分析记录
      </div>
    )
  }

  return (
    <div className="bg-white rounded-lg shadow overflow-hidden">
      <table className="min-w-full divide-y divide-gray-200">
        <thead className="bg-gray-50">
          <tr>
            <th className="px-4 py-3 text-left text-xs font-medium text-gray-500 uppercase">日期</th>
            <th className="px-4 py-3 text-left text-xs font-medium text-gray-500 uppercase">股票</th>
            <th className="px-4 py-3 text-left text-xs font-medium text-gray-500 uppercase">策略</th>
            <th className="px-4 py-3 text-left text-xs font-medium text-gray-500 uppercase">周期</th>
            <th className="px-4 py-3 text-left text-xs font-medium text-gray-500 uppercase">信号</th>
            <th className="px-4 py-3 text-left text-xs font-medium text-gray-500 uppercase">收盘价</th>
            <th className="px-4 py-3 text-left text-xs font-medium text-gray-500 uppercase">操作</th>
          </tr>
        </thead>
        <tbody className="bg-white divide-y divide-gray-200">
          {records.map(record => (
            <tr 
              key={record.id} 
              className="hover:bg-gray-50 cursor-pointer"
              onClick={() => onSelect(record)}
            >
              <td className="px-4 py-3 text-sm text-gray-900">{record.trade_date}</td>
              <td className="px-4 py-3 text-sm text-gray-900">
                <div className="font-medium">{record.stock_name}</div>
                <div className="text-gray-500 text-xs">{record.ts_code}</div>
              </td>
              <td className="px-4 py-3 text-sm text-gray-900">{record.strategy_name}</td>
              <td className="px-4 py-3 text-sm text-gray-900">{getFreqLabel(record.freq)}</td>
              <td className="px-4 py-3 text-sm">{getSignalBadge(record.signal)}</td>
              <td className="px-4 py-3 text-sm text-gray-900">{getLastClose(record.data)?.toFixed(2)}</td>
              <td className="px-4 py-3 text-sm">
                <button className="text-blue-500 hover:text-blue-700">
                  查看图表
                </button>
              </td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  )
}