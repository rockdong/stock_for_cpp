import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import { useState, useEffect } from 'react';
import AnalysisPage from './pages/Analysis';
import LoginPage from './pages/Login';
import RegisterPage from './pages/Register';
import AdminLoginPage from './pages/AdminLogin';
import { tokenStorage } from './utils/tokenStorage';
import axios from 'axios';

const api = axios.create({
  baseURL: '/api',
  timeout: 10000,
});

function App() {
  const [authRequired, setAuthRequired] = useState<boolean | null>(null);
  const isLoggedIn = tokenStorage.exists();

  useEffect(() => {
    const fetchAuthConfig = async () => {
      try {
        const response = await api.get('/auth/config');
        setAuthRequired(response.data.data.authRequired);
      } catch {
        setAuthRequired(true);
      }
    };
    fetchAuthConfig();
  }, []);

  if (authRequired === null) {
    return (
      <div className="min-h-screen bg-gray-50 flex items-center justify-center">
        <div className="text-gray-500">加载中...</div>
      </div>
    );
  }

  const canAccessAnalysis = !authRequired || isLoggedIn;

  return (
    <BrowserRouter>
      <Routes>
        <Route 
          path="/login" 
          element={canAccessAnalysis ? <Navigate to="/analysis" /> : <LoginPage />} 
        />
        <Route 
          path="/register" 
          element={canAccessAnalysis ? <Navigate to="/analysis" /> : <RegisterPage />} 
        />
        <Route 
          path="/admin-login" 
          element={canAccessAnalysis ? <Navigate to="/analysis" /> : <AdminLoginPage />} 
        />
        <Route 
          path="/analysis" 
          element={canAccessAnalysis ? <AnalysisPage /> : <Navigate to="/login" />} 
        />
        <Route 
          path="/" 
          element={<Navigate to={canAccessAnalysis ? "/analysis" : "/login"} />} 
        />
      </Routes>
    </BrowserRouter>
  );
}

export default App;