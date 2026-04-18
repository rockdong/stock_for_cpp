import { createContext, useContext, useState, useEffect, useCallback, ReactNode } from 'react'
import { analysisApi } from '../services/api'
import { AnalysisProgress, ProgressStatus, PhaseProgress } from '../types/analysis'

interface ProgressContextValue {
  progress: AnalysisProgress
  isLoading: boolean
  error: boolean
  status: ProgressStatus
  refresh: () => Promise<void>
}

const ProgressContext = createContext<ProgressContextValue | null>(null)

const DEFAULT_PHASE1: PhaseProgress = { status: 'idle', total: 0, completed: 0, qualified: 0 }
const DEFAULT_PHASE2: PhaseProgress = { status: 'idle', total: 0, completed: 0, failed: 0 }

const DEFAULT_PROGRESS: AnalysisProgress = {
  id: 1,
  phase1: DEFAULT_PHASE1,
  phase2: DEFAULT_PHASE2,
  started_at: null,
  phase1_completed_at: null,
  updated_at: ''
}

function normalizeProgress(data: any): AnalysisProgress {
  if (!data) return DEFAULT_PROGRESS
  
  if (data.phase1 && data.phase2) {
    return {
      id: data.id || 1,
      phase1: {
        status: data.phase1.status || 'idle',
        total: data.phase1.total || 0,
        completed: data.phase1.completed || 0,
        qualified: data.phase1.qualified || 0
      },
      phase2: {
        status: data.phase2.status || 'idle',
        total: data.phase2.total || 0,
        completed: data.phase2.completed || 0,
        failed: data.phase2.failed || 0
      },
      started_at: data.started_at || null,
      phase1_completed_at: data.phase1_completed_at || null,
      updated_at: data.updated_at || ''
    }
  }
  
  if (data.status) {
    return {
      id: data.id || 1,
      phase1: {
        status: data.status === 'completed' ? 'completed' : 'idle',
        total: 0,
        completed: 0,
        qualified: 0
      },
      phase2: {
        status: data.status || 'idle',
        total: data.total || 0,
        completed: data.completed || 0,
        failed: data.failed || 0
      },
      started_at: data.started_at || null,
      phase1_completed_at: null,
      updated_at: data.updated_at || ''
    }
  }
  
  return DEFAULT_PROGRESS
}

export function ProgressProvider({ 
  children, 
  refreshInterval = 5000 
}: { 
  children: ReactNode
  refreshInterval?: number 
}) {
  const [progress, setProgress] = useState<AnalysisProgress>(DEFAULT_PROGRESS)
  const [isLoading, setIsLoading] = useState(true)
  const [error, setError] = useState(false)

  const fetchProgress = useCallback(async () => {
    try {
      const data = await analysisApi.getProgress()
      setProgress(normalizeProgress(data))
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
    const p1 = progress.phase1.status
    const p2 = progress.phase2.status
    
    if (p1 === 'running' || p2 === 'running') return 'running'
    if (p1 === 'completed' && p2 === 'completed') return 'completed'
    return 'idle'
  }

  const value: ProgressContextValue = {
    progress,
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
  return progress.phase1
}

export function usePhase2Progress() {
  const { progress } = useProgress()
  return progress.phase2
}