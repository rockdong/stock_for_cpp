import { useState, useEffect, useCallback } from 'react';
import { useNavigate, useSearchParams } from 'react-router-dom';
import { authApi } from '../../services/authApi';
import { feishuAuthApi } from '../../services/feishuAuthApi';
import { tokenStorage } from '../../utils/tokenStorage';

export default function LoginPage() {
  const navigate = useNavigate();
  const [searchParams] = useSearchParams();
  const [qrUrl, setQrUrl] = useState<string>('');
  const [sessionId, setSessionId] = useState<string>('');
  const [status, setStatus] = useState<'loading' | 'pending' | 'success' | 'expired' | 'error'>('loading');
  const [countdown, setCountdown] = useState<number>(300);

  const callbackToken = searchParams.get('token');
  const callbackName = searchParams.get('name');
  const callbackError = searchParams.get('error');

  useEffect(() => {
    if (callbackToken) {
      tokenStorage.save(callbackToken);
      setStatus('success');
      setTimeout(() => {
        navigate('/analysis');
      }, 1000);
    } else if (callbackError) {
      setStatus('error');
    }
  }, [callbackToken, callbackError, navigate]);

  const generateQRCode = useCallback(async () => {
    setStatus('loading');
    try {
      const result = await feishuAuthApi.getQRCode();
      setQrUrl(result.qr_url);
      setSessionId(result.session_id);
      setCountdown(result.expires_in || 300);
      setStatus('pending');
    } catch (error) {
      console.error('生成飞书登录二维码失败:', error);
      setStatus('error');
    }
  }, []);

  useEffect(() => {
    if (status !== 'pending' || !sessionId) return;

    const interval = setInterval(async () => {
      try {
        const result = await feishuAuthApi.getStatus(sessionId);
        
        if (result.status === 'success') {
          setStatus('success');
          clearInterval(interval);
        } else if (result.status === 'expired' || result.is_expired) {
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
    if (!callbackToken && !callbackError) {
      generateQRCode();
    }
  }, [generateQRCode, callbackToken, callbackError]);

  return (
    <div className="min-h-screen bg-base flex items-center justify-center">
      <div className="bg-surface rounded-xl border border-border-default p-8 max-w-md w-full text-center">
        <h1 className="text-2xl font-semibold text-text-primary mb-2">
          飞书扫码登录
        </h1>
        <p className="text-text-tertiary mb-6">
          扫码后请在飞书 App 内确认授权
        </p>

        {status === 'loading' && (
          <div className="text-text-tertiary">正在生成登录二维码...</div>
        )}

        {status === 'pending' && qrUrl && (
          <>
            <img 
              src={qrUrl} 
              alt="飞书登录二维码" 
              className="w-64 h-64 mx-auto mb-4 rounded-lg"
            />
            <p className="text-text-tertiary text-sm">
              请使用飞书 App 扫描二维码
            </p>
            <p className="text-text-tertiary text-sm mt-1">
              扫码后在飞书 App 内点击"确认授权"
            </p>
            <p className="text-accent-amber text-sm mt-2">
              有效时间: {countdown}秒
            </p>
          </>
        )}

        {status === 'success' && (
          <div className="text-signal-sell">
            <span className="text-4xl mb-4">✓</span>
            <p>登录成功，正在跳转...</p>
            {callbackName && (
              <p className="text-text-secondary text-sm mt-2">
                欢迎，{callbackName}
              </p>
            )}
          </div>
        )}

        {status === 'expired' && (
          <>
            <p className="text-signal-buy mb-4">二维码已过期</p>
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
            <p className="text-signal-buy mb-4">登录失败</p>
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