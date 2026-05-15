import { useState } from 'react';
import { useNavigate, Link } from 'react-router-dom';
import { userAuthApi } from '../../services/userAuthApi';
import { tokenStorage } from '../../utils/tokenStorage';

export default function RegisterPage() {
  const navigate = useNavigate();
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');
  const [isLoading, setIsLoading] = useState(false);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError('');
    setIsLoading(true);

    try {
      const result = await userAuthApi.register({ username, password });

      if (result.success && result.data?.token) {
        tokenStorage.save(result.data.token);
        navigate('/analysis');
      } else {
        setError(result.message || '注册失败');
      }
    } catch (err: any) {
      console.error('注册错误:', err);
      setError(err.response?.data?.message || '网络错误，请稍后重试');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-base flex items-center justify-center relative overflow-hidden">
      <div className="absolute inset-0 overflow-hidden pointer-events-none">
        <div className="absolute -top-40 -right-40 w-80 h-80 bg-accent-blue/5 rounded-full blur-3xl" />
        <div className="absolute -bottom-40 -left-40 w-80 h-80 bg-accent-blue/3 rounded-full blur-3xl" />
      </div>

      <div className="bg-surface rounded-xl border border-border-default p-8 max-w-md w-full mx-4 relative z-10">
        <h1 className="text-2xl font-semibold text-text-primary text-center mb-2">
          注册
        </h1>
        <p className="text-text-tertiary text-center text-sm mb-6">
          注册账号后即可查看股票分析结果
        </p>

        {error && (
          <div className="mb-4 p-3 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm">
            {error}
          </div>
        )}

        <form onSubmit={handleSubmit} className="space-y-4">
          <div>
            <label
              htmlFor="username"
              className="block text-sm font-medium text-text-secondary mb-1.5"
            >
              用户名
            </label>
            <input
              id="username"
              type="text"
              value={username}
              onChange={(e) => setUsername(e.target.value)}
              className="w-full px-4 py-3 bg-elevated border border-border-default rounded-lg text-text-primary placeholder:text-text-tertiary focus:outline-none focus:border-accent-blue focus:ring-1 focus:ring-accent-blue/30 transition-colors"
              placeholder="3-20字符，字母/数字/下划线"
              required
              disabled={isLoading}
              autoComplete="username"
            />
            <p className="text-xs text-text-tertiary mt-1">
              3-20字符，仅允许字母、数字、下划线
            </p>
          </div>

          <div>
            <label
              htmlFor="password"
              className="block text-sm font-medium text-text-secondary mb-1.5"
            >
              密码
            </label>
            <input
              id="password"
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              className="w-full px-4 py-3 bg-elevated border border-border-default rounded-lg text-text-primary placeholder:text-text-tertiary focus:outline-none focus:border-accent-blue focus:ring-1 focus:ring-accent-blue/30 transition-colors"
              placeholder="6-32字符，至少包含字母和数字"
              required
              disabled={isLoading}
              autoComplete="new-password"
            />
            <p className="text-xs text-text-tertiary mt-1">
              6-32字符，至少包含字母和数字
            </p>
          </div>

          <button
            type="submit"
            disabled={isLoading || !username || !password}
            className="w-full btn--primary py-3 mt-2 disabled:opacity-50 disabled:cursor-not-allowed"
          >
            {isLoading ? (
              <span className="flex items-center justify-center gap-2">
                <svg className="animate-spin h-4 w-4" viewBox="0 0 24 24">
                  <circle
                    className="opacity-25"
                    cx="12"
                    cy="12"
                    r="10"
                    stroke="currentColor"
                    strokeWidth="4"
                    fill="none"
                  />
                  <path
                    className="opacity-75"
                    fill="currentColor"
                    d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"
                  />
                </svg>
                注册中...
              </span>
            ) : (
              '注册'
            )}
          </button>
        </form>

        <div className="mt-6 text-center">
          <span className="text-text-tertiary text-sm">已有账号？</span>
          <Link
            to="/login"
            className="text-sm text-accent-blue hover:text-accent-blue/80 ml-1 transition-colors"
          >
            点击登录
          </Link>
        </div>
      </div>
    </div>
  );
}