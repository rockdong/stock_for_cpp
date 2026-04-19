interface CircleProgressProps {
  percent: number
  completed: number
  total: number
  size?: number
  strokeWidth?: number
}

export default function CircleProgress({
  percent,
  completed,
  total,
  size = 32,
  strokeWidth = 3
}: CircleProgressProps) {
  const radius = (size - strokeWidth) / 2
  const circumference = radius * 2 * Math.PI
  const offset = circumference - (percent / 100) * circumference

  return (
    <div className="flex items-center gap-2">
      <div className="relative inline-flex items-center justify-center" style={{ width: size, height: size }}>
        <svg width={size} height={size} className="transform -rotate-90">
          <circle
            cx={size / 2}
            cy={size / 2}
            r={radius}
            stroke="currentColor"
            strokeWidth={strokeWidth}
            fill="none"
            className="text-border-default"
          />
          <circle
            cx={size / 2}
            cy={size / 2}
            r={radius}
            stroke="currentColor"
            strokeWidth={strokeWidth}
            fill="none"
            strokeLinecap="round"
            className="text-accent-blue transition-all duration-500"
            style={{
              strokeDasharray: circumference,
              strokeDashoffset: offset
            }}
          />
        </svg>
        <div className="absolute inset-0 flex items-center justify-center">
          <span className="text-2xs font-mono text-text-secondary">{percent}%</span>
        </div>
      </div>
      <span className="text-xs font-mono text-text-tertiary tabular-nums">
        {completed}/{total}
      </span>
    </div>
  )
}
