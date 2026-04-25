import { useState, useEffect, useCallback } from 'react';
import { useNavigate } from 'react-router-dom';
import { authApi } from '../../services/authApi';
import { tokenStorage } from '../../utils/tokenStorage';

export default function LoginPage() {
  const navigate = useNavigate();
  const [qrUrl, setQrUrl] = useState<string>('');
  const [sessionId, setSessionId] = useState<string>('');
  const [status, setStatus] = useState<'loading' | 'pending' | 'success' | 'expired' | 'error'>('loading');
  const [countdown, setCountdown] = useState<number>(300);

  const generateQRCode = useCallback(async () => {
    setStatus('loading');
    try {
      const result = await authApi.getQRCode();
      setQrUrl(result.qr_url);
      setSessionId(result.session_id);
      setCountdown(result.expires_in);
      setStatus('pending');
    } catch (error) {
      console.error('生成二维码失败:', error);
      setStatus('error');
    }
  }, []);

  useEffect(() => {
    if (status !== 'pending' || !sessionId) return;

    const interval = setInterval(async () => {
      try {
        const result = await authApi.getStatus(sessionId);
        
        if (result.status === 'success') {
          setStatus('success');
          clearInterval(interval);
          
          const tokenResult = await authApi.getToken(sessionId);
          tokenStorage.save(tokenResult.token);
          
          navigate('/analysis');
        } else if (result.status === 'expired') {
          setStatus('expired');
          clearInterval(interval);
        }
      } catch (error) {
        console.error('轮询状态失败:', error);
      }
    }, 2000);

    return () => clearInterval(interval);
  }, [status, sessionId, navigate]);

  useEffect(() => {
    if (status !== 'pending') return;

    const interval = setInterval(() => {
      setCountdown(prev => {
        if (prev <= 1) {
          setStatus('expired');
          return 0;
        }
        return prev - 1;
      });
    }, 1000);

    return () => clearInterval(interval);
  }, [status]);

  useEffect(() => {
    generateQRCode();
  }, [generateQRCode]);

  return (
    <div className="min-h-screen bg-base flex items-center justify-center">
      <div className="bg-surface rounded-xl border border-border-default p-8 max-w-md w-full text-center">
        <h1 className="text-2xl font-semibold text-text-primary mb-2">
          微信扫码登录
        </h1>
        <p className="text-text-tertiary mb-6">
          扫码关注公众号即可登录
        </p>

        {status === 'loading' && (
          <div className="text-text-tertiary">正在生成二维码...</div>
        )}

        {status === 'pending' && qrUrl && (
          <>
            <img 
              src={qrUrl} 
              alt="微信登录二维码" 
              className="w-64 h-64 mx-auto mb-4 rounded-lg"
            />
            <p className="text-text-tertiary text-sm">
              请使用微信扫描二维码
            </p>
            <p className="text-accent-amber text-sm mt-2">
              二维码有效期: {Math.floor(countdown / 60)}:{(countdown % 60).toString().padStart(2, '0')}
            </p>
          </>
        )}

        {status === 'success' && (
          <div className="text-signal-sell">
            <span className="text-4xl mb-4">✓</span>
            <p>登录成功，正在跳转...</p>
          </div>
        )}

        {status === 'expired' && (
          <>
            <p className="text-signal-buy mb-4">二维码已过期</p>
            <button 
              onClick={generateQRCode}
              className="btn-primary"
            >
              刷新二维码
            </button>
          </>
        )}

        {status === 'error' && (
          <>
            <p className="text-signal-buy mb-4">生成二维码失败</p>
            <button 
              onClick={generateQRCode}
              className="btn-primary"
            >
              重试
            </button>
          </>
        )}
      </div>
    </div>
  );
}