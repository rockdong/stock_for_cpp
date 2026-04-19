import { useState, useEffect } from 'react'
import AnalysisFilter from '../../components/Filter/AnalysisFilter'
import StockCardGrid from '../../components/Card/StockCardGrid'
import ChartModal from '../../components/Modal/ChartModal'
import ProgressBadge from '../../components/Progress/ProgressBadge'
import { analysisApi } from '../../services/api'
import { FilterParams, AnalysisProcessRecord, FreqType } from '../../types/analysis'

interface ModalState {
  visible: boolean
  record: AnalysisProcessRecord | null
  freq: FreqType
  strategy: string
}

export default function AnalysisPage() {
  const [records, setRecords] = useState<AnalysisProcessRecord[]>([])
  const [loading, setLoading] = useState(false)
  const [modalState, setModalState] = useState<ModalState>({
    visible: false,
    record: null,
    freq: 'd',
    strategy: '',
  })

  useEffect(() => {
    loadRecords({})
  }, [])

  const loadRecords = async (params: FilterParams) => {
    setLoading(true)
    try {
      const data = await analysisApi.getProcessRecords(params)
      setRecords(data)
    } catch (error) {
      console.error('加载记录失败:', error)
    } finally {
      setLoading(false)
    }
  }

  const handleFreqClick = (record: AnalysisProcessRecord, freq: FreqType, strategy: string) => {
    setModalState({
      visible: true,
      record,
      freq,
      strategy,
    })
  }

  const handleCloseModal = () => {
    setModalState(prev => ({ ...prev, visible: false }))
  }

  const handleFreqChange = (freq: FreqType) => {
    setModalState(prev => ({ ...prev, freq }))
  }

  const handleStrategyChange = (strategy: string) => {
    setModalState(prev => ({ ...prev, strategy }))
  }

  return (
    <div className="min-h-screen bg-gray-100">
      <header className="sticky top-0 z-40 bg-white/80 backdrop-blur-xl border-b border-gray-200">
        <div className="max-w-7xl mx-auto px-4 py-4 sm:px-6 lg:px-8">
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-xl bg-gradient-to-br from-blue-500 to-emerald-500 flex items-center justify-center">
              <svg className="w-6 h-6 text-white" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 7h8m0 0v8m0-8l-8 8-4-4-6 6" />
              </svg>
            </div>
            <div>
              <h1 className="text-xl font-semibold text-gray-900">Stock Analysis</h1>
              <p className="text-xs text-gray-500">股票分析过程数据展示</p>
            </div>
          </div>
          <div className="mt-3">
            <ProgressBadge />
          </div>
        </div>
      </header>

      <main className="max-w-7xl mx-auto px-4 py-6 sm:px-6 lg:px-8">
        <AnalysisFilter onFilter={loadRecords} />

        {loading ? (
          <div className="bg-white rounded-2xl p-8 text-center border border-gray-200">
            <div className="animate-pulse flex flex-col items-center gap-3">
              <div className="w-12 h-12 rounded-full bg-gray-200"></div>
              <p className="text-gray-500">加载中...</p>
            </div>
          </div>
        ) : (
          <StockCardGrid records={records} onFreqClick={handleFreqClick} />
        )}
      </main>

      <ChartModal
        visible={modalState.visible}
        record={modalState.record}
        freq={modalState.freq}
        strategy={modalState.strategy}
        onClose={handleCloseModal}
        onFreqChange={handleFreqChange}
        onStrategyChange={handleStrategyChange}
      />
    </div>
  )
}