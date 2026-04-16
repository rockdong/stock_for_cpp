import axios from 'axios'
import { 
  FundamentalResult, 
  FundamentalScore, 
  FundamentalProgress,
  FundamentalFilterParams 
} from '../types/fundamental'

const api = axios.create({
  baseURL: '/api',
  timeout: 60000,
})

export const fundamentalApi = {
  filter: async (params: FundamentalFilterParams): Promise<FundamentalResult> => {
    const response = await api.get('/fundamentals/filter', { params })
    return response.data
  },

  getDetail: async (code: string): Promise<FundamentalScore> => {
    const response = await api.get(`/fundamentals/${code}`)
    return response.data.data
  },

  getProgress: async (): Promise<FundamentalProgress> => {
    const response = await api.get('/fundamentals/progress')
    return response.data.data
  }
}