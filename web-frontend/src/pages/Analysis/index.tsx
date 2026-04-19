import { useState, useEffect } from 'react'
import Sidebar from '../../components/Navigation/Sidebar'
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
  const [activeNav, setActiveNav] = useState('analysis')
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
    <div className="min-h-screen bg-base flex">
      {/* 侧边栏 */}
      <Sidebar activeId={activeNav} onNavChange={setActiveNav} />

      {/* 主内容区 */}
      <div className="flex-1 ml-60 flex flex-col min-h-screen">
        {/* 顶部栏 */}
        <header className="sticky top-0 z-20 h-14 bg-surface/80 backdrop-blur-xl border-b border-border-default flex items-center justify-between px-6">
          <div className="flex items-center gap-4">
            <h2 className="text-base font-semibold text-text-primary">个股分析</h2>
            <div className="h-4 w-px bg-border-default" />
            <ProgressBadge />
          </div>
          <div className="flex items-center gap-3">
            <span className="text-xs text-text-tertiary tabular-nums">
              {new Date().toLocaleDateString('zh-CN', { year: 'numeric', month: 'long', day: 'numeric' })}
            </span>
          </div>
        </header>

        {/* 内容区 */}
        <main className="flex-1 p-6 space-y-5">
          {/* 统计概览 */}
          <div className="grid grid-cols-4 gap-4">
            <StatCard
              label="分析标的"
              value={String(records.length)}
              icon={
                <svg className="w-4 h-4 text-accent-blue" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
                  <path strokeLinecap="round" strokeLinejoin="round" d="M3 13.125C3 12.504 3.504 12 4.125 12h2.25c.621 0 1.125.504 1.125 1.125v6.75C7.5 20.496 6.996 21 6.375 21h-2.25A1.125 1.125 0 013 19.875v-6.75z" />
                </svg>
              }
              accent="blue"
            />
            <StatCard
              label="买入信号"
              value={String(records.filter(r => {
                const strategies = r.data?.strategies || []
                return strategies.some(s => s.freqs.some(f => f.signal === 'BUY'))
              }).length)}
              icon={
                <svg className="w-4 h-4 text-signal-buy" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
                  <path strokeLinecap="round" strokeLinejoin="round" d="M4.5 19.5l15-15m0 0H8.25m11.25 0v11.25" />
                </svg>
              }
              accent="red"
            />
            <StatCard
              label="卖出信号"
              value={String(records.filter(r => {
                const strategies = r.data?.strategies || []
                return strategies.some(s => s.freqs.some(f => f.signal === 'SELL'))
              }).length)}
              icon={
                <svg className="w-4 h-4 text-signal-sell" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
                  <path strokeLinecap="round" strokeLinejoin="round" d="M4.5 4.5l15 15m0 0V8.25m0 11.25H8.25" />
                </svg>
              }
              accent="green"
            />
            <StatCard
              label="待观察"
              value={String(records.filter(r => {
                const strategies = r.data?.strategies || []
                return strategies.every(s => s.freqs.every(f => f.signal === 'HOLD' || f.signal === 'NONE'))
              }).length)}
              icon={
                <svg className="w-4 h-4 text-accent-amber" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
                  <path strokeLinecap="round" strokeLinejoin="round" d="M2.036 12.322a1.012 1.012 0 010-.639C3.423 7.51 7.36 4.5 12 4.5c4.638 0 8.573 3.007 9.963 7.178.07.207.07.431 0 .639C20.577 16.49 16.64 19.5 12 19.5c-4.638 0-8.573-3.007-9.963-7.178z" />
                  <path strokeLinecap="round" strokeLinejoin="round" d="M15 12a3 3 0 11-6 0 3 3 0 016 0z" />
                </svg>
              }
              accent="amber"
            />
          </div>

          {/* 筛选器 */}
          <AnalysisFilter onFilter={loadRecords} />

          {/* 卡片列表 */}
          {loading ? (
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
              {[1, 2, 3, 4, 5, 6].map(i => (
                <div key={i} className="card animate-pulse">
                  <div className="skeleton h-5 w-24 mb-3" />
                  <div className="skeleton h-4 w-16 mb-4" />
                  <div className="grid grid-cols-2 gap-3">
                    <div className="skeleton h-4 w-full" />
                    <div className="skeleton h-4 w-full" />
                    <div className="skeleton h-4 w-full" />
                    <div className="skeleton h-4 w-full" />
                  </div>
                  <div className="flex gap-2 mt-4">
                    <div className="skeleton h-6 w-16" />
                    <div className="skeleton h-6 w-16" />
                  </div>
                </div>
              ))}
            </div>
          ) : (
            <StockCardGrid records={records} onFreqClick={handleFreqClick} />
          )}
        </main>
      </div>

      {/* 图表弹窗 */}
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

/* 统计卡片子组件 */
interface StatCardProps {
  label: string
  value: string
  icon: React.ReactNode
  accent: 'blue' | 'red' | 'green' | 'amber'
}

function StatCard({ label, value, icon, accent }: StatCardProps) {
  const accentMap = {
    blue: 'bg-accent-blue-bg text-accent-blue shadow-glow-blue',
    red: 'bg-signal-buy-bg text-signal-buy shadow-glow-red',
    green: 'bg-signal-sell-bg text-signal-sell shadow-glow-green',
    amber: 'bg-amber-500/10 text-accent-amber',
  }

  return (
    <div className="bg-surface rounded-xl border border-border-default p-4 flex items-center gap-4">
      <div className={`w-10 h-10 rounded-lg flex items-center justify-center ${accentMap[accent]}`}>
        {icon}
      </div>
      <div>
        <div className="text-2xl font-semibold text-text-primary tabular-nums">{value}</div>
        <div className="text-xs text-text-tertiary">{label}</div>
      </div>
    </div>
  )
}
