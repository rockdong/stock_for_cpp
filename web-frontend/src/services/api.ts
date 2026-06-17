import axios from 'axios'
import { AnalysisProcessRecord, ChartResponse, FilterParams, AnalysisProgress } from '../types/analysis'

const api = axios.create({
  baseURL: '/api',
  timeout: 60000,
})

class ApiError extends Error {
  constructor(public code: string, message: string) {
    super(message)
    this.name = 'ApiError'
  }
}

// 统一响应拦截：处理后端返回 { success: false } 的情况
api.interceptors.response.use(
  (response) => {
    const data = response.data
    if (data && typeof data.success === 'boolean' && !data.success) {
      return Promise.reject(new ApiError('API_ERROR', data.error || '请求失败'))
    }
    return response
  },
  (error) => {
    const msg = error?.response?.data?.message || error?.response?.data?.error || error.message || '网络异常'
    return Promise.reject(new ApiError('NETWORK_ERROR', msg))
  }
)

export { ApiError }

export const analysisApi = {
  getProcessRecords: async (params: FilterParams): Promise<AnalysisProcessRecord[]> => {
    const response = await api.get('/analysis/process', { 
      params,
      timeout: 120000
    })
    return response.data.data
  },

  getProcessRecord: async (id: number): Promise<AnalysisProcessRecord> => {
    const response = await api.get(`/analysis/process/${id}`)
    return response.data.data
  },

  getChartData: async (tsCode: string, strategy?: string, freq?: string): Promise<ChartResponse> => {
    const response = await api.get(`/analysis/process/chart/${tsCode}`, {
      params: { strategy, freq }
    })
    return response.data
  },

  getStrategies: async (): Promise<string[]> => {
    const response = await api.get('/analysis/strategies/config')
    return response.data.data
  },

  getProgress: async (): Promise<AnalysisProgress> => {
    const response = await api.get('/analysis/progress')
    return response.data.data
  }
}