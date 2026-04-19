import { useEffect, useRef } from 'react'
import { useProgressStore } from '../../stores/progressStore'
import CircleProgress from './CircleProgress'

export default function ProgressBadge() {
  const { progress, isLoading, error, fetchProgress } = useProgressStore()
  const isFirstLoad = useRef(true)

  const phase1 = progress.phase1
  const phase2 = progress.phase2
  const executeTime = progress.execute_time

  const p1Percent = phase1.total > 0 ? Math.round((phase1.completed / phase1.total) * 100) : 0
  const p2Percent = phase2.total > 0 ? Math.round((phase2.completed / phase2.total) * 100) : 0

  const isIdle = phase1.status === 'idle' && phase2.status === 'idle'
  const isCompleted = phase1.status === 'completed' && phase2.status === 'completed'

  useEffect(() => {
    fetchProgress().finally(() => {
      isFirstLoad.current = false
    })
    const interval = setInterval(fetchProgress, 5000)
    return () => clearInterval(interval)
  }, [fetchProgress])

  if (error) {
    return (
      <span className="text-xs text-signal-buy flex items-center gap-1.5">
        <span className="w-1.5 h-1.5 rounded-full bg-signal-buy" />
        连接失败
      </span>
    )
  }

  // 只在首次加载时显示加载状态，避免轮询时闪烁
  if (isLoading && isFirstLoad.current) {
    return (
      <span className="text-xs text-text-tertiary flex items-center gap-1.5">
        <span className="w-1.5 h-1.5 rounded-full bg-text-tertiary animate-pulse" />
        加载中
      </span>
    )
  }

  if (isIdle) {
    return (
      <span className="text-xs text-text-tertiary flex items-center gap-2">
        <span className="w-1.5 h-1.5 rounded-full bg-text-tertiary" />
        等待分析
        {executeTime && <span className="text-text-tertiary/60">{executeTime}</span>}
      </span>
    )
  }

  if (isCompleted) {
    return (
      <span className="text-xs text-signal-sell flex items-center gap-1.5">
        <span className="w-1.5 h-1.5 rounded-full bg-signal-sell" />
        分析完成
      </span>
    )
  }

  return (
    <div className="flex items-center gap-4 text-xs">
      {phase1.status !== 'idle' && (
        <div className="flex items-center gap-2">
          <span className={`w-1.5 h-1.5 rounded-full ${phase1.status === 'running' ? 'bg-accent-blue animate-pulse' : 'bg-signal-sell'}`} />
          <span className="text-text-tertiary font-medium">基本面</span>
          {phase1.status === 'running' && (
            <CircleProgress
              percent={p1Percent}
              completed={phase1.completed}
              total={phase1.total}
              size={28}
              strokeWidth={2.5}
            />
          )}
          {phase1.status === 'completed' && (
            <span className="text-signal-sell text-sm">✓</span>
          )}
        </div>
      )}

      {phase1.status !== 'idle' && phase2.status !== 'idle' && (
        <span className="text-border-default">|</span>
      )}

      {phase2.status !== 'idle' && (
        <div className="flex items-center gap-2">
          <span className={`w-1.5 h-1.5 rounded-full ${phase2.status === 'running' ? 'bg-accent-blue animate-pulse' : 'bg-signal-sell'}`} />
          <span className="text-text-tertiary font-medium">技术面</span>
          {phase2.status === 'running' && (
            <CircleProgress
              percent={p2Percent}
              completed={phase2.completed}
              total={phase2.total}
              size={28}
              strokeWidth={2.5}
            />
          )}
          {phase2.status === 'completed' && (
            <span className="text-signal-sell text-sm">✓</span>
          )}
        </div>
      )}
    </div>
  )
}
