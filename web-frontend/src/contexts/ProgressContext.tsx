import { createContext, useContext, useState, useEffect, useCallback, ReactNode } from 'react'
import { analysisApi } from '../services/api'
import { AnalysisProgress, ProgressStatus } from '../types/analysis'

interface ProgressContextValue {
  progress: AnalysisProgress | null
  isLoading: boolean
  error: boolean
  status: ProgressStatus
  refresh: () => Promise<void>
}

const ProgressContext = createContext<ProgressContextValue | null>(null)

const DEFAULT_PROGRESS: AnalysisProgress = {
  id: 1,
  phase1: { status: 'idle', total: 0, completed: 0, qualified: 0 },
  phase2: { status: 'idle', total: 0, completed: 0, failed: 0 },
  started_at: null,
  phase1_completed_at: null,
  updated_at: ''
}

export function ProgressProvider({ 
  children, 
  refreshInterval = 5000 
}: { 
  children: ReactNode
  refreshInterval?: number 
}) {
  const [progress, setProgress] = useState<AnalysisProgress | null>(null)
  const [isLoading, setIsLoading] = useState(true)
  const [error, setError] = useState(false)

  const fetchProgress = useCallback(async () => {
    try {
      const data = await analysisApi.getProgress()
      setProgress(data)
      setError(false)
    } catch (err) {
      console.error('获取进度失败:', err)
      setError(true)
    } finally {
      setIsLoading(false)
    }
  }, [])

  useEffect(() => {
    fetchProgress()
    const interval = setInterval(fetchProgress, refreshInterval)
    return () => clearInterval(interval)
  }, [fetchProgress, refreshInterval])

  const getOverallStatus = (): ProgressStatus => {
    if (!progress) return 'idle'
    
    const p1 = progress.phase1.status
    const p2 = progress.phase2.status
    
    if (p1 === 'running' || p2 === 'running') return 'running'
    if (p1 === 'completed' && p2 === 'completed') return 'completed'
    return 'idle'
  }

  const value: ProgressContextValue = {
    progress: progress || DEFAULT_PROGRESS,
    isLoading,
    error,
    status: getOverallStatus(),
    refresh: fetchProgress
  }

  return (
    <ProgressContext.Provider value={value}>
      {children}
    </ProgressContext.Provider>
  )
}

export function useProgress() {
  const context = useContext(ProgressContext)
  if (!context) {
    throw new Error('useProgress must be used within ProgressProvider')
  }
  return context
}

export function useProgressStatus() {
  const { status, isLoading, error } = useProgress()
  return { status, isLoading, error }
}

export function usePhase1Progress() {
  const { progress } = useProgress()
  return progress?.phase1 || DEFAULT_PROGRESS.phase1
}

export function usePhase2Progress() {
  const { progress } = useProgress()
  return progress?.phase2 || DEFAULT_PROGRESS.phase2
}