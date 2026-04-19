import AnalysisPage from './pages/Analysis'
import { ProgressProvider } from './contexts/ProgressContext'

function App() {
  return (
    <ProgressProvider>
      <AnalysisPage />
    </ProgressProvider>
  )
}

export default App