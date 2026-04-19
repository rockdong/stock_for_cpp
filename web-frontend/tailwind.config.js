/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        // 背景色阶
        base: '#0B0E11',
        surface: '#141820',
        elevated: '#1C2333',
        overlay: '#232A3B',
        
        // 边框
        'border-default': '#2A3142',
        'border-subtle': '#1E2536',
        
        // 文字色阶
        'text-primary': '#E8ECF1',
        'text-secondary': '#8B95A5',
        'text-tertiary': '#5C6678',
        'text-inverse': '#0B0E11',
        
        // 信号色
        'signal-buy': '#EF4444',
        'signal-buy-bg': 'rgba(239,68,68,0.12)',
        'signal-sell': '#22C55E',
        'signal-sell-bg': 'rgba(34,197,94,0.12)',
        'signal-hold': '#94A3B8',
        
        // 强调色
        accent: {
          blue: '#3B82F6',
          'blue-bg': 'rgba(59,130,246,0.12)',
          amber: '#F59E0B',
          purple: '#8B5CF6',
        },
      },
      fontFamily: {
        sans: ['IBM Plex Sans', 'system-ui', 'sans-serif'],
        mono: ['JetBrains Mono', 'monospace'],
      },
      fontSize: {
        '2xs': ['0.625rem', { lineHeight: '0.875rem' }],
      },
      borderRadius: {
        '2xl': '1rem',
        '3xl': '1.25rem',
      },
      boxShadow: {
        'card': '0 1px 3px rgba(0,0,0,0.3), 0 1px 2px rgba(0,0,0,0.2)',
        'card-hover': '0 10px 25px rgba(0,0,0,0.4), 0 4px 10px rgba(0,0,0,0.3)',
        'modal': '0 25px 50px rgba(0,0,0,0.5)',
        'glow-blue': '0 0 20px rgba(59,130,246,0.15)',
        'glow-red': '0 0 20px rgba(239,68,68,0.15)',
        'glow-green': '0 0 20px rgba(34,197,94,0.15)',
      },
      animation: {
        'slide-up': 'slideUp 0.25s ease-out',
        'slide-down': 'slideDown 0.2s ease-in',
        'fade-in': 'fadeIn 0.2s ease-out',
        'pulse-slow': 'pulse 2s cubic-bezier(0.4, 0, 0.6, 1) infinite',
        'shimmer': 'shimmer 1.5s infinite',
      },
      keyframes: {
        slideUp: {
          '0%': { transform: 'translateY(20px)', opacity: '0' },
          '100%': { transform: 'translateY(0)', opacity: '1' },
        },
        slideDown: {
          '0%': { transform: 'translateY(0)', opacity: '1' },
          '100%': { transform: 'translateY(20px)', opacity: '0' },
        },
        fadeIn: {
          '0%': { opacity: '0' },
          '100%': { opacity: '1' },
        },
        shimmer: {
          '0%': { backgroundPosition: '-200% 0' },
          '100%': { backgroundPosition: '200% 0' },
        },
      },
    },
  },
  plugins: [],
}
