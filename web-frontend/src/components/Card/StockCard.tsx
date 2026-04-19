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
    <div className="card group cursor-pointer">
      <StockInfo record={record} />
      {strategies.length > 0 && (
        <div className="mt-3 pt-3 border-t border-border-default">
          <FreqButtons onFreqClick={handleFreqClick} />
        </div>
      )}
    </div>
  )
}
