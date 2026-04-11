import { FreqType } from '../../types/analysis'

interface ModalHeaderProps {
  stockName: string
  tsCode: string
  tradeDate: string
  strategy: string
  freq: FreqType
  onFreqChange: (freq: FreqType) => void
  onClose: () => void
}

const freqOptions: { freq: FreqType; label: string }[] = [
  { freq: 'd', label: '日K' },
  { freq: 'w', label: '周K' },
  { freq: 'm', label: '月K' },
]

export default function ModalHeader({ 
  stockName, 
  tsCode, 
  tradeDate, 
  strategy, 
  freq, 
  onFreqChange, 
  onClose 
}: ModalHeaderProps) {
  return (
    <div className="flex items-center justify-between px-6 py-4 border-b border-gray-200 bg-white">
      <div className="flex items-center gap-4">
        <div>
          <div className="text-lg font-semibold text-gray-900">{stockName}</div>
          <div className="text-sm text-gray-500">{tsCode} | {tradeDate} | {strategy}</div>
        </div>
      </div>
      
      <div className="flex items-center gap-3">
        <div className="flex gap-2">
          {freqOptions.map(({ freq: f, label }) => (
            <button
              key={f}
              onClick={() => onFreqChange(f)}
              className={`px-3 py-1.5 text-sm font-medium rounded-lg transition-colors cursor-pointer ${
                freq === f 
                  ? 'bg-blue-500 text-white' 
                  : 'bg-gray-100 text-gray-600 hover:bg-gray-200'
              }`}
            >
              {label}
            </button>
          ))}
        </div>
        
        <button
          onClick={onClose}
          className="p-2 text-gray-400 hover:text-gray-600 hover:bg-gray-100 rounded-lg transition-colors cursor-pointer"
        >
          <svg className="w-5 h-5" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
          </svg>
        </button>
      </div>
    </div>
  )
}