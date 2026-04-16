import { useState } from 'react'
import TabNav from './components/Navigation/TabNav'
import AnalysisPage from './pages/Analysis'
import FundamentalPage from './pages/Fundamental'

function App() {
  const [activeTab, setActiveTab] = useState<'analysis' | 'fundamental'>('analysis')

  return (
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
  )
}

export default App