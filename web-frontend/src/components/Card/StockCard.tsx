import { AnalysisProcessRecord, FreqType } from '../../types/analysis'
import StockInfo from './StockInfo'
import FreqButtons from './FreqButtons'

interface StockCardProps {
  record: AnalysisProcessRecord
  defaultStrategy?: string
  onFreqClick: (record: AnalysisProcessRecord, freq: FreqType, strategy: string) => void
}

export default function StockCard({ record, defaultStrategy, onFreqClick }: StockCardProps) {
  const strategies = record.data?.strategies || []
  const strategy = defaultStrategy || (strategies.length > 0 ? strategies[0].name : '')
  
  const handleFreqClick = (freq: FreqType) => {
    if (strategy) {
      onFreqClick(record, freq, strategy)
    }
  }
  
  return (
    <div className="bg-white rounded-xl border border-gray-200 shadow-sm hover:shadow-md transition-shadow p-4">
      <StockInfo record={record} />
      {strategies.length > 0 && (
        <FreqButtons onFreqClick={handleFreqClick} />
      )}
    </div>
  )
}