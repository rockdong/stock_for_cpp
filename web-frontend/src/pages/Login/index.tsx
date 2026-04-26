import { useState, useEffect, useCallback } from 'react';
import { useNavigate } from 'react-router-dom';
import { authApi } from '../../services/authApi';
import { tokenStorage } from '../../utils/tokenStorage';

export default function LoginPage() {
  const navigate = useNavigate();
  const [qrUrl, setQrUrl] = useState<string>('');
  const [qrType, setQrType] = useState<string>('qrcode');
  const [sessionId, setSessionId] = useState<string>('');
  const [status, setStatus] = useState<'loading' | 'pending' | 'success' | 'expired' | 'error'>('loading');
  const [countdown, setCountdown] = useState<number>(60);

  const generateQRCode = useCallback(async () => {
    setStatus('loading');
    try {
      const result = await authApi.getQRCode();
      setQrUrl(result.qr_url);
      setQrType(result.qr_type || 'qrcode');
      setSessionId(result.session_id);
      setCountdown(result.expires_in || 60);
      setStatus('pending');
    } catch (error) {
      console.error('生成二维码失败:', error);
      setStatus('error');
    }
  }, []);

  const getQRCodeImageUrl = () => {
    if (qrType === 'qrcode') {
      return qrUrl;
    }
    return `https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=${encodeURIComponent(qrUrl)}`;
  };

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
    }, 3000);

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
              src={getQRCodeImageUrl()} 
              alt="微信登录二维码" 
              className="w-64 h-64 mx-auto mb-4 rounded-lg"
            />
            <p className="text-text-tertiary text-sm">
              请使用微信扫描二维码关注公众号
            </p>
            <p className="text-text-tertiary text-sm mt-1">
              关注后将自动完成登录（约5-10秒）
            </p>
            <p className="text-accent-amber text-sm mt-2">
              等待时间: {countdown}秒
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
            <p className="text-signal-buy mb-4">等待超时</p>
            <button 
              onClick={generateQRCode}
              className="btn-primary"
            >
              重新登录
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