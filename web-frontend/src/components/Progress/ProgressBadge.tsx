import { useState, useEffect } from 'react'
import { useProgress } from '../../contexts/ProgressContext'
import { ProgressStatus } from '../../types/analysis'

function CountdownTimer({ executeTime }: { executeTime: string }) {
  const [countdown, setCountdown] = useState({ hours: 0, minutes: 0, seconds: 0 })
  
  useEffect(() => {
    const calculateCountdown = () => {
      const now = new Date()
      const [hour, minute] = executeTime.split(':').map(Number)
      
      const target = new Date()
      target.setHours(hour, minute, 0, 0)
      
      if (target <= now) {
        target.setDate(target.getDate() + 1)
      }
      
      const diff = target.getTime() - now.getTime()
      
      const hours = Math.floor(diff / (1000 * 60 * 60))
      const minutes = Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60))
      const seconds = Math.floor((diff % (1000 * 60)) / 1000)
      
      setCountdown({ hours, minutes, seconds })
    }
    
    calculateCountdown()
    const interval = setInterval(calculateCountdown, 1000)
    return () => clearInterval(interval)
  }, [executeTime])
  
  const format = (n: number) => n.toString().padStart(2, '0')
  
  return (
    <span className="text-sm text-gray-400 font-mono">
      {format(countdown.hours)}:{format(countdown.minutes)}:{format(countdown.seconds)}
    </span>
  )
}

function PhaseProgressDisplay({ 
  phase, 
  label, 
  showQualified 
}: { 
  phase: { status: ProgressStatus; total: number; completed: number; qualified?: number; failed?: number }
  label: string
  showQualified?: boolean
}) {
  const percentage = phase.total > 0 
    ? Math.round((phase.completed / phase.total) * 100) 
    : 0
  
  const getStatusColor = (status: ProgressStatus) => {
    if (status === 'running') return 'text-blue-400'
    if (status === 'completed') return 'text-emerald-400'
    return 'text-gray-400'
  }
  
  const getStatusIcon = (status: ProgressStatus) => {
    if (status === 'running') {
      return (
        <div className="w-4 h-4 border-2 border-blue-400 border-t-transparent rounded-full animate-spin" />
      )
    }
    if (status === 'completed') {
      return (
        <svg className="w-4 h-4 text-emerald-400" fill="none" viewBox="0 0 24 24" stroke="currentColor">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 13l4 4L19 7" />
        </svg>
      )
    }
    return <div className="w-2 h-2 rounded-full bg-gray-400" />
  }
  
  return (
    <div className="flex flex-col gap-1">
      <div className="flex items-center gap-2">
        {getStatusIcon(phase.status)}
        <span className={`text-xs ${getStatusColor(phase.status)} font-medium`}>
          {label} {phase.status === 'idle' ? '等待中' : phase.status === 'running' ? '分析中' : '完成'}
        </span>
      </div>
      
      {phase.status === 'running' && (
        <div className="flex items-center gap-3">
          <div className="w-32 h-1.5 bg-gray-700 rounded-full overflow-hidden">
            <div 
              className="h-full bg-gradient-to-r from-blue-500 to-emerald-400 transition-all duration-500 rounded-full"
              style={{ width: `${percentage}%` }}
            />
          </div>
          <span className="text-xs text-gray-500 font-mono">
            {phase.completed}/{phase.total}
          </span>
        </div>
      )}
      
      {phase.status === 'completed' && showQualified && phase.qualified && (
        <span className="text-xs text-gray-400">
          筛选出 {phase.qualified} 只优质股票
        </span>
      )}
      
      {phase.status === 'completed' && phase.failed && phase.failed > 0 && (
        <span className="text-xs text-red-400">
          失败 {phase.failed} 项
        </span>
      )}
    </div>
  )
}

export default function ProgressBadge() {
  const { progress, isLoading, error } = useProgress()
  const [expanded, setExpanded] = useState(false)
  
  const phase1 = progress.phase1
  const phase2 = progress.phase2
  const executeTime = progress.execute_time
  
  const isRunning = phase1.status === 'running' || phase2.status === 'running'
  const isIdle = phase1.status === 'idle' && phase2.status === 'idle'
  const isCompleted = phase1.status === 'completed' && phase2.status === 'completed'
  
  if (error) {
    return (
      <div className="flex items-center gap-2 px-3 py-2 rounded-lg bg-red-500/10 border border-red-500/30">
        <svg className="w-4 h-4 text-red-400" fill="none" viewBox="0 0 24 24" stroke="currentColor">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
        </svg>
        <span className="text-xs text-red-400">连接失败</span>
      </div>
    )
  }
  
  if (isLoading) {
    return (
      <div className="flex items-center gap-2 px-3 py-2 rounded-lg bg-gray-800 border border-gray-700">
        <div className="w-4 h-4 border-2 border-gray-500 border-t-gray-300 rounded-full animate-spin" />
        <span className="text-xs text-gray-400">加载中</span>
      </div>
    )
  }
  
  if (isIdle) {
    return (
      <div 
        className="flex items-center gap-3 px-4 py-2 rounded-lg bg-gray-800 border border-gray-700 cursor-pointer hover:bg-gray-750 transition-colors"
        onClick={() => setExpanded(!expanded)}
      >
        <div className="w-2 h-2 rounded-full bg-gray-400" />
        <div className="flex flex-col">
          <span className="text-xs text-gray-400 font-medium">等待分析</span>
          {expanded && (
            <span className="text-xs text-gray-500">
              下次执行: {executeTime}
            </span>
          )}
        </div>
        {!expanded && <CountdownTimer executeTime={executeTime} />}
      </div>
    )
  }
  
  if (isCompleted) {
    return (
      <div className="flex items-center gap-2 px-3 py-2 rounded-lg bg-emerald-500/10 border border-emerald-500/30">
        <svg className="w-4 h-4 text-emerald-400" fill="none" viewBox="0 0 24 24" stroke="currentColor">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
        </svg>
        <span className="text-xs text-emerald-400 font-medium">分析完成</span>
      </div>
    )
  }
  
  return (
    <div className="flex flex-col gap-2 px-4 py-3 rounded-lg bg-gray-800 border border-gray-700">
      {!isRunning && isIdle && (
        <div className="flex items-center gap-2">
          <div className="w-2 h-2 rounded-full bg-gray-400" />
          <span className="text-xs text-gray-400 font-medium">等待分析</span>
          <CountdownTimer executeTime={executeTime} />
        </div>
      )}
      
      {(phase1.status !== 'idle' || phase2.status !== 'idle') && (
        <>
          <PhaseProgressDisplay phase={phase1} label="基本面" showQualified />
          {phase1.status !== 'idle' && phase2.status !== 'idle' && (
            <div className="w-full h-px bg-gray-700" />
          )}
          <PhaseProgressDisplay phase={phase2} label="技术面" />
        </>
      )}
    </div>
  )
}