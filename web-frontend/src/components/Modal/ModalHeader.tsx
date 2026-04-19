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
  onClose,
}: ModalHeaderProps) {
  return (
    <div className="flex items-center justify-between px-6 py-4 border-b border-border-default">
      <div className="flex items-center gap-4">
        {/* 股票信息 */}
        <div>
          <div className="flex items-center gap-2">
            <h3 className="text-base font-semibold text-text-primary">{stockName}</h3>
            <span className="text-xs text-text-tertiary font-mono">{tsCode}</span>
          </div>
          <p className="text-xs text-text-tertiary mt-0.5">{tradeDate} · {strategy}</p>
        </div>

        {/* 周期切换 */}
        <div className="flex items-center bg-surface rounded-lg p-0.5 ml-2">
          {freqOptions.map(({ freq: f, label }) => (
            <button
              key={f}
              onClick={() => onFreqChange(f)}
              className={`px-3 py-1.5 text-xs font-medium rounded-md transition-colors cursor-pointer ${
                freq === f
                  ? 'bg-accent-blue text-white shadow-sm'
                  : 'text-text-tertiary hover:text-text-secondary'
              }`}
            >
              {label}
            </button>
          ))}
        </div>
      </div>

      {/* 关闭按钮 */}
      <button
        onClick={onClose}
        className="w-8 h-8 flex items-center justify-center rounded-lg text-text-tertiary hover:text-text-primary hover:bg-surface transition-colors cursor-pointer"
      >
        <svg className="w-5 h-5" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
          <path strokeLinecap="round" strokeLinejoin="round" d="M6 18L18 6M6 6l12 12" />
        </svg>
      </button>
    </div>
  )
}
