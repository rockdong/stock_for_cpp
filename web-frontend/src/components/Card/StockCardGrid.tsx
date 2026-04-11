import { AnalysisProcessRecord, FreqType } from '../../types/analysis'
import StockCard from './StockCard'

interface StockCardGridProps {
  records: AnalysisProcessRecord[]
  onFreqClick: (record: AnalysisProcessRecord, freq: FreqType, strategy: string) => void
}

export default function StockCardGrid({ records, onFreqClick }: StockCardGridProps) {
  if (records.length === 0) {
    return (
      <div className="bg-white rounded-2xl p-12 text-center border border-gray-200">
        <div className="w-16 h-16 mx-auto mb-4 rounded-2xl bg-gray-100 flex items-center justify-center">
          <svg className="w-8 h-8 text-gray-400" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={1.5} d="M9 17v-2m3 2v-4m3 4v-6m2 10H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
          </svg>
        </div>
        <p className="text-gray-500">暂无分析记录</p>
      </div>
    )
  }
  
  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
      {records.map(record => (
        <StockCard
          key={record.id}
          record={record}
          onFreqClick={onFreqClick}
        />
      ))}
    </div>
  )
}