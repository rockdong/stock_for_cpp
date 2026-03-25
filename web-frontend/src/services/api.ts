import axios from 'axios'
import { AnalysisProcessRecord, ChartDataPoint, FilterParams } from '../types/analysis'

const api = axios.create({
  baseURL: '/api',
  timeout: 10000,
})

export const analysisApi = {
  getProcessRecords: async (params: FilterParams): Promise<AnalysisProcessRecord[]> => {
    const response = await api.get('/analysis/process', { params })
    return response.data.data
  },

  getProcessRecord: async (id: number): Promise<AnalysisProcessRecord> => {
    const response = await api.get(`/analysis/process/${id}`)
    return response.data.data
  },

  getChartData: async (tsCode: string, strategy?: string, freq?: string): Promise<ChartDataPoint[]> => {
    const response = await api.get(`/analysis/process/chart/${tsCode}`, {
      params: { strategy, freq }
    })
    return response.data.data
  },

  getStrategies: async (): Promise<string[]> => {
    const response = await api.get('/analysis/process/strategies')
    return response.data.data
  }
}