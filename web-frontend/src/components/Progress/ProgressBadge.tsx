import { useProgress } from '../../contexts/ProgressContext'

export default function ProgressBadge() {
  const { progress, isLoading, error } = useProgress()
  
  const phase1 = progress.phase1
  const phase2 = progress.phase2
  const executeTime = progress.execute_time
  
  const p1Percent = phase1.total > 0 ? Math.round((phase1.completed / phase1.total) * 100) : 0
  const p2Percent = phase2.total > 0 ? Math.round((phase2.completed / phase2.total) * 100) : 0
  
  const isIdle = phase1.status === 'idle' && phase2.status === 'idle'
  const isCompleted = phase1.status === 'completed' && phase2.status === 'completed'
  
  if (error) {
    return (
      <span className="text-xs text-red-400 flex items-center gap-1">
        <span className="w-1.5 h-1.5 rounded-full bg-red-400" />
        连接失败
      </span>
    )
  }
  
  if (isLoading) {
    return (
      <span className="text-xs text-gray-400 flex items-center gap-1">
        <span className="w-1.5 h-1.5 rounded-full bg-gray-400 animate-pulse" />
        加载中
      </span>
    )
  }
  
  if (isIdle) {
    return (
      <span className="text-xs text-gray-400 flex items-center gap-2">
        <span className="w-1.5 h-1.5 rounded-full bg-gray-400" />
        等待分析
        <span className="text-gray-500">{executeTime}</span>
      </span>
    )
  }
  
  if (isCompleted) {
    return (
      <span className="text-xs text-emerald-400 flex items-center gap-1">
        <span className="w-1.5 h-1.5 rounded-full bg-emerald-400" />
        分析完成
      </span>
    )
  }
  
  return (
    <div className="flex items-center gap-4 text-xs">
      {phase1.status !== 'idle' && (
        <div className="flex items-center gap-2">
          <span className={`w-1.5 h-1.5 rounded-full ${phase1.status === 'running' ? 'bg-blue-400' : 'bg-emerald-400'}`} />
          <span className="text-gray-400">基本面</span>
          {phase1.status === 'running' && (
            <div className="flex items-center gap-1.5">
              <div className="w-16 h-1 bg-gray-700 rounded-full overflow-hidden">
                <div className="h-full bg-blue-400 rounded-full" style={{ width: `${p1Percent}%` }} />
              </div>
              <span className="text-gray-500 font-mono">{p1Percent}%</span>
            </div>
          )}
          {phase1.status === 'completed' && (
            <span className="text-emerald-400">✓</span>
          )}
        </div>
      )}
      
      {phase1.status !== 'idle' && phase2.status !== 'idle' && (
        <span className="text-gray-600">|</span>
      )}
      
      {phase2.status !== 'idle' && (
        <div className="flex items-center gap-2">
          <span className={`w-1.5 h-1.5 rounded-full ${phase2.status === 'running' ? 'bg-blue-400' : 'bg-emerald-400'}`} />
          <span className="text-gray-400">技术面</span>
          {phase2.status === 'running' && (
            <div className="flex items-center gap-1.5">
              <div className="w-16 h-1 bg-gray-700 rounded-full overflow-hidden">
                <div className="h-full bg-blue-400 rounded-full" style={{ width: `${p2Percent}%` }} />
              </div>
              <span className="text-gray-500 font-mono">{p2Percent}%</span>
            </div>
          )}
          {phase2.status === 'completed' && (
            <span className="text-emerald-400">✓</span>
          )}
        </div>
      )}
    </div>
  )
}