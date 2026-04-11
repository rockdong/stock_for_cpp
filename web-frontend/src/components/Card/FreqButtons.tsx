import { FreqType } from '../../types/analysis'

interface FreqButtonsProps {
  onFreqClick: (freq: FreqType) => void
}

const freqOptions: { freq: FreqType; label: string }[] = [
  { freq: 'd', label: '日K' },
  { freq: 'w', label: '周K' },
  { freq: 'm', label: '月K' },
]

export default function FreqButtons({ onFreqClick }: FreqButtonsProps) {
  return (
    <div className="flex gap-2 pt-3 border-t border-gray-200">
      {freqOptions.map(({ freq, label }) => (
        <button
          key={freq}
          onClick={() => onFreqClick(freq)}
          className="flex-1 px-3 py-2 text-sm font-medium text-gray-700 bg-gray-100 rounded-lg hover:bg-blue-100 hover:text-blue-600 transition-colors cursor-pointer"
        >
          {label}
        </button>
      ))}
    </div>
  )
}