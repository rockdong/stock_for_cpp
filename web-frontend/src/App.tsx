import TabNav from './components/Navigation/TabNav'
import AnalysisPage from './pages/Analysis'
import { ProgressProvider } from './contexts/ProgressContext'

function App() {
  return (
    <ProgressProvider>
      <div className="min-h-screen bg-primary">
        <div className="max-w-7xl mx-auto p-6">
          <TabNav />
          <AnalysisPage />
        </div>
      </div>
    </ProgressProvider>
  )
}

export default App