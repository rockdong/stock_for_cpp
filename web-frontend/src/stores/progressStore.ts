import { create } from 'zustand'
import { analysisApi } from '../services/api'
import type { AnalysisProgress, PhaseProgress } from '../types/analysis'

interface ProgressState {
  progress: AnalysisProgress
  isLoading: boolean
  error: boolean
  fetchProgress: () => Promise<void>
}

const DEFAULT_PHASE: PhaseProgress = {
  status: 'idle',
  total: 0,
  completed: 0,
  qualified: 0,
  failed: 0
}

const DEFAULT_PROGRESS: AnalysisProgress = {
  id: 1,
  phase1: DEFAULT_PHASE,
  phase2: DEFAULT_PHASE,
  started_at: null,
  phase1_completed_at: null,
  updated_at: '',
  execute_time: '20:00'
}

export const useProgressStore = create<ProgressState>((set) => ({
  progress: DEFAULT_PROGRESS,
  isLoading: true,
  error: false,

  fetchProgress: async () => {
    set({ isLoading: true, error: false })
    try {
      const data = await analysisApi.getProgress()
      set({ progress: data, isLoading: false })
    } catch {
      set({ error: true, isLoading: false })
    }
  }
}))