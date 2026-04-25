import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import AnalysisPage from './pages/Analysis';
import LoginPage from './pages/Login';
import { tokenStorage } from './utils/tokenStorage';

function App() {
  const isLoggedIn = tokenStorage.exists();

  return (
    <BrowserRouter>
      <Routes>
        <Route 
          path="/login" 
          element={isLoggedIn ? <Navigate to="/analysis" /> : <LoginPage />} 
        />
        <Route 
          path="/analysis" 
          element={isLoggedIn ? <AnalysisPage /> : <Navigate to="/login" />} 
        />
        <Route 
          path="/" 
          element={<Navigate to={isLoggedIn ? "/analysis" : "/login"} />} 
        />
      </Routes>
    </BrowserRouter>
  );
}

export default App;