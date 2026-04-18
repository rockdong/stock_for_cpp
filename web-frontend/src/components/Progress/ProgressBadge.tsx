import { useProgress, usePhase1Progress, usePhase2Progress } from '../../contexts/ProgressContext'
import { ProgressStatus } from '../../types/analysis'

function StatusIndicator({ status, label }: { status: ProgressStatus; label: string }) {
  if (status === 'idle') {
    return (
      <div className="flex items-center gap-2">
        <div className="w-2 h-2 rounded-full bg-gray-400"></div>
        <span className="text-xs text-gray-500">{label} 等待中</span>
      </div>
    )
  }
  
  if (status === 'running') {
    return (
      <div className="flex items-center gap-2">
        <div className="w-4 h-4 border-2 border-blue-400 border-t-transparent rounded-full animate-spin"></div>
        <span className="text-xs text-blue-400 font-medium">{label} 分析中</span>
      </div>
    )
  }
  
  return (
    <div className="flex items-center gap-2">
      <svg className="w-4 h-4 text-emerald-400" fill="none" viewBox="0 0 24 24" stroke="currentColor">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 13l4 4L19 7" />
      </svg>
      <span className="text-xs text-emerald-400 font-medium">{label} 完成</span>
    </div>
  )
}

function PhaseProgressBar({ 
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

  return (
    <div className="flex flex-col gap-1">
      <StatusIndicator status={phase.status} label={label} />
      
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
  const phase1 = usePhase1Progress()
  const phase2 = usePhase2Progress()

  if (error) {
    return (
      <div className="flex items-center gap-2 px-4 py-2 rounded-xl bg-red-500/10 border border-red-500/30">
        <svg className="w-4 h-4 text-red-400" fill="none" viewBox="0 0 24 24" stroke="currentColor">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
        </svg>
        <span className="text-xs text-red-400">连接失败</span>
      </div>
    )
  }

  if (isLoading || !progress) {
    return (
      <div className="flex items-center gap-2 px-4 py-2 rounded-xl bg-gray-800 border border-gray-700">
        <div className="w-4 h-4 border-2 border-gray-500 border-t-gray-300 rounded-full animate-spin"></div>
        <span className="text-xs text-gray-400">加载中...</span>
      </div>
    )
  }

  const bothIdle = phase1.status === 'idle' && phase2.status === 'idle'
  const bothCompleted = phase1.status === 'completed' && phase2.status === 'completed'

  if (bothIdle) {
    return (
      <div className="flex flex-col gap-2 px-4 py-3 rounded-xl bg-gray-800 border border-gray-700">
        <div className="flex items-center gap-2">
          <div className="w-2 h-2 rounded-full bg-gray-400"></div>
          <span className="text-xs text-gray-500 font-medium">等待分析</span>
        </div>
      </div>
    )
  }

  if (bothCompleted) {
    return (
      <div className="flex flex-col gap-2 px-4 py-3 rounded-xl bg-emerald-500/10 border border-emerald-500/30">
        <div className="flex items-center gap-2">
          <svg className="w-4 h-4 text-emerald-400" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
          </svg>
          <span className="text-xs text-emerald-400 font-medium">分析完成</span>
        </div>
        <div className="flex items-center gap-4 text-xs text-gray-400">
          <span>基本面: {phase1.qualified || 0} 只优质股</span>
          <span>技术面: {phase2.completed} 项完成</span>
        </div>
      </div>
    )
  }

  return (
    <div className="flex flex-col gap-3 px-4 py-3 rounded-xl bg-gray-800 border border-gray-700">
      <PhaseProgressBar 
        phase={phase1} 
        label="基本面" 
        showQualified 
      />
      <div className="w-full h-px bg-gray-700"></div>
      <PhaseProgressBar 
        phase={phase2} 
        label="技术面" 
      />
    </div>
  )
}