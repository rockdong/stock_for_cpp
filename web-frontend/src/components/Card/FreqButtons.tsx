import { FreqType } from '../../types/analysis'

interface FreqButtonsProps {
  onFreqClick: (freq: FreqType) => void
}

const freqOptions: { freq: FreqType; label: string; icon: string }[] = [
  { freq: 'd', label: '日K', icon: 'D' },
  { freq: 'w', label: '周K', icon: 'W' },
  { freq: 'm', label: '月K', icon: 'M' },
]

export default function FreqButtons({ onFreqClick }: FreqButtonsProps) {
  return (
    <div className="flex gap-2">
      {freqOptions.map(({ freq, label, icon }) => (
        <button
          key={freq}
          onClick={() => onFreqClick(freq)}
          className="flex-1 flex items-center justify-center gap-1.5 px-3 py-2 text-sm font-medium text-text-secondary bg-elevated rounded-lg hover:bg-accent-blue-bg hover:text-accent-blue transition-colors cursor-pointer border border-transparent hover:border-accent-blue/20"
        >
          <span className="w-5 h-5 rounded text-2xs font-semibold bg-border-default/50 flex items-center justify-center">
            {icon}
          </span>
          {label}
        </button>
      ))}
    </div>
  )
}
