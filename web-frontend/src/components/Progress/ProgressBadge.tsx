import { useState, useEffect } from 'react'
import { analysisApi } from '../../services/api'
import { AnalysisProgress, ProgressStatus } from '../../types/analysis'

interface ProgressBadgeProps {
  refreshInterval?: number
}

export default function ProgressBadge({ refreshInterval = 5000 }: ProgressBadgeProps) {
  const [progress, setProgress] = useState<AnalysisProgress | null>(null)

  useEffect(() => {
    const fetchProgress = async () => {
      try {
        const data = await analysisApi.getProgress()
        setProgress(data)
      } catch (error) {
        console.error('获取进度失败:', error)
      }
    }

    fetchProgress()
    const interval = setInterval(fetchProgress, refreshInterval)
    return () => clearInterval(interval)
  }, [refreshInterval])

  if (!progress) {
    return null
  }

  const getStatusStyle = (status: ProgressStatus) => {
    const styles: Record<ProgressStatus, string> = {
      idle: 'bg-gray-100 text-gray-600',
      running: 'bg-blue-100 text-blue-800',
      completed: 'bg-green-100 text-green-800',
    }
    return styles[status] || styles.idle
  }

  const getStatusLabel = (status: ProgressStatus) => {
    const labels: Record<ProgressStatus, string> = {
      idle: '空闲',
      running: '分析中',
      completed: '已完成',
    }
    return labels[status] || status
  }

  const percentage = progress.total > 0 
    ? Math.round((progress.completed / progress.total) * 100) 
    : 0

  if (progress.status === 'idle') {
    return (
      <div className="flex items-center gap-2">
        <span className={`px-2 py-1 rounded text-xs font-medium ${getStatusStyle(progress.status)}`}>
          {getStatusLabel(progress.status)}
        </span>
      </div>
    )
  }

  return (
    <div className="flex items-center gap-3">
      <span className={`px-2 py-1 rounded text-xs font-medium ${getStatusStyle(progress.status)}`}>
        {getStatusLabel(progress.status)}
      </span>
      
      {progress.status === 'running' && (
        <div className="flex items-center gap-2">
          <div className="w-32 h-2 bg-gray-200 rounded-full overflow-hidden">
            <div 
              className="h-full bg-blue-500 transition-all duration-300"
              style={{ width: `${percentage}%` }}
            />
          </div>
          <span className="text-xs text-gray-600">
            {progress.completed}/{progress.total} ({percentage}%)
          </span>
        </div>
      )}

      {progress.status === 'completed' && (
        <div className="flex items-center gap-2 text-xs text-gray-600">
          <span>完成 {progress.completed} 项</span>
          {progress.failed > 0 && (
            <span className="text-red-600">失败 {progress.failed} 项</span>
          )}
        </div>
      )}
    </div>
  )
}