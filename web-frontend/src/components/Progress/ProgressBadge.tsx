import { useState, useEffect } from 'react'
import { analysisApi } from '../../services/api'
import { AnalysisProgress } from '../../types/analysis'

interface ProgressBadgeProps {
  refreshInterval?: number
}

export default function ProgressBadge({ refreshInterval = 5000 }: ProgressBadgeProps) {
  const [progress, setProgress] = useState<AnalysisProgress | null>(null)
  const [error, setError] = useState(false)

  useEffect(() => {
    const fetchProgress = async () => {
      try {
        const data = await analysisApi.getProgress()
        setProgress({
          id: data.id || 1,
          total: data.total || 0,
          completed: data.completed || 0,
          failed: data.failed || 0,
          status: data.status || 'idle',
          started_at: data.started_at || null,
          updated_at: data.updated_at || new Date().toISOString(),
        })
        setError(false)
      } catch (err) {
        console.error('获取进度失败:', err)
        setError(true)
      }
    }

    fetchProgress()
    const interval = setInterval(fetchProgress, refreshInterval)
    return () => clearInterval(interval)
  }, [refreshInterval])

  if (error) {
    return (
      <div className="flex items-center gap-2 px-4 py-2 rounded-xl bg-sell/10 border border-sell/30">
        <svg className="w-4 h-4 text-sell" fill="none" viewBox="0 0 24 24" stroke="currentColor">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
        </svg>
        <span className="text-xs text-sell">连接失败</span>
      </div>
    )
  }

  if (!progress) {
    return (
      <div className="flex items-center gap-2 px-4 py-2 rounded-xl bg-primary border border-border">
        <div className="w-4 h-4 border-2 border-muted border-t-cta rounded-full animate-spin"></div>
        <span className="text-xs text-muted">加载中...</span>
      </div>
    )
  }

  const percentage = progress.total > 0 
    ? Math.round((progress.completed / progress.total) * 100) 
    : 0

  if (progress.status === 'idle') {
    return (
      <div className="flex items-center gap-2 px-4 py-2 rounded-xl bg-primary border border-border">
        <div className="w-2 h-2 rounded-full bg-muted"></div>
        <span className="text-xs text-muted font-medium">等待分析</span>
      </div>
    )
  }

  if (progress.status === 'running') {
    return (
      <div className="flex flex-col items-end gap-2">
        <div className="flex items-center gap-2 px-4 py-2 rounded-xl bg-blue-500/10 border border-blue-500/30">
          <div className="w-4 h-4 border-2 border-blue-400 border-t-transparent rounded-full animate-spin"></div>
          <span className="text-xs text-blue-400 font-medium">分析中</span>
        </div>
        <div className="flex items-center gap-3">
          <div className="w-40 h-2 bg-primary rounded-full overflow-hidden">
            <div 
              className="h-full bg-gradient-to-r from-blue-500 to-cta transition-all duration-500 rounded-full"
              style={{ width: `${percentage}%` }}
            />
          </div>
          <span className="text-xs text-muted font-mono">
            {progress.completed}/{progress.total}
          </span>
        </div>
      </div>
    )
  }

  return (
    <div className="flex flex-col items-end gap-1">
      <div className="flex items-center gap-2 px-4 py-2 rounded-xl bg-cta/10 border border-cta/30">
        <svg className="w-4 h-4 text-cta" fill="none" viewBox="0 0 24 24" stroke="currentColor">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 13l4 4L19 7" />
        </svg>
        <span className="text-xs text-cta font-medium">分析完成</span>
      </div>
      <div className="flex items-center gap-2 text-xs">
        <span className="text-muted">共 {progress.completed} 项</span>
        {progress.failed > 0 && (
          <span className="text-sell">失败 {progress.failed} 项</span>
        )}
      </div>
    </div>
  )
}