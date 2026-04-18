import { useState } from 'react'
import TabNav from './components/Navigation/TabNav'
import AnalysisPage from './pages/Analysis'
import FundamentalPage from './pages/Fundamental'
import { ProgressProvider } from './contexts/ProgressContext'

function App() {
  const [activeTab, setActiveTab] = useState<'analysis' | 'fundamental'>('analysis')

  return (
    <ProgressProvider>
      <div className="min-h-screen bg-primary">
        <div className="max-w-7xl mx-auto p-6">
          <TabNav 
            defaultTab={activeTab}
            onTabChange={setActiveTab}
          />
          
          {activeTab === 'analysis' && <AnalysisPage />}
          {activeTab === 'fundamental' && <FundamentalPage />}
        </div>
      </div>
    </ProgressProvider>
  )
}

export default App